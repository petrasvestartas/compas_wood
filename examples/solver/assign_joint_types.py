"""Tag joint types on plate edges - parity port of the Rhino plugin command w_solver_assign_joint_types.

The Rhino command matches TextDots to plate edges and writes the joint-type code onto the matched
face slot; here plain ``(point, code)`` pairs play the TextDot role and the rows are written into a
:class:`compas_wood.PlateModel` via :func:`compas_wood.assign_joint_types`.

Slot convention: a plate with ``n_edges`` boundary edges has a joint-type row of length
``n_edges + 2`` (slots 0/1 = bottom/top faces); a dot matching edge ``i`` writes face slot
``i + 2``; ``-1`` means unset. Family default codes: 3=ss_ip, 15=ss_op, 20=top-to-side,
30=cr_ip, 40=tt, 58=ss_r, 60=boundary - in-family variants (e.g. 1-9 for ss_ip) select
alternative cut geometries.

The plugin's default snap radius is 0.1 model units; the demo plates are millimetre-scale,
so the default here is scaled to 10 mm.
"""

from __future__ import annotations

from compas.geometry import Point
from compas.geometry import Polyline

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import assign_joint_types

# (position, joint-type code): code 2 on the shared edge picks an alternative ss_ip cut,
# code 60 tags plate 0's outer edge as boundary.
DEFAULT_DOTS = [
    (Point(1005.0, 250.0, 0.0), 2),
    (Point(5.0, 250.0, 0.0), 60),
]


def _rect(x0: float, y0: float, x1: float, y1: float, z: float) -> Polyline:
    return Polyline([[x0, y0, z], [x1, y0, z], [x1, y1, z], [x0, y1, z], [x0, y0, z]])


def compute(snap_radius=10.0, dots=None, search_type=SEARCH_FACE_TO_FACE, save=None):
    # Two 1000 x 500 x 40 mm plates touching along the edge x = 1000.
    bottom = [_rect(0, 0, 1000, 500, 0), _rect(1000, 0, 2000, 500, 0)]
    top = [_rect(0, 0, 1000, 500, 40), _rect(1000, 0, 2000, 500, 40)]
    model = PlateModel.from_polylines(bottom, top)

    dots = list(dots) if dots is not None else list(DEFAULT_DOTS)
    changed = assign_joint_types(model, dots, snap_radius=snap_radius)
    for pid in sorted(changed):
        print(f"  plate {pid}: joint_types = {changed[pid]}")
    print(f"assign_joint_types: {len(changed)} plate(s) updated from {len(dots)} dot(s).")

    if save is not None:
        from compas import json_dump

        json_dump(model, str(save))
        print(f"assign_joint_types: model saved to {save}.")

    elements, joints = model.solve(search_type=search_type)
    print(f"assign_joint_types: {len(elements)} elements, {len(joints)} joints.")
    return model, dots, changed, elements, joints


def draw(scene, results):
    model, dots, _, elements, joints = results
    from compas_wood.viewer import add_joinery
    from compas_wood.viewer import add_plate_model
    from compas_wood.viewer import add_tags

    add_plate_model(scene, model, name="Plates")
    root = add_joinery(scene, elements, joints, draw_meshes=True, name="JoinerySolver")
    add_tags(scene, [(str(code), pt) for pt, code in dots])
    return root


def main(view=True, snap_radius=10.0, dots=None, search_type=SEARCH_FACE_TO_FACE, save=None):
    results = compute(snap_radius=snap_radius, dots=dots, search_type=search_type, save=save)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        elements = results[3]
        zoom_to(viewer, aabbs(*(el.loft_mesh() for el in elements)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
