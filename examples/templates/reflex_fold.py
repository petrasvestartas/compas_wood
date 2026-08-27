"""Reflex fold template (port of plugin command w_template_reflex_fold).

cross_section/profile = None uses the kernel's built-in fold geometry.
"""

from compas.colors import Color

from compas_wood import reflex_fold_elements
from compas_wood.viewer import add_shell
from compas_wood.viewer import add_solid

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)


def compute(
    cross_section=None,
    profile=None,
    thickness=10.0,
    chamfer=20.0,
    chamfer_angle=180.0,
    explode=True,
):
    shell, elements = reflex_fold_elements(
        cross_section=cross_section,
        profile=profile,
        thickness=thickness,
        chamfer=chamfer,
        chamfer_angle=chamfer_angle,
    )
    return {"shell": shell, "elements": elements, "explode": explode}


def draw(scene, results):
    root = scene.add_group(name="ReflexFold")
    add_shell(scene, results["shell"], parent=root, show=False)
    for i, el in enumerate(results["elements"]):
        grp = scene.add_group(name=f"plate_{i}", parent=root)
        mesh = el.loft_mesh(unwelded=results["explode"])
        add_solid(
            scene,
            mesh,
            parent=grp,
            name="mesh",
        )
        scene.add(el.top, parent=grp, name="top", linecolor=OUTLINE)
        scene.add(el.bottom, parent=grp, name="bot", linecolor=OUTLINE)
    return root


def main(view=True, **params):
    results = compute(**params)
    print(f"shell: {results['shell'].number_of_faces()} faces  plates: {len(results['elements'])}")
    if view:
        from compas_viewer import Viewer

        viewer = Viewer()
        draw(viewer.scene, results)
        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        zoom_to(viewer, aabbs(results["shell"]))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
