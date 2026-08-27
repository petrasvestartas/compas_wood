"""Minimal Brep-to-outlines conversion demo.

An OCC boolean (box minus cylinder) makes a plate-like solid with a hole;
:func:`compas_wood.brep_outlines` extracts the bottom/top outer outlines, the paired hole
outlines, and the plate thickness - the exact polylines the joinery solver consumes.
Bottom/top outlines are drawn blue, hole outlines red.
"""

from __future__ import annotations

from compas.colors import Color
from compas.geometry import Box
from compas.geometry import Brep
from compas.geometry import Cylinder
from compas.geometry import Frame

from compas_wood import brep_outlines

BLUE = Color.blue()
RED = Color.red()


def compute(box_size=(600.0, 400.0, 40.0), hole_radius=80.0, hole_center=(100.0, 50.0)):
    box = Brep.from_box(Box(*box_size, frame=Frame([0, 0, 0])))
    cylinder = Brep.from_cylinder(Cylinder(hole_radius, box_size[2] * 5, frame=Frame([*hole_center, 0])))
    solid = box - cylinder

    bottom, top, holes_bottom, holes_top, thickness = brep_outlines(solid)
    print(f"brep_outlines: thickness = {thickness}")
    print(f"  bottom: {len(bottom.points)} points, top: {len(top.points)} points")
    print(f"  holes: {len(holes_bottom)} bottom {[len(h.points) for h in holes_bottom]}", end=", ")
    print(f"{len(holes_top)} top {[len(h.points) for h in holes_top]}")
    return bottom, top, holes_bottom, holes_top, thickness


def draw(scene, results):
    bottom, top, holes_bottom, holes_top, _ = results
    root = scene.add_group(name="BrepOutlines")
    scene.add(bottom, parent=root, name="bottom", linecolor=BLUE)
    scene.add(top, parent=root, name="top", linecolor=BLUE)
    for label, holes in (("hole_bot", holes_bottom), ("hole_top", holes_top)):
        for k, hole in enumerate(holes):
            scene.add(hole, parent=root, name=f"{label}_{k}", linecolor=RED)
    return root


def main(view=True, box_size=(600.0, 400.0, 40.0), hole_radius=80.0, hole_center=(100.0, 50.0)):
    results = compute(box_size=box_size, hole_radius=hole_radius, hole_center=hole_center)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        bottom, top = results[0], results[1]
        zoom_to(viewer, aabbs(bottom, top))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
