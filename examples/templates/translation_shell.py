"""Translation shell template (port of plugin command w_template_translation_shell).

cross_section/profile = None uses the kernel's built-in arch.
"""

from compas.colors import Color

from compas_wood import translation_shell_elements
from compas_wood.viewer import PLATE_FACE
from compas_wood.viewer import add_shell

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)


def compute(
    cross_section=None,
    profile=None,
    thickness=15.0,
    chamfer=2.0,
    chamfer_angle=180.0,
    explode=True,
):
    shell, elements = translation_shell_elements(
        cross_section=cross_section,
        profile=profile,
        thickness=thickness,
        chamfer=chamfer,
        chamfer_angle=chamfer_angle,
    )
    return {"shell": shell, "elements": elements, "explode": explode}


def draw(scene, results):
    root = scene.add_group(name="TranslationShell")
    add_shell(scene, results["shell"], parent=root, show=False)
    for i, el in enumerate(results["elements"]):
        grp = scene.add_group(name=f"plate_{i}", parent=root)
        mesh = el.loft_mesh(unwelded=results["explode"])
        scene.add(mesh, parent=grp, name="mesh", facecolor=PLATE_FACE, show_lines=False)
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
