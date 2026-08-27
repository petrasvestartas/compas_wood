"""Minimal compas_wood demo: default translation shell, one scene group per plate."""

from compas.colors import Color

from compas_wood import translation_shell_elements
from compas_wood.viewer import add_solid

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)


def compute():
    shell, elements = translation_shell_elements()
    return {"shell": shell, "elements": elements}


def draw(scene, results):
    root = scene.add_group(name="Hello")
    for i, el in enumerate(results["elements"]):
        grp = scene.add_group(name=f"plate_{i}", parent=root)
        add_solid(scene, el.loft_mesh(), parent=grp, name="mesh")
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
