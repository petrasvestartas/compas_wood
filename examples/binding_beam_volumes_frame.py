from compas.geometry import Polyline
from compas_wood.binding import beam_volumes


input_polylines = [
    Polyline([
        [-5, -5, 0],
        [-5, 5, 0]
    ]),
    Polyline([
        [-5, 5, 0],
        [5, 5, 0]
    ]),
    Polyline([
        [5, 5, 0],
        [5, -5, 0]
    ]),
    Polyline([
        [5, -5, 0],
        [-5, -5, 0]
    ]),
]


input_polylines_segment_radii = []

for i in range(len(input_polylines)):
    input_polyline_segment_radii = []
    for j in range(len(input_polylines[i].points)):
        input_polyline_segment_radii.append(0.2)
    input_polylines_segment_radii.append(input_polyline_segment_radii)

input_polylines_segment_direction = []
for i in range(len(input_polylines)):
    input_polyline_segment_direction = []
    for j in range(len(input_polylines[i].points)-1):
        input_polyline_segment_direction.append([0, 0, 1])
    input_polylines_segment_direction.append(input_polyline_segment_direction)


index_polylines, index_polylines_segment, distance, point_pairs, volume_pairs, joints_areas, joints_types = beam_volumes(
        input_polylines,
        input_polylines_segment_radii,
        input_polylines_segment_direction,
        input_allowed_types_per_polyline=[0],
        input_min_distance=1,
        input_volume_length=5,
        input_cross_or_side_to_end=2.01,
)



try:
    from compas_viewer import Viewer
    from compas.geometry import Scale
    from compas.geometry import Polygon

    colors = {
        10: (255, 0, 0),  # Side-to-side rotated
        11: (0, 255, 0),  # Side-to-side out-of-plane
        12: (0, 0, 255),  # Side-to-side in-plane
        13: (100, 100, 100),  # Side-to-side in-plane
        20: (255, 0, 150),  # Top-to-side
        30: (0, 200, 255)  # Cross
    }

    viewer = Viewer(show_grid=False, rendermode='lighted')
    scale = 1e-0

    for polyline in input_polylines:
        polyline.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(polyline, show_points=False, lineswidth=1, linecolor=(150, 150, 150))

    for id, p in enumerate(point_pairs):
        for point in p:
            point.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(point, size=10, pointcolor=colors[joints_types[id]])

    for volume in volume_pairs:
        for polyline in volume:
            polyline.transform(Scale.from_factors([scale, scale, scale]))

            viewer.scene.add(polyline, show_points=False, lineswidth=2, linecolor=(150, 150, 150))

    for id, joint_area in enumerate(joints_areas):
        joint_area.transform(Scale.from_factors([scale, scale, scale]))

        viewer.scene.add(Polygon(joint_area.points[:-1]).to_mesh(),
                         show_points=False,
                         show_lines=False,
                         facecolor=colors[joints_types[id]])

    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
