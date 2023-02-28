from compas.geometry import Point
from compas.geometry import Box
from compas.geometry import Sphere
from compas.datastructures import Mesh

from compas_cgal.booleans import boolean_union
from compas_cgal.meshing import remesh


def test_booleans():

    # ==============================================================================
    # Make a box and a sphere
    # ==============================================================================

    box = Box.from_width_height_depth(2, 2, 2)
    box = Mesh.from_shape(box)
    box.quads_to_triangles()

    A = box.to_vertices_and_faces()

    sphere = Sphere(Point(1, 1, 1), 1)
    sphere = Mesh.from_shape(sphere, u=30, v=30)
    sphere.quads_to_triangles()

    B = sphere.to_vertices_and_faces()

    # ==============================================================================
    # Remesh the sphere
    # ==============================================================================

    B = remesh(B, 0.3, 10)

    # ==============================================================================
    # Compute the boolean mesh
    # ==============================================================================

    V, F = boolean_union(A, B)

    Mesh.from_vertices_and_faces(V, F)
