import numpy as np

from compas_plotters import Plotter
from compas.geometry import Point, Polygon, Translation
from compas.datastructures import Mesh
from compas_cgal.triangulation import conforming_delaunay_triangulation as cdt
from compas_cgal.triangulation import refined_delaunay_mesh as rdm

# ==============================================================================
# Constraints
# ==============================================================================

boundary = Polygon.from_sides_and_radius_xy(12, 10)
boundary = np.array(boundary, dtype=np.float64)

hole = Polygon.from_sides_and_radius_xy(12, 2)

hole1 = hole.transformed(Translation.from_vector([4, 0, 0]))
hole2 = hole.transformed(Translation.from_vector([-4, 0, 0]))
hole3 = hole.transformed(Translation.from_vector([0, 4, 0]))
hole4 = hole.transformed(Translation.from_vector([0, -4, 0]))

holes = [hole1, hole2, hole3, hole4]

points = [Point(4, 0, 0), Point(-4, 0, 0), Point(0, 4, 0), Point(0, -4, 0)]

# ==============================================================================
# Triangulation
# ==============================================================================

V, F = cdt(boundary, points=points, holes=holes)

cdt_mesh = Mesh.from_vertices_and_faces(V, F)

V, F = rdm(boundary, points=points, holes=holes, maxlength=1.0, is_optimized=True)

rdm_mesh = Mesh.from_vertices_and_faces(V, F)

# ==============================================================================
# Viz
# ==============================================================================

plotter = Plotter(figsize=(16, 9))

plotter.add(cdt_mesh, sizepolicy='absolute', vertexsize=5)
plotter.add(rdm_mesh.transformed(Translation.from_vector([23, 0, 0])), sizepolicy='absolute', vertexsize=5)

plotter.zoom_extents()
# plotter.save('docs/_images/cgal_triangulation.png', dpi=150)
plotter.show()
