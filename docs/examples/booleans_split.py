from compas.geometry import Point
from compas.geometry import Box
from compas.geometry import Sphere
from compas.datastructures import Mesh
from compas.colors import Color
from random import random

from compas_view2.app import App

from compas_cgal.booleans import split
from compas.topology import connected_components

# ==============================================================================
# Make a box and a sphere
# ==============================================================================

box = Box.from_width_height_depth(2, 2, 2)
box = Mesh.from_shape(box)
box.quads_to_triangles()

A = box.to_vertices_and_faces()

sphere = Sphere(Point(1, 1, 1), 1)
sphere = Mesh.from_shape(sphere, u=20, v=20)
sphere.quads_to_triangles()

B = sphere.to_vertices_and_faces()

# ==============================================================================
# Compute the mesh split
# ==============================================================================

V, F = split(A, B)

mesh = Mesh.from_vertices_and_faces(V, F)

# ==============================================================================
# Seperate disjoint faces and visualize
# ==============================================================================

viewer = App()

fa = mesh.face_adjacency()
parts = connected_components(fa)
for face_indexes in connected_components(fa):
    mesh = Mesh.from_vertices_and_faces(V, F[face_indexes])
    mesh.remove_unused_vertices()
    viewer.add(mesh, facecolor=Color.from_i(random()))

viewer.run()
