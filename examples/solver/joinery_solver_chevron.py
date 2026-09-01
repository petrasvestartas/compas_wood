"""Chevron joinery: the generator's joinery metadata flows into the solver.

chevron_elements returns per-element insertion vectors and joint types plus
global three-valence groups and adjacency pairs; PlateModel carries them and
solve() hands them to the kernel.
"""

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import chevron_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

u_div = 4
v_division_dist = 900.0
box_height = 760.0
top_plate_inlet = 120.0
plate_thickness = 40.0
edge_rotation = 1.0
edge_offset = 0.5

_, plates, loft_meshes, joint_data = chevron_elements(
    u_div=u_div,
    v_division_dist=v_division_dist,
    box_height=box_height,
    top_plate_inlet=top_plate_inlet,
    plate_thickness=plate_thickness,
    edge_rotation=edge_rotation,
    edge_offset=edge_offset,
)
model = PlateModel.from_elements(plates, loft_meshes=loft_meshes, joint_data=joint_data)
elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE)

scene = SessionScene("joinery_solver_chevron")
add_joinery(scene, elements, joints, draw_meshes=True, name="Chevron")
publish(scene, "joinery_solver_chevron")
