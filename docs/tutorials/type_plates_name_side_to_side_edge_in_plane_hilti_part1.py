from compas.geometry import Polyline
from compas_wood.binding import get_connection_zones

# create two polylines

polyline00 = Polyline([[0, 150, 0], [-300, 150, 0], [-300, -150, 0], [0, -150, 0], [0, 150, 0]])
polyline01 = Polyline([[0, 150, 120], [-300, 150, 120], [-300, -150, 120], [0, -150, 120], [0, 150, 120]])

polyline10 = Polyline([[0,150,0], [0, -150, 0], [300, -150, 0], [300, 150, 0], [0, 150, 0]])
polyline11 = Polyline([[0,150,120], [0, -150, 120], [300, -150, 120], [300, 150, 120], [0, 150, 120]])

polylines = [polyline00, polyline01, polyline10, polyline11]

# createa a joint

output_polylines, output_types = get_connection_zones(
    input_polyline_pairs=polylines,
    input_joint_parameters_and_types=[

    ]
    input_output_type=4)





# vizualize in the viewer

import sys
if sys.version_info >= (3, 9):
    from compas_viewer import Viewer
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False)

    scale = 1e-2
    for polyline in polylines:
        polyline.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(polyline, show_points=False)

    viewer.show()
