"""Tag insertion vectors on plate edges - parity port of the Rhino plugin command
w_solver_assign_insertion_direction.

The Rhino command matches user-drawn lines to plate edges and writes an insertion vector onto the
matched face slot; here plain :class:`compas.geometry.Line` objects play that role and the rows are
written into a :class:`compas_wood.PlateModel` via :func:`compas_wood.assign_insertion_vectors`.

Verified kernel semantics: the stored vector is ``normalize(cross(plate_normal, line_direction))``
and zero when the line parallels the plate normal. A line matches an edge when either endpoint is
within ``snap_radius`` of it; the kernel keeps only the best-distance line per (plate, face slot).
Rows are flat ``(n_edges + 2) * 3`` floats - slots 0/1 = bottom/top faces, edge ``i`` = slot ``i + 2``.

The plugin's default snap radius is 0.1 model units; the demo plates are millimetre-scale,
so the default here is scaled to 10 mm.
"""

from __future__ import annotations

from compas.geometry import Line
from compas.geometry import Polyline

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import assign_insertion_vectors

# First line starts on the shared edge x=1000 and runs along +X: stored vector
# cross((0,0,1), (1,0,0)) = (0,1,0). Second line starts on plate 1's outer edge
# x=2000 and runs along +Z (parallel to the plate normal): stored vector is zero.
DEFAULT_LINES = [
    Line([1000.0, 250.0, 0.0], [1300.0, 250.0, 0.0]),
    Line([2000.0, 250.0, 0.0], [2000.0, 250.0, 300.0]),
]


def _rect(x0: float, y0: float, x1: float, y1: float, z: float) -> Polyline:
    return Polyline([[x0, y0, z], [x1, y0, z], [x1, y1, z], [x0, y1, z], [x0, y0, z]])


def compute(snap_radius=10.0, lines=None, search_type=SEARCH_FACE_TO_FACE):
    # Two 1000 x 500 x 40 mm plates touching along the edge x = 1000.
    bottom = [_rect(0, 0, 1000, 500, 0), _rect(1000, 0, 2000, 500, 0)]
    top = [_rect(0, 0, 1000, 500, 40), _rect(1000, 0, 2000, 500, 40)]
    model = PlateModel.from_polylines(bottom, top)

    lines = list(lines) if lines is not None else list(DEFAULT_LINES)
    changed = assign_insertion_vectors(model, lines, snap_radius=snap_radius)
    for pid in sorted(changed):
        row = changed[pid]
        vectors = [tuple(round(v, 3) for v in row[k : k + 3]) for k in range(0, len(row), 3)]
        print(f"  plate {pid}: insertion_vectors = {vectors}")
    print(f"assign_insertion_direction: {len(changed)} plate(s) updated from {len(lines)} line(s).")

    elements, joints = model.solve(search_type=search_type)
    print(f"assign_insertion_direction: {len(elements)} elements, {len(joints)} joints.")
    return model, lines, changed, elements, joints


def draw(scene, results):
    model, lines, _, elements, joints = results
    from compas_wood.viewer import add_joinery
    from compas_wood.viewer import add_plate_model

    add_plate_model(scene, model, name="Plates")
    grp = scene.add_group(name="InsertionLines")
    for i, line in enumerate(lines):
        scene.add(line, parent=grp, name=f"line_{i}")
    return add_joinery(scene, elements, joints, draw_meshes=True, name="JoinerySolver")


def main(view=True, snap_radius=10.0, lines=None, search_type=SEARCH_FACE_TO_FACE):
    results = compute(snap_radius=snap_radius, lines=lines, search_type=search_type)
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
