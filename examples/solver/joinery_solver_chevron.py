"""Chevron shell joinery - all four generator joint_data channels flow through the PlateModel.

``chevron_elements`` returns joint_data with per-element ``insertion_vectors`` (18 floats)
and ``joints_per_face`` (6 ints), plus global ``three_valence`` groups and ``adjacency``
pairs; :meth:`PlateModel.from_elements` stores them and ``solve`` passes them to the kernel.
"""

from __future__ import annotations

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import chevron_elements


def compute(
    u_div=4,
    v_division_dist=900.0,
    box_height=760.0,
    top_plate_inlet=120.0,
    plate_thickness=40.0,
    edge_rotation=1.0,
    edge_offset=0.5,
):
    _, ch_elements, loft_meshes, joint_data = chevron_elements(
        u_div=u_div,
        v_division_dist=v_division_dist,
        box_height=box_height,
        top_plate_inlet=top_plate_inlet,
        plate_thickness=plate_thickness,
        edge_rotation=edge_rotation,
        edge_offset=edge_offset,
    )
    model = PlateModel.from_elements(ch_elements, loft_meshes=loft_meshes, joint_data=joint_data)
    n_typed = sum(1 for plate in model.plates.values() if any(x > 0 for x in plate.joint_types))
    print(
        f"Chevron: {len(model.plates)} plates, joinery: {n_typed} typed, "
        f"{len(model.three_valence)} three_valence, {len(model.adjacency)} adjacency pairs."
    )
    elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE)
    print(f"Chevron: {len(model.plates)} plates in, {len(elements)} elements, {len(joints)} joints.")
    return model, elements, joints


def draw(scene, results):
    _, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=True, name="Chevron")


def main(view=True, **params):
    results = compute(**params)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        _, elements, _ = results
        zoom_to(viewer, aabbs(*(pl for el in elements for pl in el.bottom_outlines)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
