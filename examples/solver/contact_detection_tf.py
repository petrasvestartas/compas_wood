"""Contact DETECTION on the compas_tf floor model - no joinery generation.

Loads the baked timber-floor STEP export (``compas_tf/data/cantilevers_baked_model.stp``,
237 solids), keeps the plate-like solids (:meth:`compas_wood.PlateModel.from_breps` with
``skip_invalid=True`` drops dowels/cylinders/connectors), and runs the wood_nano
face-to-face search on their top/bottom outline polylines.

The input Breps are drawn untouched AS BREPS (native compas_occt OCCBrepObject - true
B-rep edges, default black); the only geometry added is one RED filled polygon per
detected contact (``JointResult.area``), which is exactly what compas_tf's
``compute_contacts_wood`` consumes. Joint-type classification is printed, not drawn.
"""

from __future__ import annotations

from pathlib import Path

from compas.geometry import Brep

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import SEARCH_OPTIONS
from compas_wood import PlateModel

DEFAULT_STEP = "C:/brg/compas_tf/data/cantilevers_baked_model.stp"


def _breps_from_step(path) -> list:
    brep = Brep.from_step(str(path))
    solids = getattr(brep, "solids", None) or []
    return list(solids) if len(solids) > 1 else [brep]


def compute(step=DEFAULT_STEP, search_type=SEARCH_FACE_TO_FACE):
    if not Path(step).exists():
        raise FileNotFoundError(f"STEP file not found: {step}")
    breps = _breps_from_step(step)
    # relaxed pair tolerances cover the tapered plates (wedges, t-sections);
    # min_pair_fraction rejects curved-dominated solids (screws, dowels);
    # pairs="all" + orientations="both" close the kernel's representation and
    # orientation sensitivities (measured: 95% of ground-truth contacts, 0 false
    # positives, on this model).
    model = PlateModel.from_breps(
        breps,
        skip_invalid=True,
        angle_tol_deg=30.0,
        area_ratio=0.25,
        min_pair_fraction=0.2,
        pairs="all",
        orientations="both",
        max_pairs=6,
        min_face_area=5000.0,
        slab_faces_min_area=1500.0,
    )
    elements, joints = model.solve(search_type=int(search_type))
    contacts = model.contacts_by_source(joints)
    types = sorted((pair, joint.joint_type) for pair, joint in contacts.items())
    print(
        f"contact_detection_tf [{step}]: {len(breps)} solids -> {len(model.plates)} search plates, "
        f"{len(contacts)} contacts {types[:10]}{'...' if len(types) > 10 else ''}."
    )
    return breps, model, elements, list(contacts.values())


def draw(scene, results):
    breps, _, _, joints = results
    from compas.colors import Color

    from compas_wood.viewer import area_mesh

    root = scene.add_group(name="ContactDetection")
    stock = scene.add_group(name="Breps", parent=root)
    for i, brep in enumerate(breps):
        # patches sit between mating faces: untick the Breps group in the sidebar to reveal them
        scene.add(brep, parent=stock, name=f"brep_{i}")
    red = Color(0.9, 0.1, 0.1)
    contacts = scene.add_group(name="Contacts", parent=root)
    for joint in joints:
        a, b = joint.element_ids
        filled = area_mesh(joint.area)
        if filled is not None:
            scene.add(filled, parent=contacts, name=f"contact_{a}_{b}", facecolor=red, show_lines=False)
        if len(joint.area.points) >= 2:
            scene.add(joint.area, parent=contacts, name=f"outline_{a}_{b}", linecolor=Color(0, 0, 0), lineswidth=2)
    return root


def main(view=True, step=DEFAULT_STEP, search_type=SEARCH_FACE_TO_FACE):
    results = compute(step=step, search_type=search_type)
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

    parser = argparse.ArgumentParser(description="Contact detection on a STEP model (default: compas_tf floor).")
    parser.add_argument("step", nargs="?", default=DEFAULT_STEP)
    parser.add_argument("--search-type", default="face_to_face", choices=SEARCH_OPTIONS)
    args = parser.parse_args()
    main(step=args.step, search_type=SEARCH_OPTIONS.index(args.search_type))
