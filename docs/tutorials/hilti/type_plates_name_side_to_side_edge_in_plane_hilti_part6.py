from compas.geometry import Polyline
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import read_xml_polylines
from compas_wood.binding import wood_globals
from pathlib import Path
from compas import json_dump

# Read polylines from an exrernal file, in this case compas_wood xml dataset.
xml_polylines = read_xml_polylines(
    foldername= str(Path.cwd() / "src/compas_wood/datasets/") + '/',
    filename='type_plates_name_side_to_side_edge_inplane_hilti_square_shell',
    # scale=0.25e1
)


xml_polylines.reverse()

# Create joints.
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[1500, 1.5, 55],
    input_output_type=5)


# Remove duplicate polylines - bug in the code.
new_polyline_lists = []
for polylines in polylines_lists:
    new_polylines = []
    for polyline in polylines:
        if polyline not in new_polylines:
            new_polylines.append(polyline)
    new_polyline_lists.append(new_polylines)
polylines_lists = new_polyline_lists

# Create meshes.
meshes = mesh_boolean_difference_from_polylines(polylines_lists)

# Serialize meshes.
json_dump(meshes, 'data/json_dump/type_plates_name_side_to_side_edge_inplane_hilti_part6.json')

# Vizualize.

try:

    from compas_viewer import Viewer
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False, rendermode='ghosted')
    scale = 1e-2

    # for polylines in polylines_lists:
    #     for polyline in polylines:
    #         polyline.transform(Scale.from_factors([scale, scale, scale]))
    #         viewer.scene.add(polyline, show_points=False, line_width=2, linecolor=(255, 0, 100))

    # print(meshes[0])
    for i, mesh in enumerate(meshes):
        mesh.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(mesh, show_points=False, hide_coplanaredges=True, lineswidth=2, linecolor=(0, 0, 0))
    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")