from compas_wood.binding import read_xml_polylines



polylines = read_xml_polylines()


try:
    
    from compas_viewer import Viewer

    viewer = Viewer()

    for polyline in polylines:
        viewer.scene.add(polyline, show_points=False)

    viewer.show()


except ImportError:
    print("compas_viewer is not installed.")
