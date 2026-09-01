"""Drawing helpers, mirroring the Rhino plugin's JoinerySolver layer tree.

:data:`LAYERS` is the plugin's ``_LAYER_DEFS`` (same names, colors, default
visibility). Rhino's PlatesMesh black is a wire color; shaded parity is grey
faces + black lines, hence :data:`PLATE_FACE`.

These helpers are backend-agnostic: they only ever call ``scene.add`` and
``scene.add_group``, so any object providing those two works. The two that ship
are :class:`compas_wood.session_scene.SessionScene`, which writes a ``.pb`` for
session_viewer, and :class:`NullScene`, the headless double used by tests/CI.

Everything draws through ``scene.add(item, parent=group, ...)`` rather than
through a group handle, so per-object colors always reach the backend.
"""

from __future__ import annotations

import warnings

from compas.colors import Color
from compas.datastructures import Graph
from compas.datastructures import Mesh
from compas.geometry import Box
from compas.geometry import Frame
from compas.geometry import Point
from compas.geometry import Polyline
from compas.geometry import bounding_box

# (rgb255, default visibility) - parity with plugin_rhino w_solver_joinery_solver._LAYER_DEFS.
LAYERS: dict[str, tuple[tuple[int, int, int], bool]] = {
    "PlatesMesh": ((0, 0, 0), True),
    "CutOutlines": ((0, 120, 220), True),
    "JointArea": ((0, 200, 0), False),
    "JointVolumes": ((220, 50, 50), False),
    "JointLines": ((255, 200, 0), False),
}

# Rhino black is a wire color; shaded parity = grey faces, black lines.
PLATE_FACE = Color(0.8, 0.8, 0.8)


def _layer_color(name: str) -> Color:
    r, g, b = LAYERS[name][0]
    return Color(r / 255, g / 255, b / 255)


def _closed(polyline: Polyline) -> Polyline:
    # Kernel outlines/areas/volumes arrive already closed (first == last point);
    # this only closes foreign input, e.g. user-drawn plate outlines.
    points = polyline.points
    if len(points) < 2 or points[0] == points[-1]:
        return polyline
    return Polyline(list(points) + [points[0]])


def area_mesh(polyline: Polyline, holes: list[Polyline] | None = None) -> Mesh | None:
    """Filled surface for a closed planar outline, triangulated with CDT.

    Contact areas and joint areas are arbitrary planar polygons - frequently
    concave, sometimes with holes - and an ear-clipping fill gets those wrong:
    it can put triangles outside the outline and it has no notion of a hole at
    all. session_py's constrained Delaunay triangulation respects the outline
    (and every hole) as constrained edges, so the fill is exactly the polygon.

    ``RemeshCDT.triangulate`` works in x/y and ignores z, so the loops are first
    projected onto their own plane; the returned indices address the untouched
    3D points, so the mesh comes back in the original plane, not flattened.

    Returns None when the outline cannot be triangulated (fewer than 3 distinct
    points, or degenerate to a line).
    """
    from session_py import Point as SessionPoint
    from session_py import Polyline as SessionPolyline
    from session_py import RemeshCDT

    def _open(loop) -> list:
        pts = list(loop.points)
        if len(pts) >= 2 and pts[0].distance_to_point(pts[-1]) < 1e-9:
            pts = pts[:-1]
        return pts

    loops = [_open(polyline)] + [_open(h) for h in (holes or [])]
    if len(loops[0]) < 3:
        return None

    # Newell normal of the outer loop, then any two axes spanning its plane.
    points = loops[0]
    nx = ny = nz = 0.0
    for i, a in enumerate(points):
        b = points[(i + 1) % len(points)]
        nx += (a[1] - b[1]) * (a[2] + b[2])
        ny += (a[2] - b[2]) * (a[0] + b[0])
        nz += (a[0] - b[0]) * (a[1] + b[1])
    norm = (nx * nx + ny * ny + nz * nz) ** 0.5
    if norm < 1e-12:  # collinear outline - no surface to fill
        return None
    normal = (nx / norm, ny / norm, nz / norm)

    helper = (0.0, 0.0, 1.0) if abs(normal[2]) < 0.9 else (1.0, 0.0, 0.0)
    ux = normal[1] * helper[2] - normal[2] * helper[1]
    uy = normal[2] * helper[0] - normal[0] * helper[2]
    uz = normal[0] * helper[1] - normal[1] * helper[0]
    ulen = (ux * ux + uy * uy + uz * uz) ** 0.5
    u = (ux / ulen, uy / ulen, uz / ulen)
    v = (
        normal[1] * u[2] - normal[2] * u[1],
        normal[2] * u[0] - normal[0] * u[2],
        normal[0] * u[1] - normal[1] * u[0],
    )

    origin = points[0]

    def flatten(loop):
        out = []
        for p in loop:
            d = (p[0] - origin[0], p[1] - origin[1], p[2] - origin[2])
            out.append(
                SessionPoint(
                    d[0] * u[0] + d[1] * u[1] + d[2] * u[2],
                    d[0] * v[0] + d[1] * v[1] + d[2] * v[2],
                    0.0,
                )
            )
        return SessionPolyline(out)

    try:
        triangles = RemeshCDT.triangulate([flatten(loop) for loop in loops])
    except Exception:
        return None
    if not triangles:
        return None

    # Indices address the flat [outer..., hole0..., ...] array, in input order.
    flat = [p for loop in loops for p in loop]
    return Mesh.from_vertices_and_faces(flat, [list(t) for t in triangles])


def edges_graph(mesh: Mesh, coplanar_dot: float = 0.999) -> Graph | None:
    """Boundary + crease edges of a mesh as one Graph - the real solid edges,
    without the triangulation wires of earclipped caps."""
    graph = Graph()
    used = set()
    for u, v in mesh.edges():
        f1, f2 = mesh.edge_faces((u, v))
        if f1 is not None and f2 is not None:
            n1, n2 = mesh.face_normal(f1), mesh.face_normal(f2)
            if n1[0] * n2[0] + n1[1] * n2[1] + n1[2] * n2[2] > coplanar_dot:
                continue
        for k in (u, v):
            if k not in used:
                x, y, z = mesh.vertex_coordinates(k)
                graph.add_node(k, x=x, y=y, z=z)
                used.add(k)
        graph.add_edge(u, v)
    return graph if graph.number_of_edges() else None


def add_solid(
    scene,
    mesh: Mesh,
    *,
    parent=None,
    name: str = "mesh",
    facecolor: Color | None = None,
    edgecolor: Color | None = None,
    show: bool = True,
):
    """Shaded mesh without triangulation wires, plus its real (boundary/crease)
    edges as one graph object."""
    node = scene.add(
        mesh,
        parent=parent,
        name=name,
        facecolor=facecolor if facecolor is not None else PLATE_FACE,
        show_lines=False,
        show=show,
    )
    graph = edges_graph(mesh)
    if graph is not None:
        scene.add(
            graph,
            parent=parent,
            name=f"{name}_edges",
            linecolor=edgecolor if edgecolor is not None else Color(0, 0, 0),
            show_points=False,
            show=show,
        )
    return node


def add_joinery(
    scene,
    elements,
    joints,
    *,
    draw_meshes: bool = False,
    show_areas: bool = False,
    show_volumes: bool = False,
    show_lines: bool = False,
    name: str = "JoinerySolver",
):
    """Draw joinery solver output as the Rhino plugin's group tree.

    Root group ``name``; per-element groups ``elem_{i}`` (loft mesh + cut
    outlines); per-joint groups ``joint_{j}`` (area, volumes, lines). The
    ``draw_meshes`` / ``show_*`` flags set the objects' ``show`` state, matching
    the plugin's default layer visibility, so everything is in the scene and can
    be toggled from the sidebar.

    Parameters
    ----------
    scene : :class:`compas_wood.session_scene.SessionScene` or :class:`NullScene`
    elements : list[:class:`compas_wood.wood_element.JoineryElement`]
    joints : list[:class:`compas_wood.wood_element.JointResult`]

    Returns
    -------
    The root group handle.
    """
    root = scene.add_group(name=name)
    mesh_line = _layer_color("PlatesMesh")
    outline = _layer_color("CutOutlines")
    for i, el in enumerate(elements):
        grp = scene.add_group(name=f"elem_{i}", parent=root)
        add_solid(scene, el.loft_mesh(), parent=grp, name="mesh", edgecolor=mesh_line, show=draw_meshes)
        for k, pl in enumerate(el.top_outlines):
            scene.add(_closed(pl), parent=grp, name=f"top_{k}", linecolor=outline)
        for k, pl in enumerate(el.bottom_outlines):
            scene.add(_closed(pl), parent=grp, name=f"bot_{k}", linecolor=outline)

    area = _layer_color("JointArea")
    volume = _layer_color("JointVolumes")
    line = _layer_color("JointLines")
    for j, jt in enumerate(joints):
        grp = scene.add_group(name=f"joint_{j}", parent=root)
        filled = area_mesh(jt.area)
        if filled is not None:
            scene.add(filled, parent=grp, name="area", facecolor=area, show_lines=False, show=show_areas)
        if len(jt.area.points) >= 2:
            scene.add(_closed(jt.area), parent=grp, name="area_outline", linecolor=area, show=show_areas)
        for v, vol in enumerate(jt.volumes):
            if len(vol.points) >= 2:
                scene.add(_closed(vol), parent=grp, name=f"vol_{v}", linecolor=volume, show=show_volumes)
        for n, ln in enumerate(jt.lines):
            if len(ln.points) >= 2:
                scene.add(ln, parent=grp, name=f"line_{n}", linecolor=line, show=show_lines)
    return root


def add_plate_model(scene, model, *, show_meshes: bool = True, name: str = "Plates"):
    """Draw a :class:`compas_wood.model.PlateModel`: one group per plate,
    named ``{plate_type}_{plate_id}``, with grey mesh and blue outlines.

    Returns
    -------
    The root group handle.
    """
    root = scene.add_group(name=name)
    mesh_line = _layer_color("PlatesMesh")
    outline = _layer_color("CutOutlines")
    for pid in sorted(model.plates):
        plate = model.plates[pid]
        grp = scene.add_group(name=f"{plate.plate_type}_{plate.plate_id}", parent=root)
        if plate.mesh is not None:
            add_solid(scene, plate.mesh, parent=grp, name="mesh", edgecolor=mesh_line, show=show_meshes)
        for label, pl in (("bot", plate.bottom), ("top", plate.top)):
            if pl is not None and len(pl.points) >= 2:
                scene.add(_closed(pl), parent=grp, name=label, linecolor=outline)
        for label, holes in (("hole_bot", plate.holes_bottom), ("hole_top", plate.holes_top)):
            for k, pl in enumerate(holes or []):
                if pl is not None and len(pl.points) >= 2:
                    scene.add(_closed(pl), parent=grp, name=f"{label}_{k}", linecolor=outline)
    return root


def add_shell(scene, shell_mesh, name: str = "shell", **kwargs):
    """Add the reference shell mesh - grey faces, real edges only."""
    parent = kwargs.pop("parent", None)
    return add_solid(
        scene,
        shell_mesh,
        parent=parent,
        name=name,
        facecolor=kwargs.pop("facecolor", None),
        show=kwargs.pop("show", True),
    )


def add_tags(scene, items, name: str = "dots", **kwargs):
    """Rhino TextDot parity: one labelled marker per ``(text, position)`` pair.

    session_py has no text geometry, so each tag is written as a
    :class:`compas.geometry.Point` *named* with its text - the label survives in
    the viewer's object tree, which is where session_viewer surfaces names.
    No-op with a warning on a :class:`NullScene`, which records nothing.

    Extra kwargs are accepted and ignored (they were ``Tag`` styling).

    Returns
    -------
    list
        The scene objects added (empty when skipped).
    """
    if isinstance(scene, NullScene):
        warnings.warn("add_tags: NullScene cannot draw Tags - skipped.", stacklevel=2)
        return []
    grp = scene.add_group(name=name)
    return [scene.add(Point(*position), parent=grp, name=str(text)) for text, position in items]


def aabbs(*geometries) -> list[Box]:
    """Axis-aligned bounding boxes - Mesh via ``aabb()``, polylines via
    point bounds (``Polyline.aabb()`` raises in compas 2.15). ``None`` skipped."""
    boxes: list[Box] = []
    for g in geometries:
        if g is None:
            continue
        if isinstance(g, Box):
            boxes.append(g)
            continue
        aabb = getattr(g, "aabb", None)
        if aabb is not None:
            try:
                boxes.append(aabb() if callable(aabb) else aabb)
                continue
            except Exception:
                pass
        points = getattr(g, "points", None)
        if points:
            bb = bounding_box([[p[0], p[1], p[2]] for p in points])
            lo, hi = bb[0], bb[6]
            # Box.from_bounding_box unitizes a zero-length axis (planar outlines) - build with padded sizes
            sizes = [max(hi[i] - lo[i], 1e-9) for i in range(3)]
            center = [(lo[i] + hi[i]) / 2.0 for i in range(3)]
            boxes.append(Box(*sizes, frame=Frame(center, [1, 0, 0], [0, 1, 0])))
    return boxes


class NullScene:
    """Headless scene double - records ``add``/``add_group`` calls in ``.nodes``
    so tests/CI never need Qt. Group handles are the recorded dicts themselves,
    usable as ``parent=``."""

    def __init__(self):
        self.nodes: list[dict] = []

    def add_group(self, name=None, parent=None, **kwargs) -> dict:
        node = {"item": None, "group": True, "name": name, "parent": parent, "kwargs": kwargs}
        self.nodes.append(node)
        return node

    def add(self, item, parent=None, **kwargs) -> dict:
        node = {"item": item, "parent": parent, "kwargs": kwargs}
        self.nodes.append(node)
        return node
