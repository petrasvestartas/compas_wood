"""Diamond mesh joinery: out-of-plane side-to-side joints with an extended cut volume."""

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import diamond_mesh_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

u_div = 5
v_div = 2
thickness = -15.0
chamfer = 30.0
chamfer_angle = 180.0

# 7 joint families x [division length, shift, type]. Family 1 (side-to-side
# out-of-plane) is the one that does the joining on this model.
# fmt: off
joint_params = [
    300, 0.5, 1,    # ss_e_ip
    450, 0.64, 15,  # ss_e_op  SIDE-TO-SIDE OUT-OF-PLANE
    450, 0.5, 20,   # ts_e_p
    300, 0.5, 30,   # cr_c_ip
    6, 0.95, 40,    # tt_e_p
    300, 0.5, 58,   # ss_e_r
    300, 1.0, 60,   # b
]
# fmt: on

# extends the joint volume along its length axis, in mm
joint_volume_ext = [0.0, 0.0, -200.0]

_, plates = diamond_mesh_elements(
    u_div=u_div,
    v_div=v_div,
    thickness=thickness,
    chamfer=chamfer,
    chamfer_angle=chamfer_angle,
)
model = PlateModel.from_elements(plates)
elements, joints = model.solve(
    search_type=SEARCH_FACE_TO_FACE,
    joint_params=joint_params,
    joint_volume_ext=joint_volume_ext,
)

scene = SessionScene("joinery_solver_diamond_mesh")
add_joinery(scene, elements, joints, draw_meshes=True, name="DiamondMesh")
publish(scene, "joinery_solver_diamond_mesh")
