import math
import compas

from compas.geometry import scale_vector
from compas.geometry import Point
from compas.geometry import Vector
from compas.geometry import Plane
from compas.geometry import Polyline
from compas.geometry import Rotation
from compas.geometry import Translation
from compas.geometry import Scale
from compas.datastructures import Mesh

from compas_rhino.artists import MeshArtist
from compas_rhino.artists import PolylineArtist

from compas.rpc import Proxy

slicer = Proxy('compas_cgal.slicer')

# ==============================================================================
# Get the bunny and construct a mesh
# ==============================================================================

bunny = Mesh.from_ply(compas.get('bunny.ply'))

# ==============================================================================
# Move the bunny to the origin and rotate it upright.
# ==============================================================================

vector = Vector(* bunny.centroid())
T = Translation.from_vector(vector.scaled(-1))
S = Scale.from_factors([100, 100, 100])
R = Rotation.from_axis_and_angle(Vector(1, 0, 0), math.radians(90))

bunny.transform(R * S * T)

# ==============================================================================
# Create planes
# ==============================================================================

# replace by planes along a curve

bbox = bunny.bounding_box()

x, y, z = zip(*bbox)
xmin, xmax = min(x), max(x)
N = 50
dx = (xmax - xmin) / N

normal = Vector(1, 0, 0)
planes = []
for i in range(N):
    x = xmin + i * dx
    plane = Plane(Point(x, 0, 0), normal)
    planes.append(plane)

# ==============================================================================
# Slice
# ==============================================================================

pointsets = slicer.slice_mesh(
    bunny.to_vertices_and_faces(),
    planes)

# ==============================================================================
# Process output
# ==============================================================================

polylines = []
for points in pointsets:
    points = [Point(*point) for point in points]  # otherwise Polygon throws an error
    polyline = Polyline(points)
    polylines.append(polyline)

# ==============================================================================
# Visualize
# ==============================================================================

meshartist = MeshArtist(bunny, layer="CGAL::Slicer::Bunny")
meshartist.clear_layer()
meshartist.draw_faces(join_faces=True, color=(255, 200, 200))
meshartist.redraw()

# this is very slow

polylineartist = PolylineArtist(None, layer="CGAL::Slicer::Slices")
polylineartist.clear_layer()
for polyline in polylines:
    polylineartist.primitive = polyline
    polylineartist.color = (255, 0, 0)
    polylineartist.draw()
polylineartist.redraw()
