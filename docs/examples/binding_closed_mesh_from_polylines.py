from compas.geometry import Polyline
from compas_wood.binding import closed_mesh_from_polylines


polyline0 = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [0, 0, 0]])
polyline1 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
polyline2 = Polyline([[0.25, 0.25, 0], [0.75, 0.25, 0], [0.75, 0.75, 0], [0.25, 0.75, 0], [0.25, 0.25, 0]])
polyline3 = Polyline([[0.25, 0.25, 1], [0.75, 0.25, 1], [0.75, 0.75, 1], [0.25, 0.75, 1], [0.25, 0.25, 1]])
polylines = [polyline0, polyline1, polyline2, polyline3]

mesh = closed_mesh_from_polylines(polylines)


try:

    from compas_viewer import Viewer

    viewer = Viewer()
    viewer.add(mesh, show_faces=True, show_edges=True, show_points=False)
    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")