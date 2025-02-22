# flags: python.reloadEngine
#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, add_loft_mesh
from System.Drawing import Color
import System


def to_polyline_from_cp(curve):

    polyline = Rhino.Geometry.Polyline()
    result, polyline1 = curve.TryGetPolyline()

    if result is False:

        nurbsCurve = curve.ToNurbsCurve()
        point3dArray = []

        for i in range(len(nurbsCurve.Points)):
            result, pt = nurbsCurve.Points.GetPoint(i)
            point3dArray.append(pt)

        polyline1 = Rhino.Geometry.Polyline(point3dArray)
        nurbsCurve = polyline1.ToNurbsCurve()
        result, polyline1 = nurbsCurve.TryGetPolyline()

        polyline1.CollapseShortSegments(0.01)

        polyline1 = Rhino.Geometry.Polyline(polyline1)
        polyline = polyline1

    else:
        polyline1.CollapseShortSegments(0.01)
        polyline = polyline1

    return polyline


def sort_set_of_points(points_to_sort, guide_points):
    """Sort one array of point by an another point array

    use it like this:
    #points_to_sort_new = []
    #for i in points_to_sort_ids:
    #    points_to_sort_new.append(points_to_sort[i])

    Parameters
    ----------
    points_to_sort : list[rg.Point3d]
    guide_points : list[rg.Point3d]

    Returns
    -------
    list[int]
        the list of indices of the sorted array
    """

    # create a copy
    points_to_sort_copy = []
    for i in points_to_sort:
        points_to_sort_copy.append(Rhino.Geometry.Point3d(i))

    guide_points_copy = []
    for i in guide_points:
        guide_points_copy.append(Rhino.Geometry.Point3d(i))

    # make indices of points
    points_to_sort_ids = list(range(0, len(points_to_sort_copy)))
    guide_points_ids = list(range(0, len(guide_points_copy)))

    # sort both lists by xyz coordinates together with indices
    points_to_sort_copy, points_to_sort_ids = zip(*sorted(zip(points_to_sort_copy, points_to_sort_ids)))
    guide_points_copy, guide_points_ids = zip(*sorted(zip(guide_points_copy, guide_points_ids)))

    # sort guide points indices and order the points to sort indec
    guide_points_ids, points_to_sort_ids = zip(*sorted(zip(guide_points_ids, points_to_sort_ids)))

    # can be used like this:
    # points_to_sort_new = []
    # for i in points_to_sort_ids:
    #    points_to_sort_new.append(points_to_sort[i])

    return points_to_sort_ids


def loft_polylines_with_holes(curves0, curves1):
    """Loft polylines with holes

    Parameters
    ----------
    curves0 : list[rg.Polyline]
    curves1 : list[rg.Polyline]

    Returns
    -------
    rg.Mesh
        mesh as lofted polylines and cap them
    """

    ###############################################################################
    # user input
    ###############################################################################

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
                    curves2.append(curves0[i])
                else:
                    curves.append(curves0[i])
        elif flag1:
            for i in range(0, len(curves1), 2):
                curves.append(curves1[i])
                curves2.append(curves1[i + 1])
        elif flag2:
            curves = curves0
            curves2 = curves1

        curves0 = curves
        curves1 = curves2

    ###############################################################################
    # sort curves which one is border and which ones are holes
    ###############################################################################
    length = -1.0
    border_id = -1
    count = 0

    for curve in curves0:
        diagonal = curve.GetBoundingBox(True).Diagonal
        temp_length = diagonal.Length
        if temp_length > length:
            length = temp_length
            border_id = count
        count = count + 1

    border0 = to_polyline_from_cp(curves0[border_id])
    border1 = to_polyline_from_cp(curves1[border_id])
    holes0 = []
    holes1 = []
    for i in range(len(curves0)):
        if i != border_id:
            holes0.append(curves0[i])
            holes1.append(curves1[i])

    ###############################################################################
    # Create mesh of the bottom face
    ###############################################################################
    mesh_bottom = Rhino.Geometry.Mesh.CreatePatch(
        border0, Rhino.RhinoDoc.ActiveDoc.ModelAngleToleranceRadians, None, holes0, None, None, True, 1
    )

    ###############################################################################
    # Convert closed polylines to open ones
    ###############################################################################

    point_3d_array = []
    for i in range(len(mesh_bottom.Vertices)):
        point_3d_array.append(mesh_bottom.Vertices.Point3dAt(i))

    open_curve_points_0 = []
    open_curve_points_1 = []

    for i in range(len(border0) - 1):
        open_curve_points_0.append(border0[i])
        open_curve_points_1.append(border1[i])

    for i in range(len(holes0)):
        holes0_polyline = to_polyline_from_cp(holes0[i])
        holes1_polyline = to_polyline_from_cp(holes1[i])
        for j in range(len(holes0_polyline) - 1):
            open_curve_points_0.append(holes0_polyline[j])
            open_curve_points_1.append(holes1_polyline[j])

    ###############################################################################
    # Using the bottom mesh vertices create the top mesh f
    ###############################################################################
    points_to_sort_new = sort_set_of_points(open_curve_points_0, point_3d_array)
    mesh_top = mesh_bottom.DuplicateMesh()

    for i in range(len(points_to_sort_new)):
        mesh_top.Vertices.SetVertex(i, open_curve_points_1[points_to_sort_new[i]])

    ###############################################################################
    # Merge the top and bottom mesh into one and append sides
    ###############################################################################
    mesh = Rhino.Geometry.Mesh()
    mesh.Append(mesh_bottom)
    mesh.Append(mesh_top)

    count = len(mesh_bottom.Vertices)
    for p in range((mesh_bottom.TopologyEdges).Count):

        if mesh_bottom.TopologyEdges.GetConnectedFaces(p).Length == 1:
            topologyVertices = mesh_bottom.TopologyEdges.GetTopologyVertices(p)
            mesh.Faces.AddFace(
                topologyVertices.I, topologyVertices.J, topologyVertices.J + count, topologyVertices.I + count
            )

    mesh.UnifyNormals()
    mesh.Unweld(0.01, True)
    return mesh

    mesh.Ngons.AddPlanarNgons(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 1, 3, 1, True)
    # return mesh
    ###############################################################################
    # Unweld ngons meshes
    ###############################################################################
    return mesh
    unwelded_ngon_mesh = Rhino.Geometry.Mesh()

    count = 0
    for ngonAndFacesEnumerable in mesh.GetNgonAndFacesEnumerable():

        faces = []
        for j in ngonAndFacesEnumerable.FaceIndexList():
            faces.append(System.Int32(j))

        temp_ngon_mesh = mesh.DuplicateMesh().Faces.ExtractFaces(faces)
        temp_ngon_mesh.Ngons.Clear()

        # append colors
        if count < 2:
            temp_ngon_mesh.VertexColors.CreateMonotoneMesh(Color.LightGray)
        else:
            temp_ngon_mesh.VertexColors.CreateMonotoneMesh(Color.DeepPink)

        unwelded_ngon_mesh.Append(temp_ngon_mesh)
        count = count + 1

    ###############################################################################
    # Output
    ###############################################################################

    unwelded_ngon_mesh.RebuildNormals()
    # unwelded_ngon_mesh.Ngons.Clear()
    return unwelded_ngon_mesh


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

    meshes = []

    if type_name == "joinery":
        for i in range(len(wood_rui_globals[dataset_name][type_name])):
            polylines0 = []
            polylines1 = []
            for j in range(0, len(wood_rui_globals[dataset_name][type_name][i]), 2):
                pline0 = wood_rui_globals[dataset_name][type_name][i][j + 0]
                pline1 = wood_rui_globals[dataset_name][type_name][i][j + 1]
                polylines0.append(pline0.ToNurbsCurve())
                polylines1.append(pline1.ToNurbsCurve())
            mesh = loft_polylines_with_holes(polylines0, polylines1)
            meshes.append(mesh)

    elif type_name == "polylines":
        for i in range(0, len(wood_rui_globals[dataset_name][type_name]), 2):
            polylines0 = [wood_rui_globals[dataset_name][type_name][i + 0].ToNurbsCurve()]
            polylines1 = [wood_rui_globals[dataset_name][type_name][i + 1].ToNurbsCurve()]
            mesh = loft_polylines_with_holes(polylines0, polylines1)
            meshes.append(mesh)

    add_loft_mesh(meshes, dataset_name)
