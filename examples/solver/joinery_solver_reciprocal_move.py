"""Reciprocal frame (translation) joinery on a hex grid.

The displayed meshes are the solver's carved lofts. The uncut stock beams are
added hidden under a "Stock" group for comparison.

This kernel rests translation-based frames beam-on-beam (top-to-side, type 20),
so a cross-only search finds nothing here - hence SEARCH_BOTH.
"""

from compas_wood import SEARCH_BOTH
from compas_wood import PlateModel
from compas_wood import reciprocal_move_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery
from compas_wood.viewer import add_solid

nx = 6
ny = 6
mesh_type = "hex"
angle = 200.0
beam_w = 200.0
beam_h = 400.0

_, beams, side0, side1 = reciprocal_move_elements(
    nx=nx,
    ny=ny,
    mesh_type=mesh_type,
    angle=angle,
    beam_w=beam_w,
    beam_h=beam_h,
)
model = PlateModel.from_beams(beams, side0, side1)
elements, joints = model.solve(search_type=SEARCH_BOTH)

scene = SessionScene("joinery_solver_reciprocal_move")
root = add_joinery(scene, elements, joints, draw_meshes=True, name="ReciprocalMove")
stock = scene.add_group(name="Stock", parent=root)
for pid in sorted(model.plates):
    if model.plates[pid].mesh is not None:
        add_solid(scene, model.plates[pid].mesh, parent=stock, name=f"beam_{pid}", show=False)

publish(scene, "joinery_solver_reciprocal_move")
