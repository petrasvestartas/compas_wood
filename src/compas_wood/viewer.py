"""compas_viewer drawing helpers, mirroring the Rhino plugin's JoinerySolver layer tree.

:data:`LAYERS` is the plugin's ``_LAYER_DEFS`` (same names, colors, default
visibility). Rhino's PlatesMesh black is a wire color; shaded parity in the
viewer is grey faces + black lines, hence :data:`PLATE_FACE`.

facecolor trap: a group handle's ``add`` is core compas ``Group.add``, which
never sees compas_viewer's ``facecolor`` -> ``surfacecolor`` translation, so
``facecolor=`` passed to a group handle is silently dropped. Everything here
draws through ``scene.add(item, parent=group, facecolor=...)`` instead.

No module-level compas_viewer import - the library works without the viewer
extra. Only ``scene.add`` / ``scene.add_group`` are ever called, so
:class:`NullScene` is a drop-in headless double for tests/CI.
"""

from __future__ import annotations

import warnings

from compas.colors import Color
from compas.geometry import Box
from compas.geometry import Frame
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
    scene : :class:`compas_viewer.scene.ViewerScene` or :class:`NullScene`
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
        scene.add(
            el.loft_mesh(),
            parent=grp,
            name="mesh",
            facecolor=PLATE_FACE,
            linecolor=mesh_line,
            show_lines=False,
            show=draw_meshes,
        )
        for k, pl in enumerate(el.top_outlines):
            scene.add(_closed(pl), parent=grp, name=f"top_{k}", linecolor=outline)
        for k, pl in enumerate(el.bottom_outlines):
            scene.add(_closed(pl), parent=grp, name=f"bot_{k}", linecolor=outline)

    area = _layer_color("JointArea")
    volume = _layer_color("JointVolumes")
    line = _layer_color("JointLines")
    for j, jt in enumerate(joints):
        grp = scene.add_group(name=f"joint_{j}", parent=root)
        if len(jt.area.points) >= 2:
            scene.add(_closed(jt.area), parent=grp, name="area", linecolor=area, show=show_areas)
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
            scene.add(
                plate.mesh,
                parent=grp,
                name="mesh",
                facecolor=PLATE_FACE,
                linecolor=mesh_line,
                show_lines=False,
                show=show_meshes,
            )
        for label, pl in (("bot", plate.bottom), ("top", plate.top)):
            if pl is not None and len(pl.points) >= 2:
                scene.add(_closed(pl), parent=grp, name=label, linecolor=outline)
        for label, holes in (("hole_bot", plate.holes_bottom), ("hole_top", plate.holes_top)):
            for k, pl in enumerate(holes or []):
                if pl is not None and len(pl.points) >= 2:
                    scene.add(_closed(pl), parent=grp, name=f"{label}_{k}", linecolor=outline)
    return root


def add_shell(scene, shell_mesh, name: str = "shell", **kwargs):
    """Add the reference shell mesh - grey faces unless overridden via kwargs."""
    kwargs.setdefault("facecolor", PLATE_FACE)
    return scene.add(shell_mesh, name=name, **kwargs)


def add_tags(scene, items, name: str = "dots", **kwargs):
    """Rhino TextDot parity: one camera-facing ``Tag`` per ``(text, position)`` pair.

    Extra kwargs go to the ``Tag`` constructor (``color``, ``height``,
    ``absolute_height``, ...). No-op with a warning on a :class:`NullScene`
    or when compas_viewer is not installed - Tag is viewer-only geometry.

    Returns
    -------
    list
        The scene objects added (empty when skipped).
    """
    if isinstance(scene, NullScene):
        warnings.warn("add_tags: NullScene cannot draw Tags - skipped.", stacklevel=2)
        return []
    try:
        from compas_viewer.scene import Tag
    except Exception as exc:  # compas_viewer extra not installed
        warnings.warn(f"add_tags: compas_viewer Tag unavailable ({exc}) - skipped.", stacklevel=2)
        return []
    grp = scene.add_group(name=name)
    return [scene.add(Tag(str(text), position, **kwargs), parent=grp) for text, position in items]


def aabbs(*geometries) -> list[Box]:
    """Bounding boxes for :func:`zoom_to` - Mesh via ``aabb()``, polylines via
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


def zoom_to(viewer, boxes, tightness: float = 10.0):
    """Aim the camera at the geometry, before ``viewer.show()``.

    What the ``F`` key does - compas_viewer's ``zoom_selected`` - but computed
    from geometry rather than from the scene objects, whose bounding boxes do
    not exist until the renderer has run, so ``F`` cannot be pressed for you.

    Needed on a model in millimetres. The camera starts at ``position``
    ``[-10, -10, 10]`` with a ``far`` plane of ``1000``, and ``far`` is scaled by
    ``camera.scale``, which starts at 1 - so on a 6015 mm building the whole
    thing sits behind the far plane until something sets the scale.

    Parameters
    ----------
    viewer : :class:`compas_viewer.Viewer`
    boxes : sequence[:class:`compas.geometry.Box`]
        The bounding boxes to frame - ``element.aabb`` for model elements,
        ``brep.aabb`` for Breps. Empty leaves the camera alone.
    tightness : float, optional
        Divisor for the camera scale, as in ``zoom_selected``. Larger fills
        more of the window.
    """
    corners = [point for box in boxes if box is not None for point in box.points]
    if not corners:
        return

    low = [min(point[i] for point in corners) for i in range(3)]
    high = [max(point[i] for point in corners) for i in range(3)]
    diagonal = max(sum((high[i] - low[i]) ** 2 for i in range(3)) ** 0.5, 1.0)
    center = [(low[i] + high[i]) / 2 for i in range(3)]

    camera = viewer.renderer.camera
    # scale drives near/far as well as pan speed, so it is what stops the model
    # being clipped away.
    camera.scale = diagonal / tightness

    # Keep the direction the camera is already looking from and only move it -
    # the view vector is position MINUS target, not target minus position. Using
    # the latter is degenerate here: the default position sits almost on the
    # origin, so on a model whose centre is 1.5 m up it points nearly straight
    # down and the camera ends up underneath the building.
    view = [camera.position[i] - camera.target[i] for i in range(3)]
    length = sum(value**2 for value in view) ** 0.5 or 1.0
    camera.target = center
    camera.position = [center[i] + view[i] / length * diagonal for i in range(3)]


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
