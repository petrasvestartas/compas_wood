"""Brep plates to wood_nano outline pairs.

The backend is compas_occt (OCCBrep); faces/loops/edges are duck-typed OCC
wrappers, so nothing OCC-specific is imported at module level.
Facts this module relies on (verified against compas_occt):
- ``face.to_plane()`` reports the RAW surface normal; opposite faces of an OCC
  solid share the same normal direction and differ by ``orientation``
  (0 = FORWARD, 1 = REVERSED). Outward normal = raw normal flipped if REVERSED.
- ``loop.edges`` / ``loop.vertices`` come in wire order; an edge with
  ``orientation == 1`` is traversed last_vertex -> first_vertex in the loop.
- OCC conic parameterization: P(t) = C + r*cos(t)*X + r*sin(t)*Y over
  ``edge.domain`` (full circle hole = one closed edge, domain (0, 2*pi)).
- ``face.centroid`` returns the whole-shape centroid, not the face's - unused.
"""

from math import acos
from math import ceil
from math import cos
from math import radians
from math import sin

from compas.geometry import Point
from compas.geometry import Polyline
from compas.geometry import Vector

_REVERSED = 1  # TopAbs_REVERSED as returned by shape_orientation
_DEFAULT_SEGMENTS = 16
_MIN_SEGMENTS = 4
_MAX_SEGMENTS = 512


def _planar_faces(brep):
    planar = []
    for face in brep.faces:
        if not face.is_plane:
            continue
        plane = face.to_plane()
        raw = Vector(*plane.normal).unitized()
        outward = raw * -1.0 if int(face.orientation) == _REVERSED else raw
        planar.append((face, plane, raw, outward, float(face.area)))
    return planar


def plate_pairs(
    brep,
    tol: float = 1e-6,
    angle_tol_deg: float = 0.5,
    area_ratio: float = 0.99,
    min_pair_fraction: float = 0.0,
    max_pairs: int = 3,
):
    """ALL disjoint opposing planar face pairs of a prismatic solid, largest first.

    A plate yields one pair; a box column yields up to three (one per axis) so
    contacts on every side can be searched. ``min_pair_fraction`` rejects pairs
    whose combined area is below that fraction of the total surface area - a
    curved-dominated solid (screw, dowel, cylinder) then yields no pairs at
    all. Returns a list of ``(bottom_face, top_face)``.
    """
    planar = _planar_faces(brep)
    cos_limit = -cos(radians(max(angle_tol_deg, 1e-9)))
    candidates = []
    for i in range(len(planar)):
        for j in range(i + 1, len(planar)):
            if planar[i][3].dot(planar[j][3]) > cos_limit:
                continue
            ai, aj = planar[i][4], planar[j][4]
            if min(ai, aj) < area_ratio * max(ai, aj):
                continue
            candidates.append((min(ai, aj), i, j))
    candidates.sort(key=lambda c: c[0], reverse=True)
    # min_pair_fraction gates the SOLID, not every pair: a curved-dominated
    # solid (screw, dowel) has no dominant pair at all, but a legitimate plate
    # may still need its smaller secondary pairs to expose a touching face.
    if min_pair_fraction > 0.0 and candidates:
        total = sum(float(face.area) for face in brep.faces)
        _s, bi, bj = candidates[0]
        if (planar[bi][4] + planar[bj][4]) < min_pair_fraction * total:
            return []
    used = set()
    pairs = []
    for _score, i, j in candidates:
        if i in used or j in used or len(pairs) >= max_pairs:
            continue
        used.add(i)
        used.add(j)
        pairs.append(_order_pair(planar[i], planar[j]))
    return pairs


def _order_pair(pi, pj):
    face_i, plane_i, raw_i, _, _ = pi
    face_j, plane_j, raw_j, outward_j, _ = pj
    d = Vector(*plane_j.point) - Vector(*plane_i.point)
    away_i = raw_i.dot(d) < 0.0
    away_j = raw_j.dot(d) > 0.0
    if away_i and not away_j:
        return face_i, face_j
    if away_j and not away_i:
        return face_j, face_i
    return (face_j, face_i) if outward_j.dot(d) > 0.0 else (face_i, face_j)


def plate_faces(
    brep,
    tol: float = 1e-6,
    angle_tol_deg: float = 0.5,
    area_ratio: float = 0.99,
    min_pair_fraction: float = 0.0,
):
    """Find the two opposing plate faces of a plate-like Brep solid.

    Among planar faces, picks the largest pair whose outward normals are
    anti-parallel within ``angle_tol_deg`` and whose areas satisfy
    ``min/max >= area_ratio``. The defaults accept prismatic plates only;
    tapered plates (wedges, t-sections) need e.g. ``angle_tol_deg=16,
    area_ratio=0.25``. Returns ``(bottom_face, top_face)`` where bottom is the
    face whose surface plane normal points away from the other face.

    Raises
    ------
    ValueError
        If no pair of near-parallel planar faces passes the tolerances.
    """
    if len(_planar_faces(brep)) < 2:
        raise ValueError("Not a plate-like solid: fewer than 2 planar faces, need an opposing pair.")
    pairs = plate_pairs(
        brep,
        tol=tol,
        angle_tol_deg=angle_tol_deg,
        area_ratio=area_ratio,
        min_pair_fraction=min_pair_fraction,
        max_pairs=1,
    )
    if not pairs:
        raise ValueError(
            "Not a plate-like solid: no pair of parallel planar faces with matching area "
            "passes the tolerances (or the solid is curved-dominated)."
        )
    return pairs[0]


def outline_from_face(face, deflection: float | None = None) -> tuple[Polyline, list[Polyline]]:
    """Extract the outer loop and inner (hole) loops of a face as closed polylines."""
    outer = _loop_polyline(face.outerloop, deflection)
    holes = [_loop_polyline(loop, deflection) for loop in face.innerloops]
    return outer, holes


def brep_outlines(
    brep,
    tol: float = 1e-6,
    deflection: float | None = None,
    angle_tol_deg: float = 0.5,
    area_ratio: float = 0.99,
    min_pair_fraction: float = 0.0,
    faces=None,
):
    """Bottom/top outlines, hole outlines, and thickness of a plate-like Brep.

    The top outer ring and each top hole ring are rotated/reversed to match the
    winding and start point of their bottom counterpart, so the pair can feed
    the joinery solver directly.

    Returns
    -------
    tuple[Polyline, Polyline, list[Polyline], list[Polyline], float]
        (bottom, top, holes_bottom, holes_top, thickness)
    """
    if faces is None:
        faces = plate_faces(
            brep, tol, angle_tol_deg=angle_tol_deg, area_ratio=area_ratio, min_pair_fraction=min_pair_fraction
        )
    bottom_face, top_face = faces
    bottom, holes_bottom = outline_from_face(bottom_face, deflection)
    top, holes_top = outline_from_face(top_face, deflection)

    plane_b = bottom_face.to_plane()
    plane_t = top_face.to_plane()
    normal_b = Vector(*plane_b.normal).unitized()
    thickness = abs(normal_b.dot(Vector(*plane_t.point) - Vector(*plane_b.point)))

    top = _align_ring(bottom, top)
    bottom, top, holes_bottom, holes_top = _enforce_winding(bottom, top, holes_bottom, holes_top)
    holes_bottom, holes_top = _pair_holes(holes_bottom, holes_top)
    return bottom, top, holes_bottom, holes_top, thickness


def _newell(points) -> Vector:
    n = [0.0, 0.0, 0.0]
    m = len(points)
    for i in range(m):
        p, q = points[i], points[(i + 1) % m]
        n[0] += (p[1] - q[1]) * (p[2] + q[2])
        n[1] += (p[2] - q[2]) * (p[0] + q[0])
        n[2] += (p[0] - q[0]) * (p[1] + q[1])
    return Vector(*n)


def _reversed_ring(polyline: Polyline) -> Polyline:
    return Polyline(list(reversed(polyline.points)))


def _enforce_winding(bottom, top, holes_bottom, holes_top):
    """wood convention (measured 145/145 on the compas_tf plates): the bottom
    ring winds so its Newell normal points TOWARD the top; otherwise the
    solver sees inward side faces and face-to-face detection silently fails."""
    pb, pt = bottom.points, top.points
    ring = pb[:-1] if len(pb) > 1 and pb[0] == pb[-1] else pb
    cb = Vector(*[sum(c) / len(pb) for c in zip(*pb)])
    ct = Vector(*[sum(c) / len(pt) for c in zip(*pt)])
    if _newell(ring).dot(ct - cb) >= 0.0:
        return bottom, top, holes_bottom, holes_top
    return (
        _reversed_ring(bottom),
        _reversed_ring(top),
        [_reversed_ring(h) for h in holes_bottom],
        [_reversed_ring(h) for h in holes_top],
    )


def plate_from_brep(
    brep,
    plate_id: int,
    tol: float = 1e-6,
    deflection: float | None = None,
    angle_tol_deg: float = 0.5,
    area_ratio: float = 0.99,
    min_pair_fraction: float = 0.0,
    faces=None,
    name: str | None = None,
):
    """Build a :class:`compas_wood.model.Plate` from a plate-like Brep solid."""
    from compas_wood.model import Plate

    bottom, top, holes_bottom, holes_top, _ = brep_outlines(
        brep,
        tol=tol,
        deflection=deflection,
        angle_tol_deg=angle_tol_deg,
        area_ratio=area_ratio,
        min_pair_fraction=min_pair_fraction,
        faces=faces,
    )
    return Plate(plate_id, bottom, top, mesh=None, holes_bottom=holes_bottom, holes_top=holes_top, name=name)


# ----------------------------------------------------------------------
# Loop / edge sampling
# ----------------------------------------------------------------------


def _loop_polyline(loop, deflection: float | None) -> Polyline:
    points: list[Point] = []
    for edge in loop.edges:
        points.extend(_edge_points(edge, deflection))
    if not points:
        raise ValueError("Face loop has no edges.")
    points.append(Point(*points[0]))
    return Polyline(points)


def _edge_points(edge, deflection: float | None) -> list[Point]:
    """Points of one edge in loop direction: start included, end excluded."""
    reverse = int(edge.orientation) == _REVERSED
    evaluate = None if edge.is_line else _edge_evaluator(edge)
    if evaluate is None:
        # Line, or an unsupported curve type collapsed to its chord.
        vertex = edge.last_vertex if reverse else edge.first_vertex
        return [Point(*vertex.point)]
    t0, t1 = edge.domain
    n = _segment_count(edge, evaluate, deflection)
    if reverse:
        params = [t1 - (t1 - t0) * k / n for k in range(n)]
    else:
        params = [t0 + (t1 - t0) * k / n for k in range(n)]
    return [evaluate(t) for t in params]


def _edge_evaluator(edge):
    if edge.is_circle:
        circle = edge.to_circle()
        o = Point(*circle.frame.point)
        x = Vector(*circle.frame.xaxis) * circle.radius
        y = Vector(*circle.frame.yaxis) * circle.radius
        return lambda t: o + x * cos(t) + y * sin(t)
    if edge.is_ellipse:
        ellipse = edge.to_ellipse()
        o = Point(*ellipse.frame.point)
        x = Vector(*ellipse.frame.xaxis) * ellipse.major
        y = Vector(*ellipse.frame.yaxis) * ellipse.minor
        return lambda t: o + x * cos(t) + y * sin(t)
    if edge.is_bspline:
        curve = edge.to_bspline()
        return lambda t: Point(*curve.point_at(t))
    if edge.is_bezier:
        curve = edge.to_bezier()
        return lambda t: Point(*curve.point_at(t))
    return None


def _segment_count(edge, evaluate, deflection: float | None) -> int:
    if deflection is None:
        return _DEFAULT_SEGMENTS
    t0, t1 = edge.domain
    radius = None
    if edge.is_circle:
        radius = edge.to_circle().radius
    elif edge.is_ellipse:
        ellipse = edge.to_ellipse()
        radius = max(ellipse.major, ellipse.minor)
    if radius is not None and 0.0 < deflection < radius:
        # sagitta of a chord spanning angle a: radius * (1 - cos(a / 2))
        step = 2.0 * acos(1.0 - deflection / radius)
        n = ceil(abs(t1 - t0) / step)
        return min(max(n, _MIN_SEGMENTS), _MAX_SEGMENTS)
    if radius is not None:
        return _MIN_SEGMENTS
    return _refined_count(evaluate, t0, t1, deflection)


def _refined_count(evaluate, t0: float, t1: float, deflection: float) -> int:
    # Freeform curve: double the count until the mid-sample deviation fits.
    n = 8
    while n < _MAX_SEGMENTS:
        worst = 0.0
        for k in range(n):
            a = t0 + (t1 - t0) * k / n
            b = t0 + (t1 - t0) * (k + 1) / n
            pa = evaluate(a)
            pb = evaluate(b)
            mid = evaluate((a + b) / 2.0)
            chord_mid = Point((pa[0] + pb[0]) / 2.0, (pa[1] + pb[1]) / 2.0, (pa[2] + pb[2]) / 2.0)
            worst = max(worst, mid.distance_to_point(chord_mid))
        if worst <= deflection:
            break
        n *= 2
    return n


# ----------------------------------------------------------------------
# Ring alignment (solver needs matching winding + start points)
# ----------------------------------------------------------------------


def _align_ring(reference: Polyline, ring: Polyline) -> Polyline:
    """Rotate/reverse a closed ring so it runs parallel to the reference ring."""
    ref = reference.points[:-1]
    pts = ring.points[:-1]
    if len(pts) < 3 or not ref:
        return ring
    k = min(range(len(pts)), key=lambda i: pts[i].distance_to_point(ref[0]))
    forward = pts[k:] + pts[:k]
    backward = [forward[0]] + forward[1:][::-1]
    if len(pts) == len(ref):
        cost_f = sum(a.distance_to_point(b) for a, b in zip(forward, ref))
        cost_b = sum(a.distance_to_point(b) for a, b in zip(backward, ref))
        best = forward if cost_f <= cost_b else backward
    else:
        best = forward if _newell_normal(forward).dot(_newell_normal(ref)) >= 0.0 else backward
    return Polyline(best + [Point(*best[0])])


def _newell_normal(points: list[Point]) -> Vector:
    nx = ny = nz = 0.0
    m = len(points)
    for i in range(m):
        a = points[i]
        b = points[(i + 1) % m]
        nx += (a[1] - b[1]) * (a[2] + b[2])
        ny += (a[2] - b[2]) * (a[0] + b[0])
        nz += (a[0] - b[0]) * (a[1] + b[1])
    return Vector(nx, ny, nz)


def _ring_center(ring: Polyline) -> Point:
    pts = ring.points[:-1]
    m = len(pts)
    return Point(sum(p[0] for p in pts) / m, sum(p[1] for p in pts) / m, sum(p[2] for p in pts) / m)


def _pair_holes(holes_bottom: list[Polyline], holes_top: list[Polyline]):
    """Reorder top holes to their nearest bottom hole (by ring center) and align each ring."""
    if not holes_bottom or not holes_top or len(holes_bottom) != len(holes_top):
        return holes_bottom, holes_top
    remaining = list(range(len(holes_top)))
    ordered = []
    for hole in holes_bottom:
        center = _ring_center(hole)
        j = min(remaining, key=lambda i: _ring_center(holes_top[i]).distance_to_point(center))
        remaining.remove(j)
        ordered.append(_align_ring(hole, holes_top[j]))
    return holes_bottom, ordered
