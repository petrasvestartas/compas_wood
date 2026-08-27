"""Compare Brep contact detection against ground truth and SHOW the missing pairs.

Ground truth = the design outline pairs exported from the compas_tf model
(``compas_tf/data/plate_outlines_model_space.json``, written with::

    # in the compas_tf repo / venv
    import json, compas

    m = compas.json_load("data/cantilevers_baked_model.json")
    plates = [e for e in m.elements() if type(e).__name__ == "PlateElement"]
    out = []
    for p in plates:
        T = p.modeltransformation
        out.append(
            {
                "bottom": [list(q) for q in (p.bottom.transformed(T) if T else p.bottom).points],
                "top": [list(q) for q in (p.top.transformed(T) if T else p.top).points],
            }
        )
    json.dump(out, open("data/plate_outlines_model_space.json", "w"))

) solved with the same wood face-to-face search.

Viewer: untouched Breps (native), DETECTED contacts as filled RED patches,
MISSING pairs (in ground truth but not detected from the Breps) as filled
YELLOW patches plus a yellow line connecting the two solids' centers.
"""

from __future__ import annotations

import json
import warnings
from pathlib import Path

from compas.geometry import Brep
from compas.geometry import Polyline
from compas.geometry import centroid_points

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import joinery_solver_elements

DEFAULT_STEP = "C:/brg/compas_tf/data/cantilevers_baked_model.stp"
DEFAULT_TRUTH = "C:/brg/compas_tf/data/plate_outlines_model_space.json"


def compute(step=DEFAULT_STEP, truth=DEFAULT_TRUTH, search_type=SEARCH_FACE_TO_FACE):
    for path in (step, truth):
        if not Path(path).exists():
            raise FileNotFoundError(path)
    data = json.load(open(truth))
    bottoms = [Polyline(d["bottom"] + [d["bottom"][0]]) for d in data]
    tops = [Polyline(d["top"] + [d["top"][0]]) for d in data]
    _, truth_joints = joinery_solver_elements(bottoms, tops, search_type=int(search_type))
    truth_map = {}
    for joint in truth_joints:
        truth_map[tuple(sorted(joint.element_ids))] = joint

    solids = list(Brep.from_step(str(step)).solids)
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        model = PlateModel.from_breps(
            solids,
            skip_invalid=True,
            angle_tol_deg=30.0,
            area_ratio=0.25,
            min_pair_fraction=0.2,
            pairs="all",
            orientations="both",
            max_pairs=6,
            slab_faces_min_area=5000.0,
        )
    _, joints = model.solve(search_type=int(search_type))
    detected = model.contacts_by_source(joints)

    # match solids to truth plates by centroid so pair sets are comparable
    centers = [centroid_points(d["bottom"] + d["top"]) for d in data]
    solid_to_truth = {}
    for si, solid in enumerate(solids):
        c = solid.aabb.frame.point
        best = min(
            range(len(centers)),
            key=lambda ti: (c[0] - centers[ti][0]) ** 2 + (c[1] - centers[ti][1]) ** 2 + (c[2] - centers[ti][2]) ** 2,
        )
        d2 = (c[0] - centers[best][0]) ** 2 + (c[1] - centers[best][1]) ** 2 + (c[2] - centers[best][2]) ** 2
        if d2 < 150.0**2:
            solid_to_truth[f"solid_{si}"] = best

    detected_truth_pairs = set()
    for (sa, sb), joint in detected.items():
        ta, tb = solid_to_truth.get(sa), solid_to_truth.get(sb)
        if ta is not None and tb is not None:
            detected_truth_pairs.add(tuple(sorted((ta, tb))))

    missing = {pair: truth_map[pair] for pair in truth_map if pair not in detected_truth_pairs}

    def _aabb_gap(s1, s2):
        b1, b2 = s1.aabb, s2.aabb
        g = -1e12
        for k, sz1, sz2 in ((0, b1.xsize, b2.xsize), (1, b1.ysize, b2.ysize), (2, b1.zsize, b2.zsize)):
            low1, high1 = b1.frame.point[k] - sz1 / 2, b1.frame.point[k] + sz1 / 2
            low2, high2 = b2.frame.point[k] - sz2 / 2, b2.frame.point[k] + sz2 / 2
            g = max(g, low1 - high2, low2 - high1)
        return g

    truth_to_solid = {ti: si for si, ti in ((int(k.split("_")[1]), v) for k, v in solid_to_truth.items())}
    print(
        f"compare_contacts_tf: truth {len(truth_map)} pairs | detected (plate-plate) "
        f"{len(detected_truth_pairs & set(truth_map))} | MISSING {len(missing)}:"
    )
    for pair in sorted(missing):
        sa, sb = truth_to_solid.get(pair[0]), truth_to_solid.get(pair[1])
        gap = _aabb_gap(solids[sa], solids[sb]) if sa is not None and sb is not None else float("nan")
        note = "design-only: carved solids DO NOT TOUCH" if gap > 0.5 else "should touch - real miss"
        print(f"  {pair}: carved gap {gap:6.1f} mm  ({note})")
    return solids, list(detected.values()), missing, centers


def draw(scene, results):
    solids, detected_joints, missing, centers = results
    from compas.colors import Color
    from compas.geometry import Line

    from compas_wood.viewer import area_mesh

    root = scene.add_group(name="ContactComparison")
    stock = scene.add_group(name="Breps", parent=root)
    for i, solid in enumerate(solids):
        scene.add(solid, parent=stock, name=f"brep_{i}")

    red = Color(0.9, 0.1, 0.1)
    found = scene.add_group(name="Detected", parent=root)
    for joint in detected_joints:
        filled = area_mesh(joint.area)
        if filled is not None:
            scene.add(filled, parent=found, name="contact", facecolor=red)

    yellow = Color(1.0, 0.8, 0.0)
    miss = scene.add_group(name="MISSING", parent=root)
    for (a, b), joint in sorted(missing.items()):
        filled = area_mesh(joint.area)
        if filled is not None:
            scene.add(filled, parent=miss, name=f"missing_{a}_{b}", facecolor=yellow, linecolor=yellow)
        else:
            scene.add(joint.area, parent=miss, name=f"missing_{a}_{b}", linecolor=yellow, lineswidth=5)
        scene.add(
            Line(centers[a], centers[b]),
            parent=miss,
            name=f"missing_link_{a}_{b}",
            linecolor=yellow,
            lineswidth=3,
        )
    return root


def main(view=True, step=DEFAULT_STEP, truth=DEFAULT_TRUTH, search_type=SEARCH_FACE_TO_FACE):
    results = compute(step=step, truth=truth, search_type=search_type)
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

    parser = argparse.ArgumentParser(description="Show detected (red) vs missing (yellow) contacts.")
    parser.add_argument("step", nargs="?", default=DEFAULT_STEP)
    parser.add_argument("--truth", default=DEFAULT_TRUTH)
    args = parser.parse_args()
    main(step=args.step, truth=args.truth)
