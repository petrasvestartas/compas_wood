"""session_py scene backend - the drawing target that replaces compas_viewer.

The viewer is `session_viewer <https://github.com/petrasvestartas/session>`_: a
browser-only WebGPU renderer compiled to WASM. It does not open a desktop
window, so a "view" here means *writing a scene* rather than showing one - the
drawing helpers in :mod:`compas_wood.viewer` build a :class:`SessionScene`, and
:meth:`SessionScene.save` serialises it to a ``.pb`` the viewer fetches.

:class:`SessionScene` is a drop-in for the ``scene`` argument those helpers
already take: it implements the same two calls they use, ``add_group`` and
``add``, so no drawing code changes. compas geometry is converted on the way in
(:class:`compas.datastructures.Mesh` -> :class:`session_py.Mesh`, and so on);
``facecolor`` / ``linecolor`` become the session object's own colors, which is
how the layer palette survives the round trip.

``show=False`` objects are still written. Visibility is the viewer's business -
its tree sidebar toggles them - and dropping them here would make the scene
lie about what the solver produced.
"""

from __future__ import annotations

import warnings
from pathlib import Path

import session_py
from session_py import Color as SessionColor
from session_py import Mesh as SessionMesh
from session_py import Point as SessionPoint
from session_py import Polyline as SessionPolyline
from session_py import Session
from session_py.tree import TreeNode

__all__ = [
    "SessionScene",
    "merge_coplanar_faces",
    "to_session_color",
    "to_session_mesh",
    "to_session_polyline",
]


def to_session_color(color, default: tuple[float, float, float] = (0.0, 0.0, 0.0)) -> SessionColor:
    """compas :class:`~compas.colors.Color` (or any rgb triple) -> session Color.

    Both sides are 0..1 floats, so this only re-wraps; ``None`` yields ``default``.
    """
    if color is None:
        rgb = default
    else:
        rgb = (float(color[0]), float(color[1]), float(color[2]))
    alpha = float(color[3]) if color is not None and len(color) > 3 else 1.0
    return SessionColor(rgb[0], rgb[1], rgb[2], alpha)


def to_session_polyline(polyline, *, name: str = "polyline", linecolor=None, width: float = 1.0) -> SessionPolyline:
    """compas :class:`~compas.geometry.Polyline` -> session Polyline."""
    out = SessionPolyline([SessionPoint(float(p[0]), float(p[1]), float(p[2])) for p in polyline.points])
    out.name = name
    out.linecolor = to_session_color(linecolor)
    out.width = float(width)
    return out


def merge_coplanar_faces(vertices, faces, *, dot_tol: float = 0.999, dist_tol: float = 1e-6):
    """Merge groups of adjacent coplanar faces into single polygonal faces.

    Lofted plates and earclipped joint areas arrive triangulated: a flat plate
    cap is dozens of triangles whose shared edges are not real edges of the
    solid. The viewer draws mesh edges from face topology, so those diagonals
    show up as internal wireframe over what should read as one flat polygon.
    Merging the coplanar run into one n-gon removes them at the source, which
    is also what makes each detected outline *one* face rather than a fan.

    A group is emitted as a single face only when its boundary is exactly one
    closed cycle. A region with a hole, or one pinched so its boundary visits a
    vertex twice, has no single-loop representation, so those keep their
    original faces rather than being silently mangled.

    Returns
    -------
    tuple[list, list[list[int]]]
        The (unchanged) vertices and the rebuilt face list.
    """

    def normal(face):
        # Newell: robust for n-gons and for slivers, unlike a single cross product.
        nx = ny = nz = 0.0
        for i, vi in enumerate(face):
            x0, y0, z0 = vertices[vi]
            x1, y1, z1 = vertices[face[(i + 1) % len(face)]]
            nx += (y0 - y1) * (z0 + z1)
            ny += (z0 - z1) * (x0 + x1)
            nz += (x0 - x1) * (y0 + y1)
        length = (nx * nx + ny * ny + nz * nz) ** 0.5
        return (nx / length, ny / length, nz / length) if length else None

    normals = [normal(f) for f in faces]

    # Undirected edge -> the faces using it. A manifold edge has two.
    edge_faces: dict[tuple[int, int], list[int]] = {}
    for fi, face in enumerate(faces):
        for i, vi in enumerate(face):
            vj = face[(i + 1) % len(face)]
            edge_faces.setdefault((vi, vj) if vi < vj else (vj, vi), []).append(fi)

    def coplanar(a: int, b: int) -> bool:
        na, nb = normals[a], normals[b]
        if na is None or nb is None:
            return False
        if sum(x * y for x, y in zip(na, nb)) < dot_tol:
            return False
        # Same orientation is not enough - two parallel faces on opposite sides
        # of a thin plate would pass. Require b's vertices on a's plane too.
        origin = vertices[faces[a][0]]
        return all(abs(sum(n * (vertices[v][i] - origin[i]) for i, n in enumerate(na))) <= dist_tol for v in faces[b])

    group_of = [-1] * len(faces)
    groups: list[list[int]] = []
    for seed in range(len(faces)):
        if group_of[seed] != -1:
            continue
        gid = len(groups)
        group, stack = [], [seed]
        group_of[seed] = gid
        while stack:
            fi = stack.pop()
            group.append(fi)
            face = faces[fi]
            for i, vi in enumerate(face):
                vj = face[(i + 1) % len(face)]
                for fj in edge_faces[(vi, vj) if vi < vj else (vj, vi)]:
                    if group_of[fj] == -1 and coplanar(fi, fj):
                        group_of[fj] = gid
                        stack.append(fj)
        groups.append(group)

    merged: list[list[int]] = []
    for group in groups:
        if len(group) == 1:
            merged.append(list(faces[group[0]]))
            continue

        # Directed edges of the group; a boundary edge is one whose opposite is
        # not also in the group (i.e. it is not shared with a group member).
        directed = set()
        for fi in group:
            face = faces[fi]
            for i, vi in enumerate(face):
                directed.add((vi, face[(i + 1) % len(face)]))
        boundary = {u: v for u, v in directed if (v, u) not in directed}

        if len(boundary) != len(set(boundary.values())):  # a vertex entered twice
            merged.extend(list(faces[fi]) for fi in group)
            continue

        start = next(iter(boundary))
        loop, current = [start], boundary[start]
        while current != start and current in boundary and len(loop) <= len(boundary):
            loop.append(current)
            current = boundary[current]
        if current != start or len(loop) != len(boundary):  # not one closed cycle
            merged.extend(list(faces[fi]) for fi in group)
            continue
        merged.append(loop)

    return vertices, merged


def to_session_mesh(mesh, *, name: str = "mesh", facecolor=None, merge_coplanar: bool = True) -> SessionMesh:
    """compas :class:`~compas.datastructures.Mesh` -> session Mesh.

    ``to_vertices_and_faces`` returns the vertex keys already compacted to
    0..n-1 indices, which is exactly what session's constructor wants.
    """
    vertices, faces = mesh.to_vertices_and_faces()
    if merge_coplanar:
        vertices, faces = merge_coplanar_faces(vertices, faces)
    out = SessionMesh.from_vertices_and_faces(
        [SessionPoint(float(v[0]), float(v[1]), float(v[2])) for v in vertices],
        [list(face) for face in faces],
    )
    out.name = name
    if facecolor is not None:
        out.set_objectcolor(to_session_color(facecolor))

    return out


class SessionScene:
    """A ``scene`` for the :mod:`compas_wood.viewer` helpers that records into a session.

    Implements only what those helpers call - ``add_group(name=, parent=)`` and
    ``add(item, parent=, **kwargs)`` - and returns :class:`session_py.tree.TreeNode`
    handles, usable as ``parent=`` exactly like compas_viewer's group handles.

    Parameters
    ----------
    name : str, optional
        Name of the root group everything is nested under.

    Examples
    --------
    >>> from compas_wood.session_scene import SessionScene
    >>> scene = SessionScene()
    >>> _ = scene.add_group(name="plates")
    >>> path = scene.save("plates.pb")  # doctest: +SKIP
    """

    def __init__(self, name: str = "compas_wood"):
        self.session = Session()
        self.name = name
        self.root = self.session.add_group(name)
        #: Geometry that could not be converted, as ``(item, reason)`` - see :meth:`add`.
        self.skipped: list[tuple[object, str]] = []

    def add_group(self, name: str | None = None, parent: TreeNode | None = None, **kwargs) -> TreeNode:
        """Nested group. ``session_py.Session.add_group`` always attaches to the
        tree root, so the node is built and placed with ``Session.add`` instead."""
        node = TreeNode(name=name or "group")
        self.session.add(node, parent if parent is not None else self.root)
        return node

    def add(self, item, parent: TreeNode | None = None, **kwargs):
        """Convert and record one geometry.

        Unknown geometry is collected in :attr:`skipped` and warned about rather
        than raised on: one unsupported item should not lose the whole scene.
        """
        parent = parent if parent is not None else self.root
        name = kwargs.get("name") or getattr(item, "name", None) or type(item).__name__

        # compas Graph carries the crease/boundary edges of a mesh (see
        # viewer.edges_graph); session has no graph type, so each edge is written
        # as its own 2-point polyline under one group.
        # A crease/boundary Graph is DROPPED, deliberately. compas_viewer needed it
        # because a mesh there was drawn with `show_lines=False` and had no edges
        # of its own; session_viewer derives edges from face topology and draws
        # them itself. Sending both put two strokes on every edge, in two
        # different lanes - which reads as a thick, doubled, ragged line rather
        # than as the extra information it was meant to be.
        if _is_graph(item):
            return None

        if _is_mesh(item):
            # `show_lines` is not honoured here on purpose: the mesh keeps its
            # edges in the data, and session_viewer decides whether to draw them
            # (off by default, `E` toggles). Baking the choice into the file
            # would leave that key with nothing to reveal.
            mesh = to_session_mesh(item, name=name, facecolor=kwargs.get("facecolor"))
            return self.session.add_mesh(mesh, parent)

        if _is_polyline(item):
            polyline = to_session_polyline(item, name=name, linecolor=kwargs.get("linecolor"))
            return self.session.add_polyline(polyline, parent)

        reason = f"{type(item).__name__} has no session_py equivalent"
        self.skipped.append((item, reason))
        warnings.warn(f"SessionScene.add: skipped {name} - {reason}.", stacklevel=2)
        return None

    def save(self, path: str | Path) -> Path:
        """Write the scene as protobuf (``.pb``) or JSON (``.json``), by suffix."""
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.suffix == ".json":
            self.session.file_json_dump(str(path))
        else:
            self.session.pb_dump(str(path))
        return path


def _pt(xyz) -> SessionPoint:
    return SessionPoint(float(xyz[0]), float(xyz[1]), float(xyz[2]))


# Duck-typed rather than isinstance: compas geometry is imported lazily by the
# callers, and session's own Mesh/Polyline must never be re-converted.
def _is_mesh(item) -> bool:
    return not isinstance(item, session_py.Mesh) and hasattr(item, "to_vertices_and_faces")


def _is_polyline(item) -> bool:
    return not isinstance(item, session_py.Polyline) and hasattr(item, "points") and not hasattr(item, "faces")


def _is_graph(item) -> bool:
    return hasattr(item, "edges") and hasattr(item, "node_coordinates")


#: Where :func:`publish` writes, unless told otherwise. Overridable with the
#: ``COMPAS_WOOD_SCENE_DIR`` environment variable - the docs build points it at
#: the embedded viewer's asset tree.
DEFAULT_SCENE_DIR = "_scenes"


def scene_dir(explicit: str | Path | None = None) -> Path:
    """Resolve the viewer asset root: argument, then env var, then :data:`DEFAULT_SCENE_DIR`."""
    import os

    return Path(explicit or os.environ.get("COMPAS_WOOD_SCENE_DIR") or DEFAULT_SCENE_DIR)


def publish(scene: SessionScene, name: str, directory: str | Path | None = None) -> Path:
    """Write ``scene`` and a one-item manifest for it, and return the manifest path.

    The layout is the viewer's own: ``pb/<name>.pb`` next to ``scenes/<name>.toml``,
    both under the asset root, so the manifest's ``file`` entry and the viewer's
    ``?scene=`` argument are just paths under that root.

    No placement is written. A manifest item with neither ``at`` nor ``xform``
    gets an auto-grid slot from the viewer, which is what a single-item scene
    wants - the geometry keeps the coordinates the solver gave it.
    """
    root = scene_dir(directory)
    pb_path = scene.save(root / "pb" / f"{name}.pb")

    manifest = root / "scenes" / f"{name}.toml"
    manifest.parent.mkdir(parents=True, exist_ok=True)
    manifest.write_text(
        f'# Written by compas_wood.session_scene.publish - regenerate, do not hand-edit.\nname = "{name}"\n\n'
        f'[[items]]\nfile = "pb/{pb_path.name}"\nname = "{name}"\n',
        encoding="utf-8",
    )
    return manifest


def viewer_url(name: str, base: str = "http://localhost:8770") -> str:
    """The URL that shows the scene :func:`publish` wrote under ``name``."""
    return f"{base}/index.html?scene=scenes/{name}.toml"
