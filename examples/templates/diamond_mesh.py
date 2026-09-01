"""Diamond mesh: diamond-pattern triangular plates on the kernel's built-in arch.

Change a number, run it, and the viewer above updates.
"""

from compas.colors import Color
from compas_wood import diamond_mesh_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_shell
from compas_wood.viewer import add_solid

u_div = 5  # plates along the arch
v_div = 2  # plates across it
thickness = -15.0  # negative offsets the plate below the surface
chamfer = 30.0
chamfer_angle = 180.0
explode = True  # unwelded loft, so each plate reads as its own solid

shell, elements = diamond_mesh_elements(
    u_div=u_div,
    v_div=v_div,
    thickness=thickness,
    chamfer=chamfer,
    chamfer_angle=chamfer_angle,
)
print(f"shell: {shell.number_of_faces()} faces, plates: {len(elements)}")

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)

scene = SessionScene("diamond_mesh")
root = scene.add_group(name="DiamondMesh")
add_shell(scene, shell, parent=root, show=False)

for i, element in enumerate(elements):
    group = scene.add_group(name=f"plate_{i}", parent=root)
    add_solid(scene, element.loft_mesh(unwelded=explode), parent=group, name="mesh")
    scene.add(element.top, parent=group, name="top", linecolor=OUTLINE)
    scene.add(element.bottom, parent=group, name="bot", linecolor=OUTLINE)

publish(scene, "diamond_mesh")
