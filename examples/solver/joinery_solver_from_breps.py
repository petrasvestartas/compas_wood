"""Contact DETECTION on Brep solids - no joinery generation.

The input Breps are drawn untouched (grey); the only geometry added is one RED
closed polygon per detected contact (``JointResult.area``). The solver's joint
classification (type codes) is printed to the console but no cut geometry is
displayed - this mirrors what compas_tf's ``compute_contacts_wood`` consumes:
the contact interface polygons, nothing more.

:meth:`compas_wood.PlateModel.from_breps` converts each Brep solid's top/bottom plate faces to
outline polylines (:func:`compas_wood.brep_outlines`, compas_occt backend), so wood_nano
face-to-face / cross detection runs on Brep geometry.

Configs (built with ``compas.geometry.Brep.from_box``):

- ``"cross"``: two vertical plates crossing in an X (SEARCH_CROSS_JOINT, joint family 30-39)
- ``"stack"``: two stacked overlapping slabs (SEARCH_FACE_TO_FACE, top-to-top family 40-49)
- ``"corner"``: an L of two plates meeting at an edge (SEARCH_FACE_TO_FACE, top-to-side family 20-29)

``step=PATH`` loads Brep solids from a STEP file via ``compas.geometry.Brep.from_step`` instead
(searched with SEARCH_BOTH unless ``search_type`` is given). Non-plate solids (dowels,
cylinders, connectors) are skipped with a warning. Real-world demo on the compas_tf floor::

    python examples/solver/joinery_solver_from_breps.py --step C:/brg/compas_tf/data/cantilevers_baked_model.stp

which loads 237 solids, keeps the 145 plate-like ones, and finds their contacts.

The displayed meshes are the solver's carved lofts; the uncut source Breps sit hidden
under a "Stock" group.

Source Breps are drawn as grey meshes via ``brep.to_viewmesh()`` (compas_occt OCCBrep also offers
``to_tesselation`` - same signature and return - and ``to_meshes``; ``to_viewmesh`` is used here
because it returns one merged mesh plus boundary polylines).
"""

from __future__ import annotations

from compas.geometry import Box
from compas.geometry import Brep
from compas.geometry import Frame

from compas_wood import SEARCH_BOTH
from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import SEARCH_OPTIONS
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


def _breps_from_step(path) -> list:
    brep = Brep.from_step(str(path))
    solids = getattr(brep, "solids", None) or []
    return list(solids) if len(solids) > 1 else [brep]


def compute(config="cross", step=None, search_type=None):
    if step is not None:
        breps = _breps_from_step(step)
        search_type = SEARCH_BOTH if search_type is None else int(search_type)
        source = str(step)
    else:
        if config not in CONFIGS:
            raise ValueError(f"config must be one of {sorted(CONFIGS)}, got {config!r}.")
        boxes, default_search = CONFIGS[config]
        breps = [Brep.from_box(box) for box in boxes]
        search_type = default_search if search_type is None else int(search_type)
        source = config

    model = PlateModel.from_breps(breps, skip_invalid=step is not None)
    elements, joints = model.solve(search_type=search_type)
    types = sorted((joint.element_ids, joint.joint_type) for joint in joints)
    print(
        f"joinery_solver_from_breps [{source}]: {len(breps)} breps -> {len(model.plates)} plates, "
        f"{len(elements)} elements, {len(joints)} joints {types}."
    )
    return breps, model, elements, joints


def draw(scene, results):
    breps, _, _, joints = results
    from compas.colors import Color

    from compas_wood.viewer import add_shell

    root = scene.add_group(name="ContactDetection")
    stock = scene.add_group(name="Breps", parent=root)
    for i, brep in enumerate(breps):
        mesh, _ = brep.to_viewmesh()
        add_shell(scene, mesh, name=f"brep_{i}", parent=stock, show_lines=False)
    red = Color(0.9, 0.1, 0.1)
    contacts = scene.add_group(name="Contacts", parent=root)
    for joint in joints:
        if len(joint.area.points) >= 2:
            a, b = joint.element_ids
            scene.add(joint.area, parent=contacts, name=f"contact_{a}_{b}", linecolor=red, lineswidth=3)
    return root


def main(view=True, config="cross", step=None, search_type=None):
    results = compute(config=config, step=step, search_type=search_type)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        breps = results[0]
        zoom_to(viewer, aabbs(*(brep.to_viewmesh()[0] for brep in breps)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Run the joinery solver on plate-like Breps.")
    parser.add_argument("config", nargs="?", default="cross", choices=sorted(CONFIGS))
    parser.add_argument("--step", default=None, help="load Brep solids from a STEP file instead")
    parser.add_argument("--search-type", default=None, choices=SEARCH_OPTIONS)
    args = parser.parse_args()
    search = None if args.search_type is None else SEARCH_OPTIONS.index(args.search_type)
    main(config=args.config, step=args.step, search_type=search)
