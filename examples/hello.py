"""Minimal compas_wood demo: the built-in translation shell, one group per plate."""

from compas.colors import Color
from compas_wood import translation_shell_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_solid

shell, elements = translation_shell_elements()
print(f"shell: {shell.number_of_faces()} faces, plates: {len(elements)}")

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)

scene = SessionScene("hello")
root = scene.add_group(name="Hello")

for i, element in enumerate(elements):
    group = scene.add_group(name=f"plate_{i}", parent=root)
    add_solid(scene, element.loft_mesh(), parent=group, name="mesh")
    scene.add(element.top, parent=group, name="top", linecolor=OUTLINE)
    scene.add(element.bottom, parent=group, name="bot", linecolor=OUTLINE)

publish(scene, "hello")
