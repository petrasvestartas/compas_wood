from compas.geometry import Polyline
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import read_xml_polylines
from compas.datastructures import Mesh
from compas_wood.binding import wood_globals

# Hard code mesh.
vertices = [
[1978.5396270848985, 112.18997185154055, 577.3224456715213], 
[0.0, 0.0, 0.0], 
[-112.18997185154105, -1978.5396270848892, 577.3224456715292], 
[1920.162503885306, -1920.1625038852967, 1187.7604905134772], 
[3999.999999999999, 146.9968976249949, 756.473945159439], 
[3999.999999999999, -1902.0308010453325, 1377.3914296049913], 
[6021.4603729151, 112.18997185154055, 577.3224456715213], 
[6079.837496114693, -1920.1625038852965, 1187.7604905134772], 
[7999.999999999998, 0.0, 0.0], 
[8112.18997185154, -1978.5396270848892, 577.3224456715253], 
[-146.9968976249954, -4000.000000000001, 756.473945159439], 
[1902.0308010453416, -4000.000000000001, 1377.3914296049952], 
[3999.9999999999986, -4000.000000000001, 1570.3422579488933], 
[6097.9691989546545, -4000.000000000001, 1377.3914296049952], 
[8146.996897624993, -4000.000000000001, 756.473945159439], 
[-112.18997185154203, -6021.460372915113, 577.3224456715213], 
[1920.1625038853044, -6079.8374961147065, 1187.7604905134772], 
[3999.999999999998, -6097.96919895467, 1377.3914296049913], 
[6079.837496114693, -6079.8374961147065, 1187.7604905134772], 
[8112.18997185154, -6021.460372915113, 577.3224456715253], 
[-1.9675624286558107e-12, -8000.000000000001, -3.935124857311621e-12], 
[1978.539627084896, -8112.1899718515415, 577.3224456715253], 
[3999.999999999998, -8146.9968976249975, 756.473945159439], 
[6021.4603729151, -8112.1899718515415, 577.3224456715213], 
[7999.999999999998, -8000.000000000002, 0.0]]

faces = [[0, 1, 2, 3], 
         [4, 0, 3, 5], 
         [6, 4, 5, 7], 
         [8, 6, 7, 9], 
         [3, 2, 10, 11], 
         [5, 3, 11, 12], 
         [7, 5, 12, 13], 
         [9, 7, 13, 14], 
         [11, 10, 15, 16], 
         [12, 11, 16, 17], 
         [13, 12, 17, 18],
         [14, 13, 18, 19], 
         [16, 15, 20, 21], 
         [17, 16, 21, 22], 
         [18, 17, 22, 23], 
         [19, 18, 23, 24]]

mesh1 = Mesh.from_vertices_and_faces(vertices, faces)
scale = 1
mesh1.scale(scale)
mesh2 = mesh1.copy().offset(120*scale)
polygons0 = mesh1.to_polygons()
polygons1 = mesh2.to_polygons()

# conert polygons to polylines
input_polylines = []
for polygon0, polygon1 in zip(polygons0, polygons1):
    input_polylines.append(Polyline(polygon0))
    input_polylines[-1].points.append(input_polylines[-1].points[0])
    input_polylines.append(Polyline(polygon1))
    input_polylines[-1].points.append(input_polylines[-1].points[0])


# Create joints.
wood_globals.distance_squared = 10
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True


polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=input_polylines,
    input_joint_parameters_and_types=[1200, 1.0, 55],
    input_output_type=5,
    input_scale=[1, 1.0, 1])


# Create meshes.
meshes = mesh_boolean_difference_from_polylines(polylines_lists)

# Vizualize.
import sys
if sys.version_info >= (3, 9):
    try:
        from compas_viewer import Viewer
    except ImportError:
        print("compas_viewer is not installed.")
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False, rendermode='lighted')

    scale = 1e-2


    for polylines in polylines_lists:
        for polyline in polylines:
            polyline.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(polyline, show_points=False, line_width=2, linecolor=(0, 0, 0))



    for mesh in meshes:
        mesh.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(mesh, show_points=False, show_lines=False, hide_coplanaredges=False)

    viewer.show()
