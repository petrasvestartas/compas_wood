"""Generic joinery solver - parity port of the Rhino plugin command w_solver_joinery_solver.

Solves a :class:`compas_wood.PlateModel` (from a JSON file, an instance, or the built-in
translation shell when none is given) and draws the result as the plugin's JoinerySolver
group tree via :func:`compas_wood.viewer.add_joinery`.
"""

from __future__ import annotations

import time
from pathlib import Path

from compas_wood import SEARCH_OPTIONS
from compas_wood import PlateModel
from compas_wood import translation_shell_elements


def _resolve_model(model) -> PlateModel:
    if model is None:
        return PlateModel.from_elements(translation_shell_elements()[1])
    if isinstance(model, PlateModel):
        return model
    from compas import json_load

    loaded = json_load(str(Path(model)))
    if not isinstance(loaded, PlateModel):
        raise TypeError(f"{model}: expected a PlateModel JSON, got {type(loaded).__name__}.")
    return loaded


def _resolve_search_type(search_type) -> int:
    if isinstance(search_type, str):
        name = search_type.lower()
        if name not in SEARCH_OPTIONS:
            raise ValueError(f"search_type must be one of {SEARCH_OPTIONS} or 0-2, got {search_type!r}.")
        return SEARCH_OPTIONS.index(name)
    return int(search_type)


def compute(
    model=None,
    search_type="face_to_face",
    joint_params=None,
    joint_volume_ext=None,
    plate_ids=None,
):
    model = _resolve_model(model)
    n_in = len(model.plates) if plate_ids is None else sum(1 for p in set(plate_ids) if int(p) in model.plates)
    t0 = time.perf_counter()
    elements, joints = model.solve(
        search_type=_resolve_search_type(search_type),
        joint_params=joint_params,
        joint_volume_ext=joint_volume_ext,
        plate_ids=plate_ids,
    )
    seconds = time.perf_counter() - t0
    print(f"Joinery solver: {n_in} plates in, {len(elements)} elements, {len(joints)} joints, {seconds:.2f}s solve.")
    return model, elements, joints


def draw(scene, results, draw_meshes=False):
    _, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=draw_meshes, name="JoinerySolver")


def main(
    view=True,
    model=None,
    search_type="face_to_face",
    joint_params=None,
    joint_volume_ext=None,
    draw_meshes=False,
    plate_ids=None,
):
    results = compute(
        model=model,
        search_type=search_type,
        joint_params=joint_params,
        joint_volume_ext=joint_volume_ext,
        plate_ids=plate_ids,
    )
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results, draw_meshes=draw_meshes)
        _, elements, _ = results
        zoom_to(viewer, aabbs(*(pl for el in elements for pl in el.bottom_outlines + el.top_outlines)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results, draw_meshes=draw_meshes)
    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Run the joinery solver on a PlateModel.")
    parser.add_argument("--model", default=None, help="path to a PlateModel JSON (default: built-in translation shell)")
    parser.add_argument("--search-type", default="face_to_face", choices=SEARCH_OPTIONS)
    parser.add_argument("--draw-meshes", action="store_true")
    args = parser.parse_args()
    main(model=args.model, search_type=args.search_type, draw_meshes=args.draw_meshes)
