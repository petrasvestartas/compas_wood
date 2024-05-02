from compas_wood.binding import get_connection_zones
from compas.geometry import Polyline
from compas_wood import data_sets_plates


# joinery parameters
division_length = 300
joint_parameters = [
    100,
    0.5,
    5,
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

polylines =[
    Polyline([
        [-12.0, 84, -5],
        [-12.0, 84, 5],
        [37, 84, 5],
        [37, 84, -5.0],
        [-12.0, 84, -5],
    ]),
    Polyline([
        [-12.0, 74, -5],
        [-12.0, 74, 5],
        [37, 74, 5],
        [37, 74, -5.0],
        [-12.0, 74, -5],
    ]),
    Polyline([
        [37, 84, -5.0],
        [37, 84, 5],
        [86, 84, 5],
        [86, 84, -5.0],
        [37, 84, -5.0],
    ]),
    Polyline([
        [37, 74, -5.0],
        [37, 74, 5],
        [86, 74, 5],
        [86, 74, -5.0],
        [37, 74, -5.0],
    ])
]

# generate joints
output_polylines, output_types = get_connection_zones(
    polylines,
    [],
    [],
    [],
    [],
    joint_parameters,
    0,
    [1, 1, 1],
    3,
)


try:

    from compas_viewer import Viewer
    from compas.geometry import Scale

    scale = 1e-1
    scale_transform = Scale.from_factors([scale, scale, scale])
    viewer = Viewer(show_grid=False)
    for polylines in output_polylines:
        for polyline in polylines:
            polyline.transform(scale_transform)
            viewer.scene.add(Polyline(polyline), show_points=False)

    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
