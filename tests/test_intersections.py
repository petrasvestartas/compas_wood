import numpy as np

from compas.geometry import Point
from compas.geometry import Sphere
from compas.geometry import Polyline
from compas.datastructures import Mesh


from compas_cgal._cgal import intersections


def test_intersections():

    A = Sphere(Point(0, 0, 0), 1.0)
    A = Mesh.from_shape(A, u=50, v=50)
    A.quads_to_triangles()

    B = Sphere(Point(1.0, 0, 0), 1.0)
    B = Mesh.from_shape(B, u=50, v=50)
    B.quads_to_triangles()

    va, fa = A.to_vertices_and_faces()
    VA = np.asarray(va, dtype=np.float64)
    FA = np.asarray(fa, dtype=np.int32)

    vb, fb = B.to_vertices_and_faces()
    VB = np.asarray(vb, dtype=np.float64)
    FB = np.asarray(fb, dtype=np.int32)

    polylines = []
    pointsets = intersections.intersection_mesh_mesh(VA, FA, VB, FB)
    for points in pointsets:
        points = [Point(*point) for point in points]  # otherwise Polygon throws an error
        polyline = Polyline(points)
        polylines.append(polyline)

    print(pointsets)


if __name__ == "__main__":
    test_intersections()
