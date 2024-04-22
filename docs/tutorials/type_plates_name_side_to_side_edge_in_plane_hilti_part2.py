from compas.geometry import Polyline
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import read_xml_polylines
from compas_wood.binding import wood_globals

# Read polylines from an exrernal file, in this case compas_wood xml dataset.
xml_polylines = read_xml_polylines(
    foldername="/home/petras/brg/2_code/wood_nano/src/wood/cmake/src/wood/dataset/",
    filename='type_plates_name_side_to_side_edge_inplane_hilti',
    scale=1e0
)

# Create joints.
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[5000, 1.0, 55],
    input_output_type=5)

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

    viewer = Viewer(show_grid=False, rendermode='ghosted')

    scale = 1e-2

    # for polylines in polylines_lists:
    #     for polyline in polylines:
    #         polyline.transform(Scale.from_factors([scale, scale, scale]))
    #         viewer.scene.add(polyline, show_points=False, line_width=2, linecolor=(255, 0, 100))

    for mesh in meshes:
        mesh.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(mesh, show_points=False, hide_coplanaredges=True, lineswidth=2, linecolor=(0, 0, 0))

    viewer.show()
