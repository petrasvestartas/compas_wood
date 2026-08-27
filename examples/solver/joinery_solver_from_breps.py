"""Joinery generation on plate-like Brep solids.

:meth:`compas_wood.PlateModel.from_breps` converts each Brep solid's top/bottom plate faces to
outline polylines (:func:`compas_wood.brep_outlines`, compas_occt backend), so wood_nano
face-to-face / cross detection and joinery generation run on Brep geometry.

Configs (built with ``compas.geometry.Brep.from_box``):

- ``"cross"``: two vertical plates crossing in an X (SEARCH_CROSS_JOINT, joint family 30-39)
- ``"stack"``: two stacked overlapping slabs (SEARCH_FACE_TO_FACE, top-to-top family 40-49)
- ``"corner"``: an L of two plates meeting at an edge (SEARCH_FACE_TO_FACE, top-to-side family 20-29)

The displayed meshes are the solver's carved lofts; the untouched source Breps sit hidden
under a "Stock" group (native OCC Brep display - toggle it on to compare).

For contact DETECTION ONLY on the compas_tf floor model, see ``contact_detection_tf.py``
and ``contact_detection_tf_stress.py``.
"""

from __future__ import annotations

from compas.geometry import Box
from compas.geometry import Brep
from compas.geometry import Frame

from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel

# config -> (boxes, search_type); 2000 x 200-400 x 40 mm plate-like solids.
CONFIGS = {
    "cross": (
        [Box(2000, 40, 400, frame=Frame([0, 0, 0])), Box(40, 2000, 400, frame=Frame([0, 0, 0]))],
        SEARCH_CROSS_JOINT,
    ),
    "stack": (
        [Box(1000, 1000, 40, frame=Frame([0, 0, 20])), Box(1000, 1000, 40, frame=Frame([500, 0, 60]))],
        SEARCH_FACE_TO_FACE,
    ),
    "corner": (
        [Box(1000, 600, 40, frame=Frame([500, 300, -20])), Box(40, 600, 500, frame=Frame([980, 300, 250]))],
        SEARCH_FACE_TO_FACE,
    ),
}


def compute(config="cross", search_type=None):
    if config not in CONFIGS:
        raise ValueError(f"config must be one of {sorted(CONFIGS)}, got {config!r}.")
    boxes, default_search = CONFIGS[config]
    breps = [Brep.from_box(box) for box in boxes]
    search_type = default_search if search_type is None else int(search_type)

    model = PlateModel.from_breps(breps)
    elements, joints = model.solve(search_type=search_type)
    types = sorted((joint.element_ids, joint.joint_type) for joint in joints)
    print(
        f"joinery_solver_from_breps [{config}]: {len(breps)} breps -> {len(model.plates)} plates, "
        f"{len(elements)} elements, {len(joints)} joints {types}."
    )
    return breps, model, elements, joints


def draw(scene, results):
    breps, _, elements, joints = results
    from compas_wood.viewer import PLATE_FACE
    from compas_wood.viewer import add_joinery

    root = add_joinery(scene, elements, joints, draw_meshes=True, show_volumes=True, name="JoinerySolver")
    stock = scene.add_group(name="Stock", parent=root)
    for i, brep in enumerate(breps):
        scene.add(brep, parent=stock, name=f"brep_{i}", facecolor=PLATE_FACE, show=False)
    return root


def main(view=True, config="cross", search_type=None):
    results = compute(config=config, search_type=search_type)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        zoom_to(viewer, aabbs(*results[0]))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate joinery on plate-like Breps.")
    parser.add_argument("config", nargs="?", default="cross", choices=sorted(CONFIGS))
    args = parser.parse_args()
    main(config=args.config)
