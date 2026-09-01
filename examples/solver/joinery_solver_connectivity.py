"""Connectivity table: which elements meet, with what joint type and family."""

from compas_wood import FAMILY_LABELS
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import translation_shell_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

thickness = 10.0
chamfer = 1.0
chamfer_angle = 180.0

# fmt: off
joint_params = [
    50, 0.5, 1,     # ss_e_ip  SIDE-TO-SIDE IN-PLANE
    450, 0.64, 15,  # ss_e_op
    450, 0.5, 20,   # ts_e_p
    300, 0.5, 30,   # cr_c_ip
    6, 0.95, 40,    # tt_e_p
    300, 0.5, 58,   # ss_e_r
    300, 1.0, 60,   # b
]
# fmt: on

_, plates = translation_shell_elements(thickness=thickness, chamfer=chamfer, chamfer_angle=chamfer_angle)
model = PlateModel.from_elements(plates)
elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE, joint_params=joint_params)

# The table is the point of this example, so it is printed rather than drawn.
print(f"{'idx':>4}  {'el_a':>5}  {'el_b':>5}  {'type':>5}  {'area_pts':>8}  family")
print("-" * 72)
for idx, joint in enumerate(joints):
    el_a, el_b = joint.element_ids
    family_index = 0 if joint.joint_type < 10 else joint.joint_type // 10
    family = FAMILY_LABELS[family_index] if 0 <= family_index < len(FAMILY_LABELS) else "unknown"
    print(f"{idx:>4}  {el_a:>5}  {el_b:>5}  {joint.joint_type:>5}  {len(joint.area.points):>8}  {family}")
print(f"\n{len(elements)} elements, {len(joints)} joints total")

scene = SessionScene("joinery_solver_connectivity")
add_joinery(scene, elements, joints, draw_meshes=True, name="Connectivity")
publish(scene, "joinery_solver_connectivity")
