"""Translation shell joinery: face-to-face search with finger joints."""

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import translation_shell_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

thickness = 10.0
chamfer = 1.0
chamfer_angle = 180.0

# 7 joint families x [division length, shift, type]. Family 0 (side-to-side
# in-plane) is overridden to finger joints; the rest stay at kernel defaults.
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

scene = SessionScene("joinery_solver_translation_shell")
add_joinery(scene, elements, joints, draw_meshes=True, name="TranslationShell")
publish(scene, "joinery_solver_translation_shell")
