import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import List, Tuple, Dict, Any
from wood_rui import wood_rui_globals

def closest_lines(dataset_name: str, lines: List[Rhino.Geometry.Line]) -> None:
    """
    Match the closest lines from the selected dataset to the input lines and update
    the insertion vectors.

    Parameters
    ----------
    dataset_name : str
        The name of the dataset to use.
    lines : List[Rhino.Geometry.Line]
        List of lines to compare against the polylines in the dataset.
    """
    
    # Convert polylines to pairs
    polylines: List[List[Any]] = []  # This will hold the list of lists

    # Flat list of polylines
    flat_polylines: List[Any] = wood_rui_globals[dataset_name]["polylines"]

    # Iterate over the flat list in steps of 2
    for i in range(0, len(flat_polylines), 2):
        # Each sublist will contain two consecutive points
        sublist: List[Any] = [flat_polylines[i], flat_polylines[i + 1]]
        polylines.append(sublist)

    # Convert element polylines to a flattened dictionary
    segments_dictionary: Dict[int, List[Any]] = {}
    vectors: List[List[Rhino.Geometry.Vector3d]] = []
    count: int = 0
    
    # Populate segments dictionary and initialize vectors
    for i in range(len(polylines)):
        vectors.append([Rhino.Geometry.Vector3d.Zero] * (polylines[i][0].SegmentCount + 2))
        for j in range(len(polylines[i])):
            for k in range(polylines[i][j].SegmentCount):
                bbox = polylines[i][j].SegmentAt(k).BoundingBox
                bbox.Inflate(0.02)
                segments_dictionary[count] = [i, j, k, bbox, polylines[i][j].SegmentAt(k)]
                count += 1

    # Create and fill the RTree
    rtree = Rhino.Geometry.RTree()
    for i in segments_dictionary:
        rtree.Insert(segments_dictionary[i][3], i)

    # Callbacks for RTree search
    def search_callback(sender: Any, rtree_event_args: Rhino.Geometry.RTreeEventArgs) -> None:
        data_by_reference.append(rtree_event_args.Id)

    # Perform search for both start and end points of each line
    for i in range(len(lines)):
        data_by_reference: List[int] = []
        if rtree.Search(Rhino.Geometry.Sphere(lines[i].From, 0), search_callback, data_by_reference):
            for j in data_by_reference:
                if lines[i].From.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].From, True)) < 0.001:
                    vectors[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = lines[i].Direction

    for i in range(len(lines)):
        data_by_reference: List[int] = []
        if rtree.Search(Rhino.Geometry.Sphere(lines[i].To, 0), search_callback, data_by_reference):
            for j in data_by_reference:
                if lines[i].To.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].To, True)) < 0.001:
                    vectors[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = -lines[i].Direction

    # Store the insertion vectors in the global data structure
    wood_rui_globals[dataset_name]["insertion_vectors"] = vectors
    print("Number of insertion vectors: ", len(wood_rui_globals[dataset_name]["insertion_vectors"]))

    # Update Rhino object attributes
    polylines_guid_list = wood_rui_globals[dataset_name]["polylines_guid"]
    for i in range(0, len(polylines_guid_list), 2):
        first_guid = polylines_guid_list[i]
        second_guid = polylines_guid_list[i + 1]

        rhino_object_first = Rhino.RhinoDoc.ActiveDoc.Objects.Find(first_guid)
        rhino_object_second = Rhino.RhinoDoc.ActiveDoc.Objects.Find(second_guid)

        for idx, v in enumerate(vectors[int(i * 0.5)]):
            name = f"insertion_vector_{idx}"
            value = f"{v.X} {v.Y} {v.Z}"
            rhino_object_first.Attributes.SetUserString(name, value)
            rhino_object_second.Attributes.SetUserString(name, value)


def command_line_input() -> Tuple[str, List[Rhino.Geometry.Line]]:
    """
    Loads datasets and selects lines based on user input from the command line.

    Returns
    -------
    Tuple[str, List[Rhino.Geometry.Line]]
        Selected dataset name and list of lines.
    """
    
    # Create an instance of GetOption to define options
    get_options: Rhino.Input.Custom.GetOption = Rhino.Input.Custom.GetOption()

    # Populate options from wood_rui_globals.datasets.keys()
    case_names: List[str] = list(wood_rui_globals.dataset.keys())

    if not case_names:
        Rhino.RhinoApp.WriteLine("\nATTENTION: First create geometry, set joint parameters, and then run the solver.\n")
        return Rhino.Commands.Result.Cancel, []

    get_options.AddOption("select_lines")

    for case_name in case_names:
        get_options.AddOption(case_name)

    Rhino.RhinoApp.WriteLine("\n─ Select a case and lines. Hit 'Enter' when done. ─")
    get_options.SetCommandPrompt("Select a case or lines.")

    selected_case_name: str = ""
    selected_lines: List[Rhino.Geometry.Line] = []

    while True:
        res = get_options.Get()

        if res == Rhino.Input.GetResult.Nothing or res == Rhino.Input.GetResult.Cancel:
            if not selected_case_name:
                selected_case_name = case_names[0]
            Rhino.RhinoApp.WriteLine("Selection completed.")
            break

        if res == Rhino.Input.GetResult.Option:
            option_name: str = get_options.Option().EnglishName

            if option_name == "select_lines":
                Rhino.RhinoApp.WriteLine("\n─ Select the lines. Press Enter when done. ─")
                go = Rhino.Input.Custom.GetObject()
                go.SetCommandPrompt("Select lines")
                go.GeometryFilter = Rhino.DocObjects.ObjectType.Curve
                go.SubObjectSelect = False
                go.EnablePreSelect(True, True)
                go.GetMultiple(1, 0)

                if go.CommandResult() != Rhino.Commands.Result.Success:
                    Rhino.RhinoApp.WriteLine("Failed to select lines.")
                    continue

                for i in range(go.ObjectCount):
                    obj_ref = go.Object(i)
                    curve = obj_ref.Curve()
                    if curve:
                        selected_lines.append(Rhino.Geometry.Line(curve.PointAtStart, curve.PointAtEnd))

                Rhino.RhinoApp.WriteLine(f"Total lines selected: {len(selected_lines)}")

            elif option_name in case_names:
                selected_case_name = option_name
                Rhino.RhinoApp.WriteLine(f"Case selected: {selected_case_name}")

    return selected_case_name, selected_lines


if __name__ == "__main__":
    selected_case_name, selected_lines = command_line_input()
    if selected_case_name and selected_lines:
        closest_lines(selected_case_name, selected_lines)
