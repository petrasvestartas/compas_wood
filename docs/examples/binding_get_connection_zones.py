from compas_wood.binding import get_connection_zones
from compas.geometry import Polyline
from compas_wood import data_sets_plates


# joinery parameters
division_length = 300
joint_parameters = [
    division_length,
    0.5,
    9,
    division_length * 1.5,
    0.65,
    10,
    division_length * 1.5,
    0.5,
    21,
    division_length,
    0.95,
    30,
    division_length,
    0.95,
    40,
    division_length,
    0.95,
    50,
]

# generate joints
output_polylines, output_types = get_connection_zones(
    data_sets_plates.annen_small_polylines(),
    data_sets_plates.annen_small_edge_directions(),
    data_sets_plates.annen_small_edge_joints(),
    data_sets_plates.annen_small_three_valance_element_indices_and_instruction(),
    [],
    joint_parameters,
    0,
    [1, 1, 1],
    4,
)

import sys

if sys.version_info >= (3, 9):

    from compas_viewer import Viewer
    from compas.geometry import Scale

    scale = 1e-3
    scale_transform = Scale.from_factors([scale, scale, scale])
    viewer = Viewer(show_grid=False)
    for polylines in output_polylines:
        for polyline in polylines:
            polyline.transform(scale_transform)
            viewer.scene.add(Polyline(polyline), show_points=False)

    viewer.show()
