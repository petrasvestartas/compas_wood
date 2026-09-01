"""Connectors: face plates and edge connectors from the kernel's default VDA mesh.

Face plates are drawn blue, edge connectors orange.
"""

from compas.colors import Color
from compas_wood import PlateModel
from compas_wood import connectors_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_solid

face_thickness = 20.0
face_positions = [0.0]
edge_divisions = [2]
rect_width = 200.0
rect_height = 300.0
rect_thickness = 20.0

face_plates, _, _, edge_plates, _, _ = connectors_elements(
    face_thickness=face_thickness,
    face_positions=face_positions,
    edge_divisions=edge_divisions,
    rect_width=rect_width,
    rect_height=rect_height,
    rect_thickness=rect_thickness,
)
model = PlateModel.from_connectors(face_plates, edge_plates)
print(f"face plates: {len(face_plates)}, total plates: {len(model.plates)}")

FACE_COLOR = Color(0.3, 0.6, 0.9)
EDGE_COLOR = Color(0.9, 0.5, 0.1)

scene = SessionScene("connectors")
root = scene.add_group(name="Connectors")

for pid in sorted(model.plates):
    plate = model.plates[pid]
    color = FACE_COLOR if plate.plate_type == "face" else EDGE_COLOR
    group = scene.add_group(name=f"{plate.plate_type}_{pid}", parent=root)
    if plate.mesh is not None:
        add_solid(scene, plate.mesh, parent=group, name="mesh", edgecolor=color)
    scene.add(plate.bottom, parent=group, name="bot", linecolor=color)
    scene.add(plate.top, parent=group, name="top", linecolor=color)

publish(scene, "connectors")
