"""Match tag points / direction lines to plate edges and write joint types and insertion vectors.

Slot convention: a plate with ``n_edges`` boundary edges has ``n_faces = n_edges + 2`` faces.
Slots 0/1 are the bottom/top faces; edge ``i`` maps to face slot ``i + 2``.
"""

from __future__ import annotations

import math
import warnings
from typing import TYPE_CHECKING

from compas.geometry import Polyline
from wood_nano import assign_vectors as _kernel

if TYPE_CHECKING:
    from compas_wood.model import PlateModel

# Lines shorter than this cannot define an insertion direction.
_MIN_LINE_LENGTH = 1e-10


def _to_xyz(pt) -> list[float]:
    return [float(pt[0]), float(pt[1]), float(pt[2])]


def _to_pts(pl) -> list[list[float]]:
    if isinstance(pl, Polyline):
        return [_to_xyz(p) for p in pl.points]
    return [_to_xyz(p) for p in pl]


def _to_start_end(line) -> tuple[list[float], list[float]]:
    if hasattr(line, "start") and hasattr(line, "end"):
        return _to_xyz(line.start), _to_xyz(line.end)
    start, end = line
    return _to_xyz(start), _to_xyz(end)


def _merge_joint_type_row(existing, n_faces: int, slot_map: dict[int, int]) -> list[int]:
    """Write ``slot_map`` into a joint-type row of length ``n_faces``.

    Existing values are kept only when ``len(existing) == n_faces``; otherwise the row resets to -1.
    """
    row = [int(x) for x in existing] if existing and len(existing) == n_faces else [-1] * n_faces
    for face_slot, code in slot_map.items():
        if 0 <= face_slot < n_faces:
            row[face_slot] = int(code)
    return row


def _merge_insertion_vector_row(existing, n_faces: int, slot_map: dict[int, tuple]) -> list[float]:
    """Write ``slot_map`` (slot -> (x, y, z)) into a flat vector row of length ``n_faces * 3``.

    Existing values are kept only when ``len(existing) == n_faces * 3``; otherwise the row resets to 0.0.
    """
    n_floats = n_faces * 3
    row = [float(x) for x in existing] if existing and len(existing) == n_floats else [0.0] * n_floats
    for face_slot, (dx, dy, dz) in slot_map.items():
        if 0 <= face_slot < n_faces:
            row[face_slot * 3 : face_slot * 3 + 3] = [float(dx), float(dy), float(dz)]
    return row


def match_points_to_plate_edges(bot_polylines, points, snap_radius: float = 0.1) -> list[tuple[int, int, int]]:
    """Find all (point, plate edge) pairs within ``snap_radius``.

    Parameters
    ----------
    bot_polylines : sequence of :class:`compas.geometry.Polyline` or list of [x, y, z]
        Bottom outline of each plate.
    points : sequence of :class:`compas.geometry.Point` or [x, y, z]
        Query positions.
    snap_radius : float
        Maximum distance from a point to an edge for a match.

    Returns
    -------
    list[tuple[int, int, int]]
        ``(point_idx, plate_idx, edge_idx)`` per match — ``edge_idx`` is raw, without the +2 slot offset.
    """
    return _kernel.match_points_to_plate_edges(
        [_to_pts(pl) for pl in bot_polylines],
        [_to_xyz(p) for p in points],
        float(snap_radius),
    )


def insertion_vectors_from_lines(
    bot_polylines,
    lines,
    snap_radius: float = 0.1,
) -> list[tuple[int, int, float, float, float]]:
    """Match lines to plate edges and compute insertion vectors from the line directions.

    Parameters
    ----------
    bot_polylines : sequence of :class:`compas.geometry.Polyline` or list of [x, y, z]
        Bottom outline of each plate.
    lines : sequence of :class:`compas.geometry.Line` or (start, end) pairs
        A line matches an edge when either endpoint is within ``snap_radius`` of it.
        Lines shorter than 1e-10 are skipped with a warning.
    snap_radius : float
        Maximum distance from a line endpoint to an edge for a match.

    Returns
    -------
    list[tuple[int, int, float, float, float]]
        ``(plate_idx, face_slot, iv_x, iv_y, iv_z)`` — ``face_slot`` already has the +2 offset, and the
        kernel keeps only the best-distance line per (plate, slot).
        The vector is ``normalize(cross(plate_normal, line_direction))`` — zero when the line is parallel
        to the plate normal.
    """
    starts: list[list[float]] = []
    ends: list[list[float]] = []
    for line in lines:
        start, end = _to_start_end(line)
        if math.dist(start, end) < _MIN_LINE_LENGTH:
            warnings.warn(f"Skipping zero-length line at {start}.", stacklevel=2)
            continue
        starts.append(start)
        ends.append(end)
    if not starts:
        return []
    return _kernel.assign_insertion_vectors(
        [_to_pts(pl) for pl in bot_polylines],
        starts,
        ends,
        float(snap_radius),
    )


def assign_joint_types(model: PlateModel, dots, snap_radius: float = 0.1, plate_ids=None) -> dict[int, list[int]]:
    """Match dot positions to plate edges and tag the joint type on each matched face slot.

    Joint type codes: 3=ss_ip, 15=ss_op, 20=top-to-side, 30=cr_ip, 40=tt, 58=ss_r, 60=boundary.

    Parameters
    ----------
    model : :class:`compas_wood.model.PlateModel`
        Mutated in place via ``model.tag_plate_joinery``.
    dots : sequence of (point, joint_type_code)
        Point is a :class:`compas.geometry.Point` or [x, y, z]; the code is one of the integers above.
    snap_radius : float
        Maximum distance from a dot to a plate edge for a match.
    plate_ids : sequence[int], optional
        Restrict matching to these plates; default is all plates in sorted-id order.

    Returns
    -------
    dict[int, list[int]]
        The changed joint-type rows (length ``n_faces``) keyed by plate id.
        A dot matching edge ``i`` writes to face slot ``i + 2``; the last match wins per slot.
        An existing row is merged only when its length matches ``n_faces``.
    """
    ids = sorted(plate_ids) if plate_ids is not None else model.plate_ids()
    plates = [model.plates[pid] for pid in ids]
    points = [_to_xyz(pt) for pt, _ in dots]
    codes = [int(code) for _, code in dots]

    matches = _kernel.match_points_to_plate_edges(
        [_to_pts(plate.bottom) for plate in plates],
        points,
        float(snap_radius),
    )

    slot_maps: dict[int, dict[int, int]] = {}
    for dot_idx, plate_idx, edge_idx in matches:
        slot_maps.setdefault(plate_idx, {})[edge_idx + 2] = codes[dot_idx]

    changed: dict[int, list[int]] = {}
    for plate_idx, slot_map in slot_maps.items():
        pid = ids[plate_idx]
        plate = model.plates[pid]
        row = _merge_joint_type_row(plate.joint_types, plate.n_faces, slot_map)
        model.tag_plate_joinery(pid, row, plate.insertion_vectors)
        changed[pid] = row
    return changed


def assign_insertion_vectors(
    model: PlateModel,
    lines,
    snap_radius: float = 0.1,
    plate_ids=None,
) -> dict[int, list[float]]:
    """Match lines to plate edges and tag the insertion vector on each matched face slot.

    The stored vector is ``normalize(cross(plate_normal, line_direction))`` — see
    :func:`insertion_vectors_from_lines` for the matching rules.

    Parameters
    ----------
    model : :class:`compas_wood.model.PlateModel`
        Mutated in place via ``model.tag_plate_joinery``.
    lines : sequence of :class:`compas.geometry.Line` or (start, end) pairs
        A line matches an edge when either endpoint is within ``snap_radius`` of it.
        Lines shorter than 1e-10 are skipped with a warning.
    snap_radius : float
        Maximum distance from a line endpoint to an edge for a match.
    plate_ids : sequence[int], optional
        Restrict matching to these plates; default is all plates in sorted-id order.

    Returns
    -------
    dict[int, list[float]]
        The changed flat vector rows (length ``(n_edges + 2) * 3``) keyed by plate id.
        An existing row is merged only when its length matches.
    """
    ids = sorted(plate_ids) if plate_ids is not None else model.plate_ids()
    plates = [model.plates[pid] for pid in ids]

    results = insertion_vectors_from_lines(
        [plate.bottom for plate in plates],
        lines,
        snap_radius,
    )

    slot_maps: dict[int, dict[int, tuple]] = {}
    for plate_idx, face_slot, ix, iy, iz in results:
        slot_maps.setdefault(plate_idx, {})[face_slot] = (ix, iy, iz)

    changed: dict[int, list[float]] = {}
    for plate_idx, slot_map in slot_maps.items():
        pid = ids[plate_idx]
        plate = model.plates[pid]
        row = _merge_insertion_vector_row(plate.insertion_vectors, plate.n_faces, slot_map)
        model.tag_plate_joinery(pid, plate.joint_types, row)
        changed[pid] = row
    return changed
