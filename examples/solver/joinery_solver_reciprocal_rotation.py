"""Reciprocal frame (rotation) joinery on a quad grid, cross-joint search.

The displayed meshes are the solver's carved lofts - the volumetric beam with
the cross-joint notches cut in. The uncut stock beams are added hidden under a
"Stock" group for comparison.
"""

from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import PlateModel
from compas_wood import reciprocal_rotation_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery
from compas_wood.viewer import add_solid

nx = 6
ny = 6
width = 6000.0
depth = 5000.0
height = 1500.0
mesh_type = "quad"
angle = 0.2
beam_w = 50.0
beam_h = 200.0
cut_offset = 2.0

_, beams, side0, side1 = reciprocal_rotation_elements(
    nx=nx,
    ny=ny,
    W=width,
    D=depth,
    h=height,
    mesh_type=mesh_type,
    angle=angle,
    beam_w=beam_w,
    beam_h=beam_h,
    cut_offset_factor=cut_offset,
)
model = PlateModel.from_beams(beams, side0, side1)
elements, joints = model.solve(search_type=SEARCH_CROSS_JOINT)

scene = SessionScene("joinery_solver_reciprocal_rotation")
root = add_joinery(scene, elements, joints, draw_meshes=True, name="ReciprocalRotation")
stock = scene.add_group(name="Stock", parent=root)
for pid in sorted(model.plates):
    if model.plates[pid].mesh is not None:
        add_solid(scene, model.plates[pid].mesh, parent=stock, name=f"beam_{pid}", show=False)

publish(scene, "joinery_solver_reciprocal_rotation")
