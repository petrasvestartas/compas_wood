"""Translation shell: a cross-section swept along a profile into plate elements.

cross_section / profile = None uses the kernel's built-in arch.
"""

from compas.colors import Color
from compas_wood import translation_shell_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_shell
from compas_wood.viewer import add_solid

cross_section = None
profile = None
thickness = 15.0
chamfer = 2.0
chamfer_angle = 180.0
explode = True  # unwelded loft, so each plate reads as its own solid

shell, elements = translation_shell_elements(
    cross_section=cross_section,
    profile=profile,
    thickness=thickness,
    chamfer=chamfer,
    chamfer_angle=chamfer_angle,
)
print(f"shell: {shell.number_of_faces()} faces, plates: {len(elements)}")

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)

scene = SessionScene("translation_shell")
root = scene.add_group(name="TranslationShell")
add_shell(scene, shell, parent=root, show=False)

for i, element in enumerate(elements):
    group = scene.add_group(name=f"plate_{i}", parent=root)
    add_solid(scene, element.loft_mesh(unwelded=explode), parent=group, name="mesh")
    scene.add(element.top, parent=group, name="top", linecolor=OUTLINE)
    scene.add(element.bottom, parent=group, name="bot", linecolor=OUTLINE)

publish(scene, "translation_shell")
