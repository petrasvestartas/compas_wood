"""Tag joint types on plate edges before solving.

A plate with n boundary edges has a joint-type row of length n + 2 (slots 0/1
are the bottom/top faces); a point matching edge i writes slot i + 2, and -1
means unset. Family codes: 3=ss_ip, 15=ss_op, 20=top-to-side, 30=cr_ip, 40=tt,
58=ss_r, 60=boundary; in-family variants pick alternative cut geometries.
"""

from compas.geometry import Point
from compas.geometry import Polyline
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import assign_joint_types
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_tags

snap_radius = 10.0  # mm; the plugin's 0.1 model units, scaled to this model

# (position, joint-type code). Code 2 on the shared edge picks an alternative
# in-plane cut; code 60 tags plate 0's outer edge as boundary.
dots = [
    (Point(1005.0, 250.0, 0.0), 2),
    (Point(5.0, 250.0, 0.0), 60),
]


def rectangle(x0, y0, x1, y1, z):
    return Polyline([[x0, y0, z], [x1, y0, z], [x1, y1, z], [x0, y1, z], [x0, y0, z]])


# Two 1000 x 500 x 40 mm plates touching along the edge x = 1000.
bottom = [rectangle(0, 0, 1000, 500, 0), rectangle(1000, 0, 2000, 500, 0)]
top = [rectangle(0, 0, 1000, 500, 40), rectangle(1000, 0, 2000, 500, 40)]
model = PlateModel.from_polylines(bottom, top)

changed = assign_joint_types(model, dots, snap_radius=snap_radius)
for pid in sorted(changed):
    print(f"plate {pid}: joint_types = {changed[pid]}")

elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE)

scene = SessionScene("assign_joint_types")
add_plate_model(scene, model, name="Plates")
add_joinery(scene, elements, joints, draw_meshes=True, name="JoinerySolver")
add_tags(scene, [(str(code), point) for point, code in dots])
publish(scene, "assign_joint_types")
