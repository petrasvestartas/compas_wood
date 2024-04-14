from compas_wood.binding import joints
from compas.geometry import Polyline


polyline0 = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [0, 0, 0]])
polyline1 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
polyline2 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
polyline3 = Polyline([[0, 0, 2], [1, 0, 2], [1, 1, 2], [0, 1, 2], [0, 0, 2]])

from compas.geometry import Rotation

xform = Rotation.from_axis_and_angle([0, 0, 1], 0.2)
polyline2.transform(xform)
polyline3.transform(xform)

polylines = [polyline0, polyline1, polyline2, polyline3]

element_pairs, joint_areas, joint_types = joints(polylines)
print(element_pairs)
print(joint_types)

import sys

if sys.version_info >= (3, 9):
    from compas_viewer import Viewer

    viewer = Viewer(show_grid=False)

    for polyline in polylines:
        viewer.scene.add(polyline, show_points=False)

    for polyline in joint_areas:
        viewer.scene.add(polyline, show_points=True, lineswidth=3)

    viewer.show()
