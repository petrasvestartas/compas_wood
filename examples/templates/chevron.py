"""Chevron: chevron-pattern plates with their joinery metadata.

surface_idx = -1 uses the kernel's built-in flat surface; 0..22 picks an Annen
NURBS surface.
"""

from compas.colors import Color
from compas_wood import chevron_elements
from compas_wood import chevron_elements_annen
from compas_wood import unweld_mesh
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_shell
from compas_wood.viewer import add_solid

surface_idx = -1  # -1 = built-in flat surface, 0..22 = an Annen NURBS surface
u_div = 4
v_division_dist = 900.0
box_height = 760.0
top_plate_inlet = 120.0
plate_thickness = 40.0
edge_rotation = 1.0
edge_offset = 0.5
explode = False

params = dict(
    u_div=u_div,
    v_division_dist=v_division_dist,
    box_height=box_height,
    top_plate_inlet=top_plate_inlet,
    plate_thickness=plate_thickness,
    edge_rotation=edge_rotation,
    edge_offset=edge_offset,
)
if surface_idx >= 0:
    shell, elements, loft_meshes, joint_data = chevron_elements_annen(surface_idx=surface_idx, **params)
else:
    shell, elements, loft_meshes, joint_data = chevron_elements(**params)

typed = sum(1 for face in joint_data["joints_per_face"] if any(x > 0 for x in face)) if joint_data else 0
print(f"shell: {shell.number_of_faces()} faces, plates: {len(elements)}, {typed} typed joints")

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)

scene = SessionScene("chevron")
root = scene.add_group(name="Chevron")
add_shell(scene, shell, parent=root, show=False)

for i, (element, mesh) in enumerate(zip(elements, loft_meshes)):
    group = scene.add_group(name=f"plate_{i}", parent=root)
    add_solid(scene, unweld_mesh(mesh) if explode else mesh, parent=group, name="mesh")
    scene.add(element.top, parent=group, name="top", linecolor=OUTLINE)
    scene.add(element.bottom, parent=group, name="bot", linecolor=OUTLINE)

publish(scene, "chevron")
