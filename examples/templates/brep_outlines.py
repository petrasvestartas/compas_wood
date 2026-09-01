"""Brep to outlines: the polylines the joinery solver consumes.

An OCC boolean (box minus cylinder) makes a plate-like solid with a hole.
Outer outlines are drawn blue, hole outlines red.
"""

from compas.colors import Color
from compas.geometry import Box
from compas.geometry import Brep
from compas.geometry import Cylinder
from compas.geometry import Frame
from compas_wood import brep_outlines
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish

box_size = (600.0, 400.0, 40.0)
hole_radius = 80.0
hole_center = (100.0, 50.0)

box = Brep.from_box(Box(*box_size, frame=Frame([0, 0, 0])))
cylinder = Brep.from_cylinder(Cylinder(hole_radius, box_size[2] * 5, frame=Frame([*hole_center, 0])))
solid = box - cylinder

bottom, top, holes_bottom, holes_top, thickness = brep_outlines(solid)

BLUE = Color.blue()
RED = Color.red()

scene = SessionScene("brep_outlines")
root = scene.add_group(name="BrepOutlines")
scene.add(bottom, parent=root, name="bottom", linecolor=BLUE)
scene.add(top, parent=root, name="top", linecolor=BLUE)
for label, holes in (("hole_bot", holes_bottom), ("hole_top", holes_top)):
    for k, hole in enumerate(holes):
        scene.add(hole, parent=root, name=f"{label}_{k}", linecolor=RED)

publish(scene, "brep_outlines")
