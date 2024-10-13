import Rhino
import Rhino.Input
import Rhino.DocObjects
import System
from typing import List, Tuple, Dict, Any
from wood_rui import wood_rui_globals, ensure_layer_exists, get_objects_by_layer, add_joint_type
import ast


def closest_points(dataset_name: str, points: List[Rhino.Geometry.Point3d], points_ids: List[int]) -> None:
    # _polylines: Grasshopper.DataTree[Rhino.Geometry.Curve],
    # _points: Grasshopper.DataTree[Rhino.Geometry.Point3d],
    # _joint_types: Grasshopper.DataTree[int]

    """component iterates polyline edges and assigns index based on the point list and their type RTree search

    Match the closest lines from the selected dataset to the input lines and update
    the insertion vectors.

    Parameters
    ----------
    dataset_name : str
        The name of the dataset to use.
    lines : List[Rhino.Geometry.Line]
        List of lines to compare against the polylines in the dataset.
    """

    ###############################################################################
    # Conversion
    ###############################################################################
    # Flat list of polylines
    flat_polylines: List[Any] = wood_rui_globals[dataset_name]["polylines"]

    # Iterate over the flat list in steps of 2
    polylines = []
    for i in range(0, len(flat_polylines), 2):
        # Each sublist will contain two consecutive points
        sublist: List[Any] = [flat_polylines[i], flat_polylines[i + 1]]
        polylines.append(sublist)

    ###############################################################################
    # Run the closest point search
    ###############################################################################

    # convert element polylines to a flattened dictionary:
    segments_dictionary = {}
    # stores ids e.g. int , [element_int,bottom_or_top_polyline_int,edge,int, bounding_box, hit_count, point_id]
    joint_types = []
    count = 0
    for i in range(len(polylines)):
        joint_types.append([-1] * (polylines[i][0].SegmentCount + 2))
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

    # call_backs of rtree
    def search_callback(sender, rtree_event_args):
        data_by_reference.append(rtree_event_args.Id)

    # perform search
    for i in range(len(points)):
        data_by_reference = []
        if rtree.Search(Rhino.Geometry.Sphere(points[i], 0), search_callback, data_by_reference):
            for j in data_by_reference:
                if points[i].DistanceToSquared(segments_dictionary[j][4].ClosestPoint(points[i], True)) < 0.001:
                    joint_types[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = points_ids[i]

    ###############################################################################
    # Output
    ###############################################################################
    wood_rui_globals[dataset_name]["joints_per_face"]
    add_joint_type(joint_types, dataset_name)

    # _polylines_out = polylines_out
    # _joints_types_out = th.list_to_tree(joint_types)

    # # return outputs if you have them; here I try it for you:
    # return (_polylines_out, _joints_types_out)


def command_line_input() -> Tuple[str, List[Rhino.Geometry.Point3d], List[int]]:
    """
    Automatically selects all Rhino Text Dots from the nested layer "compas_wood > selected_case_name > joint_types".
    Converts the Text Dots into points and attempts to convert their text to integers.
    If conversion fails, assigns -1 to that value.

    Returns
    -------
    Tuple[str, List[Rhino.Geometry.Point3d], List[int]]
        Selected dataset name and lists of points and corresponding integer values (or -1 if not convertible).
    """

    # Define base and target layer names
    base_layer_name = "compas_wood"
    child_layer_name = "joint_types"

    # Populate options from wood_rui_globals.datasets.keys()
    case_names: List[str] = list(wood_rui_globals.dataset.keys())

    if not case_names:
        Rhino.RhinoApp.WriteLine("\nATTENTION: First create geometry, set joint parameters, and then run the solver.\n")
        return Rhino.Commands.Result.Cancel, [], []

    # Create an instance of GetOption to allow the user to select a case
    get_options: Rhino.Input.Custom.GetOption = Rhino.Input.Custom.GetOption()
    for case_name in case_names:
        get_options.AddOption(case_name)

    Rhino.RhinoApp.WriteLine("\n─ Select a case. Hit 'Enter' when done. ─")
    get_options.SetCommandPrompt("Select a case.")

    selected_case_name: str = ""

    # Get user's case selection
    while True:
        res = get_options.Get()

        if res == Rhino.Input.GetResult.Nothing or res == Rhino.Input.GetResult.Cancel:
            if not selected_case_name:
                selected_case_name = case_names[0]  # Default to the first case if none is selected
            Rhino.RhinoApp.WriteLine(f"Case selected: {selected_case_name}")
            break

        if res == Rhino.Input.GetResult.Option:
            option_name: str = get_options.Option().EnglishName
            if option_name in case_names:
                selected_case_name = option_name
                Rhino.RhinoApp.WriteLine(f"Case selected: {selected_case_name}")
                break

    # Construct full layer path: "compas_wood > selected_case_name > joint_types"
    full_layer_name = f"{base_layer_name}::{selected_case_name}::{child_layer_name}"

    # Use FindByLayer to get all objects in the target layer
    text_dot_objects = get_objects_by_layer(full_layer_name)

    selected_points: List[Rhino.Geometry.Point3d] = []
    selected_values: List[int] = []

    # Iterate through only the TextDots on the desired layer
    if text_dot_objects:
        for obj in text_dot_objects:
            if isinstance(obj.Geometry, Rhino.Geometry.TextDot):
                text_dot = obj.Geometry
                selected_points.append(text_dot.Point)

                # Try converting the text to an integer using ast.literal_eval
                try:
                    value = ast.literal_eval(text_dot.Text)
                    if not isinstance(value, int):  # Ensure it's an integer
                        value = -1
                except (ValueError, SyntaxError):
                    value = -1
                selected_values.append(value)

    Rhino.RhinoApp.WriteLine(f"Total text dots found on layer '{full_layer_name}': {len(selected_points)}")
    return selected_case_name, selected_points, selected_values


if __name__ == "__main__":

    selected_case_name, selected_points, selected_values = command_line_input()
    if selected_case_name and selected_points and selected_values:
        closest_points(selected_case_name, selected_points, selected_values)
