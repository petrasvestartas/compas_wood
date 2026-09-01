"""Joinery generation on plate-like Brep solids.

PlateModel.from_breps converts each solid's top/bottom plate faces to outline
polylines (compas_occt backend), so detection and joinery run on Brep geometry.
The displayed meshes are the solver's carved lofts.

Swap `boxes` and `search_type` for the other configurations:
  stack:  two overlapping slabs, SEARCH_FACE_TO_FACE (top-to-top, family 40-49)
  corner: an L of two plates,    SEARCH_FACE_TO_FACE (top-to-side, family 20-29)
"""

from compas.geometry import Box
from compas.geometry import Brep
from compas.geometry import Frame
from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import PlateModel
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

# Two vertical plates crossing in an X.
boxes = [
    Box(2000, 40, 400, frame=Frame([0, 0, 0])),
    Box(40, 2000, 400, frame=Frame([0, 0, 0])),
]
search_type = SEARCH_CROSS_JOINT

breps = [Brep.from_box(box) for box in boxes]
model = PlateModel.from_breps(breps)
elements, joints = model.solve(search_type=search_type)
print(f"{len(breps)} breps -> {len(model.plates)} plates, {len(elements)} elements, {len(joints)} joints")

scene = SessionScene("joinery_solver_from_breps")
add_joinery(scene, elements, joints, draw_meshes=True, show_volumes=True, name="JoinerySolver")
publish(scene, "joinery_solver_from_breps")
