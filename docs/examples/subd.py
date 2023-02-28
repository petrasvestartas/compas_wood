from compas.geometry import Box, Polyhedron
from compas.geometry import Translation
from compas_cgal.subdivision import catmull_clark
from compas_view2.app import App

# ==============================================================================
# Input
# ==============================================================================

box = Box.from_diagonal(([0, 0, 0], [1, 1, 1]))

V, F = box.to_vertices_and_faces()

VF2 = catmull_clark((V, F), 2)
VF4 = catmull_clark((V, F), 4)
VF6 = catmull_clark((V, F), 6)
VF8 = catmull_clark((V, F), 8)

S2 = Polyhedron(*VF2)
S4 = Polyhedron(*VF4)
S6 = Polyhedron(*VF6)
S8 = Polyhedron(*VF8)

S2.transform(Translation.from_vector([1, 0, 0]))
S4.transform(Translation.from_vector([2, 0, 0]))
S6.transform(Translation.from_vector([3, 0, 0]))
S8.transform(Translation.from_vector([4, 0, 0]))

# ==============================================================================
# Viz
# ==============================================================================

viewer = App(width=1200, height=750)
viewer.view.camera.tx = -2.5
viewer.view.camera.ty = -1
viewer.view.camera.rz = 0
viewer.view.camera.distance = 5

viewer.add(box)
viewer.add(S2)
viewer.add(S4)
viewer.add(S6)
viewer.add(S8)
viewer.show()
