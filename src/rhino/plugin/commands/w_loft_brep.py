#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, add_loft_brep
from System.Drawing import Color
from compas_wood.binding import wood_globals


def project_curves_to_plane(curves: List[Rhino.Geometry.Curve]) -> List[Rhino.Geometry.Curve]:
    """
    Projects a list of curves onto a best-fit plane.

    Parameters
    ----------
    curves : List[Rhino.Geometry.Curve]
        List of curves to project onto a plane.

    Returns
    -------
    List[Rhino.Geometry.Curve]
        A list of projected curves.
    """
    points = []
    for curve in curves:
        for point in curve.ToNurbsCurve().Points:
            points.append(point.Location)

    result, plane = Rhino.Geometry.Plane.FitPlaneToPoints(points)

    xform = Rhino.Geometry.Transform.PlanarProjection(plane)
    curves_projected = []
    for curve in curves:
        curve_projected = curve.DuplicateCurve()
        curve_projected.Transform(xform)
        curves_projected.append(curve_projected)
    return curves_projected


def loft_polylines_with_holes(
    curves0: List[Rhino.Geometry.Polyline], curves1: List[Rhino.Geometry.Polyline] = None, color: Optional[Color] = None
) -> Optional[Rhino.Geometry.Brep]:
    """
    Creates a lofted Brep between two sets of polylines, handling holes if necessary.

    Parameters
    ----------
    curves0 : List[Rhino.Geometry.Polyline]
        First set of polylines to loft.
    curves1 : Optional[List[Rhino.Geometry.Polyline]]
        Second set of polylines to loft. If not provided, lofting will be skipped.
    color : Optional[Color]
        Color to apply to the Brep faces. If None, a default color is used.

    Returns
    -------
    Optional[Rhino.Geometry.Brep]
        A lofted Brep or None if invalid input is provided.
    """

    ###############################################################################
    # user input
    ###############################################################################

    if curves0 == [None]:
        return
    if curves1 == [None]:
        return

    if len(curves0) == 0:
        return
    if len(curves1) == 0:
        return

    flag = len(curves0) != 0 if True else len(curves1) != 0
    if flag:
        curves = []
        curves2 = []

        flag0 = len(curves1) == 0
        flag1 = len(curves0) == 0 and len(curves1) != 0
        flag2 = len(curves0) and len(curves1)

        if flag0:
            for i in range(len(curves0)):
                if float(i) >= float(len(curves0)) * 0.5:
                    curves2.Add(curves0[i])
                else:
                    curves.Add(curves0[i])
        elif flag1:
            for i in range(0, len(curves1), 2):
                curves.Add(curves1[i])
                curves2.Add(curves1[i + 1])
        elif flag2:
            curves = curves0
            curves2 = curves1

        curves0 = curves
        curves1 = curves2

    ###############################################################################
    # Convert Polylines to NurbsCurve
    ###############################################################################
    for i in range(len(curves0)):
        # skip lines
        if curves0[i].Count < 3:
            return
        curves0[i] = curves0[i].ToNurbsCurve()

    for i in range(len(curves1)):
        # skip lines
        if curves1[i].Count < 3:
            return
        curves1[i] = curves1[i].ToNurbsCurve()

    ###############################################################################
    # Create mesh of the bottom face
    ###############################################################################

    curves0 = project_curves_to_plane(curves0)
    curves1 = project_curves_to_plane(curves1)

    breps_0 = Rhino.Geometry.Brep.CreatePlanarBreps(curves0, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
    if not breps_0:
        return

    breps_1 = Rhino.Geometry.Brep.CreatePlanarBreps(curves1, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
    if not breps_1:
        return

    brep_0 = breps_0[0]
    brep_1 = breps_1[0]

    ###############################################################################
    # Create lofts
    ###############################################################################
    breps_to_join = [brep_0, brep_1]
    for i in range(len(curves0)):
        wall = Rhino.Geometry.Brep.CreateFromLoft(
            [curves0[i], curves1[i]],
            Rhino.Geometry.Point3d.Unset,
            Rhino.Geometry.Point3d.Unset,
            Rhino.Geometry.LoftType.Normal,
            False,
        )[0]
        breps_to_join.append(wall)

    ###############################################################################
    # Join Brep
    ###############################################################################

    solid = Rhino.Geometry.Brep.JoinBreps(breps_to_join, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

    faces = []
    for i in range(solid.Faces.Count):
        faces.append(solid.Faces[i].DuplicateFace(False))

    solid = Rhino.Geometry.Brep.JoinBreps(faces, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

    if solid:
        solid.Faces.SplitKinkyFaces(Rhino.RhinoMath.DefaultAngleTolerance, True)

        if Rhino.Geometry.BrepSolidOrientation.Inward is solid.SolidOrientation:
            solid.Flip()

    if color is not None:
        for i in range(solid.Faces.Count):
            solid.Faces[i].PerFaceColor = color
    else:
        for i in range(solid.Faces.Count):
            if i < 2:
                solid.Faces[i].PerFaceColor = Color.LightGray  # Color.LightGray
            else:
                solid.Faces[i].PerFaceColor = Color.DodgerBlue

    return solid


def command_line_input() -> (str, str):
    """
    Gets user input for selecting a dataset from wood_rui_globals and a data type.

    Returns:
        tuple: The name of the selected dataset and the selected data type, or empty strings if cancelled.
    """
    # All datasets as a list of strings.
    datasets: List[str] = list(wood_rui_globals.dataset.keys())

    # Create an instance of GetOption to allow user to choose a dataset
    get_options = Rhino.Input.Custom.GetOption()

    # Add each dataset as an option
    for dataset in datasets:
        get_options.AddOption(dataset)

    get_options.SetCommandPrompt("Select a dataset from wood_rui_globals:")

    while True:
        res = get_options.Get()

        if res == Rhino.Input.GetResult.Option:
            selected_index = get_options.OptionIndex() - 1  # Adjusting for 1-based index
            selected_dataset = datasets[selected_index]
            print(f"Selected dataset: {selected_dataset}")
            break

        elif res == Rhino.Input.GetResult.Cancel:
            print("Selection cancelled.")
            return "", ""

    # Create an instance of GetOption to allow user to choose a data type
    get_options = Rhino.Input.Custom.GetOption()
    data_types = ["joinery", "polylines"]

    # Add each data type as an option
    for data_type in data_types:
        get_options.AddOption(data_type)

    get_options.SetCommandPrompt("Select a data type (joinery/polylines):")

    while True:
        res = get_options.Get()

        if res == Rhino.Input.GetResult.Option:
            selected_index = get_options.OptionIndex() - 1  # Adjusting for 1-based index
            selected_data_type = data_types[selected_index]
            print(f"Selected data type: {selected_data_type}")
            return selected_dataset, selected_data_type

        elif res == Rhino.Input.GetResult.Cancel:
            print("Selection cancelled.")
            return "", ""


if __name__ == "__main__":

    dataset_name, type_name = command_line_input()

    lists_breps = []
    element_ids = []
    if type_name == "joinery":

        # check if type is polyline or just lofts
        are_plates: bool = wood_globals.output_geometry_type == 4

        # for types in wood_rui_globals[dataset_name][type_name]:
        #     print("___", types)
        #     for curr_type in types:
        #         if curr_type == 'edge_insertion' or curr_type == 'hole' or curr_type == 'insert_between_multiple_edges':
        #             are_plates = True
        #             print("are_plates")
        #             break
        #     if are_plates:
        #         break

        if are_plates:

            for i in range(len(wood_rui_globals[dataset_name][type_name])):
                polylines0 = []
                polylines1 = []
                for j in range(0, len(wood_rui_globals[dataset_name][type_name][i]), 2):
                    pline0 = wood_rui_globals[dataset_name][type_name][i][j + 0]
                    pline1 = wood_rui_globals[dataset_name][type_name][i][j + 1]
                    polylines0.append(pline0)
                    polylines1.append(pline1)

                brep = loft_polylines_with_holes(polylines0, polylines1)
                lists_breps.append([brep])
                element_ids.append(i)

        else:

            for i in range(len(wood_rui_globals[dataset_name][type_name])):
                breps = []
                for j in range(0, len(wood_rui_globals[dataset_name][type_name][i]), 2):
                    pline0 = wood_rui_globals[dataset_name][type_name][i][j + 0]
                    pline1 = wood_rui_globals[dataset_name][type_name][i][j + 1]
                    brep = loft_polylines_with_holes([pline0], [pline1])
                    if brep:
                        breps.append(brep)
                lists_breps.append(breps)
                element_ids.append(i)

    elif type_name == "polylines":
        for i in range(0, len(wood_rui_globals[dataset_name][type_name]), 2):
            polylines0 = [wood_rui_globals[dataset_name][type_name][i + 0]]
            polylines1 = [wood_rui_globals[dataset_name][type_name][i + 1]]
            brep = loft_polylines_with_holes(polylines0, polylines1)
            if brep:
                lists_breps.append([brep])
                element_ids.append(i)

    add_loft_brep(lists_breps, dataset_name, element_ids)
