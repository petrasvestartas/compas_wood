"""Browse the JSON datasets shipped with wood_nano and solve one through the joinery solver.

Each dataset carries plate outlines plus its solver parameters (search_type, joint_params,
joint_volume_ext, per_element_insertion_vectors, per_element_joint_types, adjacency), which are
passed straight through to :func:`compas_wood.joinery_solver_elements`.

Run ``python datasets_browser.py --list`` to print the available dataset names.
"""

from __future__ import annotations

from collections import Counter

from compas_wood import joinery_solver_elements
from compas_wood import list_datasets
from compas_wood import load_dataset


def compute(name="type_plates_name_hexbox_and_corner"):
    bottom, top, params = load_dataset(name)
    elements, joints = joinery_solver_elements(bottom, top, **params)
    types = Counter(joint.joint_type for joint in joints)
    print(
        f"{name}: {len(bottom)} plates, search_type={params['search_type']}, "
        f"{len(elements)} elements, {len(joints)} joints, types={dict(sorted(types.items()))}."
    )
    return name, elements, joints


def draw(scene, results):
    name, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=True, name=name)


def main(view=True, name="type_plates_name_hexbox_and_corner"):
    results = compute(name=name)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        elements = results[1]
        zoom_to(viewer, aabbs(*(pl for el in elements for pl in el.bottom_outlines + el.top_outlines)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Solve a wood_nano dataset through the joinery solver.")
    parser.add_argument("name", nargs="?", default="type_plates_name_hexbox_and_corner", help="dataset name")
    parser.add_argument("--list", action="store_true", help="print the available dataset names and exit")
    args = parser.parse_args()
    if args.list:
        for dataset_name in list_datasets():
            print(dataset_name)
    else:
        main(name=args.name)
