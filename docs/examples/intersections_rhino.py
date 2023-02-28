from compas.geometry import Point
from compas.geometry import Box
from compas.geometry import Sphere
from compas.geometry import Polyline
from compas.datastructures import Mesh
from compas.utilities import hex_to_rgb

from compas_rhino.artists import PointArtist, PolylineArtist, MeshArtist

# @me add restart option to init
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

B = proxy.remesh(B, 0.3, 10)

# ==============================================================================
# Compute the intersections
# ==============================================================================

proxy.package = 'compas_cgal.intersections'

pointsets = proxy.intersection_mesh_mesh(A, B)

# ==============================================================================
# Process output
# ==============================================================================

polylines = []
for points in pointsets:
    points = [Point(*point) for point in points]
    polyline = Polyline(points)
    polylines.append(polyline)

# ==============================================================================
# Visualize
# ==============================================================================

meshartist = MeshArtist(None)

meshartist.mesh = Mesh.from_vertices_and_faces(*A)
meshartist.layer = "CGAL::Intersections::A"
meshartist.clear_layer()
meshartist.draw_faces(join_faces=True, color=hex_to_rgb('#222222'))

meshartist.mesh = Mesh.from_vertices_and_faces(*B)
meshartist.layer = "CGAL::Intersections::B"
meshartist.clear_layer()
meshartist.draw_faces(join_faces=True, color=hex_to_rgb('#888888'))

polylineartist = PolylineArtist(None, layer="CGAL::Intersections::Polylines")
polylineartist.clear_layer()
pointartist = PointArtist(None, layer='CGAL::Intersections::Points')
pointartist.clear_layer()

for polyline in polylines:
    polylineartist.primitive = polyline
    polylineartist.color = hex_to_rgb('#ffffff')
    polylineartist.draw()
    PointArtist.draw_collection(
        polyline.points, color=(255, 0, 0), layer='CGAL::Intersections::Points')

polylineartist.redraw()
