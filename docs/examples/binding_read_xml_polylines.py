from compas_wood.binding import read_xml_polylines



polylines = read_xml_polylines()

import sys

if sys.version_info >= (3, 9):
    try:
        from compas_viewer import Viewer
    except ImportError:
        print("compas_viewer is not installed.")

    viewer = Viewer()

    for polyline in polylines:
        viewer.scene.add(polyline, show_points=False)

    viewer.show()
