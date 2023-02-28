from compas.geometry import Point
from compas.geometry import Box
from compas.geometry import Sphere
from compas.datastructures import Mesh

from compas_rhino.artists import MeshArtist

from compas.rpc import Proxy

proxy = Proxy()

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

proxy.package = 'compas_cgal.meshing'

B = proxy.remesh(B, 0.3, 100)

# ==============================================================================
# Compute the boolean mesh
# ==============================================================================

proxy.package = 'compas_cgal.booleans'

V, F = proxy.boolean_union(A, B)

mesh = Mesh.from_vertices_and_faces(V, F)

# ==============================================================================
# Visualize
# ==============================================================================

artist = MeshArtist(mesh, layer="CGAL::Booleans::Union")
artist.clear_layer()

artist.draw_faces(join_faces=True)

artist.redraw()
