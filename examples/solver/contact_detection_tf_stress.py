"""STRESS TEST: contact detection on ALL elements of the compas_tf floor model.

Same detection-only display as ``contact_detection_tf.py``, but with relaxed plate
extraction (``angle_tol_deg=16, area_ratio=0.25``) so the tapered wedges, t-sections
and connector solids join the search too, and SEARCH_BOTH (face-to-face + cross).
Every stage is timed and a joint-type histogram is printed. Solids that still have
no usable planar face pair are reported and drawn but excluded from the search.
"""

from __future__ import annotations

import time
import warnings
from collections import Counter
from pathlib import Path

from compas.geometry import Brep

from compas_wood import SEARCH_BOTH
from compas_wood import SEARCH_OPTIONS
from compas_wood import PlateModel

DEFAULT_STEP = "C:/brg/compas_tf/data/cantilevers_baked_model.stp"


def compute(step=DEFAULT_STEP, search_type=SEARCH_BOTH, angle_tol_deg=16.0, area_ratio=0.25):
    if not Path(step).exists():
        raise FileNotFoundError(f"STEP file not found: {step}")
    t0 = time.perf_counter()
    brep = Brep.from_step(str(step))
    solids = list(getattr(brep, "solids", None) or [brep])
    t_load = time.perf_counter() - t0

    t0 = time.perf_counter()
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        model = PlateModel.from_breps(
            solids,
            skip_invalid=True,
            angle_tol_deg=angle_tol_deg,
            area_ratio=area_ratio,
            min_pair_fraction=0.2,
            pairs="all",
            orientations="both",
        )
    t_extract = time.perf_counter() - t0
    sources = {plate.name for plate in model.plates.values()}

    t0 = time.perf_counter()
    elements, joints = model.solve(search_type=int(search_type))
    contacts = model.contacts_by_source(joints)
    t_solve = time.perf_counter() - t0

    histogram = Counter(joint.joint_type for joint in contacts.values())
    print(
        f"contact_detection_tf_stress [{step}]:\n"
        f"  load    {len(solids):4d} solids            {t_load:6.2f}s\n"
        f"  extract {len(model.plates):4d} search plates from {len(sources)} solids "
        f"({len(solids) - len(sources)} rejected: curved/no pair)  {t_extract:6.2f}s\n"
        f"  solve   {len(contacts):4d} contacts           {t_solve:6.2f}s  "
        f"(search={SEARCH_OPTIONS[int(search_type)]})\n"
        f"  types   {dict(sorted(histogram.items()))}"
    )
    return solids, model, elements, list(contacts.values())


def draw(scene, results):
    solids, _, _, joints = results
    from compas.colors import Color

    from compas_wood.viewer import area_mesh

    t0 = time.perf_counter()
    root = scene.add_group(name="StressTest")
    stock = scene.add_group(name="Breps", parent=root)
    for i, solid in enumerate(solids):
        scene.add(solid, parent=stock, name=f"brep_{i}")
    red = Color(0.9, 0.1, 0.1)
    contacts = scene.add_group(name="Contacts", parent=root)
    for joint in joints:
        a, b = joint.element_ids
        filled = area_mesh(joint.area)
        if filled is not None:
            scene.add(filled, parent=contacts, name=f"contact_{a}_{b}", facecolor=red)
        elif len(joint.area.points) >= 2:
            scene.add(joint.area, parent=contacts, name=f"contact_{a}_{b}", linecolor=red, lineswidth=3)
    print(f"  draw    {len(joints):4d} patches            {time.perf_counter() - t0:6.2f}s")
    return root


def main(view=True, step=DEFAULT_STEP, search_type=SEARCH_BOTH, angle_tol_deg=16.0, area_ratio=0.25):
    results = compute(step=step, search_type=search_type, angle_tol_deg=angle_tol_deg, area_ratio=area_ratio)
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

    parser = argparse.ArgumentParser(description="All-elements contact-detection stress test on a STEP model.")
    parser.add_argument("step", nargs="?", default=DEFAULT_STEP)
    parser.add_argument("--search-type", default="both", choices=SEARCH_OPTIONS)
    args = parser.parse_args()
    main(step=args.step, search_type=SEARCH_OPTIONS.index(args.search_type))
