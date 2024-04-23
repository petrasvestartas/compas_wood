from compas.geometry import Polyline
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import wood_globals

# Create two polylines.
polyline00 = Polyline([[0, 150, 0], [-300, 150, 0], [-300, -150, 0], [0, -150, 0], [0, 150, 0]])
polyline01 = Polyline([[0, 150, 120], [-300, 150, 120], [-300, -150, 120], [0, -150, 120], [0, 150, 120]])
polyline10 = Polyline([[0,150,0], [0, -150, 0], [300, -150, 0], [300, 150, 0], [0, 150, 0]])
polyline11 = Polyline([[0,150,120], [0, -150, 120], [300, -150, 120], [300, 150, 120], [0, 150, 120]])
polylines = [polyline00, polyline01, polyline10, polyline11]

# Create joints.
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=polylines,
    input_joint_parameters_and_types=[500, 1.0, 55],
    input_output_type=5)

# Create meshes.
meshes = mesh_boolean_difference_from_polylines(polylines_lists)


# Vizualize.

try:
    from compas_viewer import Viewer
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False, rendermode='ghosted')
    scale = 1e-2

    for polylines in polylines_lists:
        for polyline in polylines:
            polyline.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(polyline, show_points=False)
            
    for mesh in meshes:
        mesh.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(mesh, show_points=False, hide_coplanaredges=True, lineswidth=2, linecolor=(0, 0, 0))

    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
