"""Tag insertion vectors on plate edges before solving.

A line matches an edge when either endpoint is within snap_radius of it, and
the stored vector is normalize(cross(plate_normal, line_direction)) - zero when
the line parallels the plate normal. Rows are flat (n_edges + 2) * 3 floats:
slots 0/1 are the bottom/top faces, edge i is slot i + 2.
"""

from compas.geometry import Line
from compas.geometry import Polyline
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import assign_insertion_vectors
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery
from compas_wood.viewer import add_plate_model

snap_radius = 10.0  # mm; the plugin's 0.1 model units, scaled to this model

lines = [
    Line([1000.0, 250.0, 0.0], [1300.0, 250.0, 0.0]),
    Line([2000.0, 250.0, 0.0], [2000.0, 250.0, 300.0]),
]


def rectangle(x0, y0, x1, y1, z):
    return Polyline([[x0, y0, z], [x1, y0, z], [x1, y1, z], [x0, y1, z], [x0, y0, z]])


# Two 1000 x 500 x 40 mm plates touching along the edge x = 1000.
bottom = [rectangle(0, 0, 1000, 500, 0), rectangle(1000, 0, 2000, 500, 0)]
top = [rectangle(0, 0, 1000, 500, 40), rectangle(1000, 0, 2000, 500, 40)]
model = PlateModel.from_polylines(bottom, top)

changed = assign_insertion_vectors(model, lines, snap_radius=snap_radius)
for pid in sorted(changed):
    row = changed[pid]
    vectors = [tuple(round(v, 3) for v in row[k : k + 3]) for k in range(0, len(row), 3)]
    print(f"plate {pid}: insertion_vectors = {vectors}")

elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE)

scene = SessionScene("assign_insertion_direction")
add_plate_model(scene, model, name="Plates")
group = scene.add_group(name="InsertionLines")
for i, line in enumerate(lines):
    scene.add(line, parent=group, name=f"line_{i}")
add_joinery(scene, elements, joints, draw_meshes=True, name="JoinerySolver")
publish(scene, "assign_insertion_direction")
