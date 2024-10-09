import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals

def find_insertion_lines():
        ###############################################################################
        # Run the closest point search
        ###############################################################################

        # convert element polylines to a flattened dictionary:
        segments_dictionary = {}
        # stores ids e.g. int , [element_int,bottom_or_top_polyline_int,edge,int, bounding_box, hit_count, point_id]
        vectors = []
        count = 0
        for i in range(len(polylines)):
            vectors.append([Vector3d.Zero] * (polylines[i][0].SegmentCount + 2))
            for j in range(len(polylines[i])):
                for k in range(polylines[i][j].SegmentCount):
                    bbox = polylines[i][j].SegmentAt(k).BoundingBox
                    bbox.Inflate(0.02)
                    segments_dictionary[count] = [i, j, k, bbox, polylines[i][j].SegmentAt(k)]

                    count = count + 1

        # create a tree
        rtree = Rhino.Geometry.RTree()

        # fill the tree
        for i in segments_dictionary:
            rtree.Insert(segments_dictionary[i][3], i)

        # call_backs of rtree search
        def search_callback(sender, rtree_event_args):
            data_by_reference.append(rtree_event_args.Id)

        # perform search, two times first for start points of a polyline, second for the end point
        for i in range(len(lines)):
            data_by_reference = []
            if rtree.Search(Sphere(lines[i].From, 0), search_callback, data_by_reference):
                for j in data_by_reference:
                    if (
                        lines[i].From.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].From, True))
                        < 0.001
                    ):
                        vectors[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = lines[i].Direction
                        # vectors[segments_dictionary[j][0]][segments_dictionary[j][2]+2].Unitize()

        for i in range(len(lines)):
            data_by_reference = []
            if rtree.Search(Sphere(lines[i].To, 0), search_callback, data_by_reference):
                for j in data_by_reference:
                    if lines[i].To.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].To, True)) < 0.001:
                        vectors[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = -lines[i].Direction
                        # vectors[segments_dictionary[j][0]][segments_dictionary[j][2]+2].Unitize()

        ###############################################################################
        # Output
        ###############################################################################
        _polylines_out = polylines_out
        _vectors = th.list_to_tree(vectors)

        # return outputs if you have them; here I try it for you:
        return (_polylines_out, _vectors)


def command_line_input() -> Tuple[str, List[Rhino.Geometry.Line]]:

    # Create an instance of GetOption to define options in one line
    get_options: Rhino.Input.Custom.GetOption = Rhino.Input.Custom.GetOption()

    # Populate options from wood_rui_globals.datasets.keys()

    case_names = list(wood_rui_globals.dataset.keys())

    # Check if case_names is empty
    if not case_names:
        Rhino.RhinoApp.WriteLine("\nATTENTION: First create geometry, set joint parameters, and then run the solver.\n")
        return Rhino.Commands.Result.Cancel  # Exit if no datasets are found

    # Add an option to select lines
    get_options.AddOption("select_lines")

    # Add case names as options
    for case_name in case_names:
        get_options.AddOption(case_name)

    Rhino.RhinoApp.WriteLine("\n─ Select a case and lines. Hit 'Enter' when done. ─")
    get_options.SetCommandPrompt("Select a case or lines.")

    # Initialize storage for the case name and selected lines
    selected_case_name = None
    selected_lines = []

    # Run the option selection loop
    while True:
        res = get_options.Get()

        # Check if the operation was cancelled or Enter was pressed (GetResult.Nothing)
        if res == Rhino.Input.GetResult.Nothing or res == Rhino.Input.GetResult.Cancel:
            if not selected_case_name:
                selected_case_name = case_names[0]  # Default to the first case if none was selected
            Rhino.RhinoApp.WriteLine("Selection completed.")
            break

        # If an option is selected
        if res == Rhino.Input.GetResult.Option:
            option_name = get_options.Option().EnglishName

            # If user selects the option to choose lines
            if option_name == "select_lines":
                Rhino.RhinoApp.WriteLine("\n─ Select the lines. Press Enter when done. ─")
                
                # Create an instance of GetObject to select curves (lines)
                go = Rhino.Input.Custom.GetObject()
                go.SetCommandPrompt("Select lines")
                go.GeometryFilter = Rhino.DocObjects.ObjectType.Curve  # Filter only curves (includes lines)
                go.SubObjectSelect = False  # Don't allow subobject selection
                go.EnablePreSelect(True, True)  # Allow pre-selection of objects
                go.GetMultiple(1, 0)  # Allow selecting multiple lines, 1 as the minimum

                # Check the result of the line selection
                if go.CommandResult() != Rhino.Commands.Result.Success:
                    Rhino.RhinoApp.WriteLine("Failed to select lines.")
                    continue  # Continue to let user select more lines

                # Process the selected lines
                for i in range(go.ObjectCount):
                    obj_ref = go.Object(i)
                    curve = obj_ref.Curve()
                    if curve:
                        selected_lines.append(curve)

                Rhino.RhinoApp.WriteLine(f"Total lines selected: {len(selected_lines)}")

            # If the user selects a case
            elif option_name in case_names:
                selected_case_name = option_name
                Rhino.RhinoApp.WriteLine(f"Case selected: {selected_case_name}")

    # Handle the datasets based on user input
    print(selected_case_name, selected_lines)
    return selected_case_name, selected_lines

if __name__ == "__main__":
    
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms

    command_line_input()



    case_name = "your_case_name"  # Replace with the actual case name
    # polylines_guid_list = wood_rui_globals[case_name]["polylines_guid"]

    # # Iterate through the list two items at a time
    # for i in range(0, len(polylines_guid_list), 2):
    #     # Get the current GUIDs for both first and second items
    #     first_guid = polylines_guid_list[i]  # Current index
    #     second_guid = polylines_guid_list[i + 1]  # Next index (always exists)

    #     # Process the first object
    #     rhino_object_first = Rhino.RhinoDoc.ActiveDoc.Objects.Find(first_guid)
    #     for idx, v in enumerate(vectors[int(i * 0.5)]):
    #         name = f"insertion_vector_{idx}"  # Use the same index for both
    #         value = f"{v.X} {v.Y} {v.Z}"  # Ensure you are referencing the correct coordinates
    #         rhino_object_first.Attributes.SetUserString(name, value)

    #     # Process the second object
    #     rhino_object_second = Rhino.RhinoDoc.ActiveDoc.Objects.Find(second_guid)
    #     for idx, v in enumerate(vectors[int(i * 0.5)]):
    #         name = f"insertion_vector_{idx}"  # Same name pattern
    #         value = f"{v.X} {v.Y} {v.Z}"  # Ensure you are referencing the correct coordinates
    #         rhino_object_second.Attributes.SetUserString(name, value)
