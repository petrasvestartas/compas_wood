from compas.geometry import Frame
from compas.geometry import Polyline
from wood_nano._connectors import make_default_vda_mesh
from wood_nano._connectors import make_vda_mesh

from compas_wood.convert import frame_from_cpp
from compas_wood.convert import polyline_from_cpp


def connectors_elements(
    mesh: tuple[list[list[float]], list[list[int]]] | None = None,
    face_thickness: float = 20.0,
    face_positions: tuple[float, ...] | list[float] = (0.0,),
    edge_divisions: tuple[int, ...] | list[int] = (2,),
    edge_division_len: tuple[float, ...] | list[float] = (),
    insertion_lines: tuple | list[list[list[float]]] = (),
    rect_width: float = 200.0,
    rect_height: float = 200.0,
    rect_thickness: float = 20.0,
) -> tuple[
    list[list[Polyline]],
    list[list[Frame | None]],
    list[list[str]],
    list[list[Polyline]],
    list[list[Frame | None]],
    list[list[str]],
]:
    """Mesh-to-plates-and-connectors.

    Returns
    -------
    tuple of six parallel nested lists
        ``(f_polylines, f_frames, f_index, e_polylines, e_frames, e_index)``
    """
    fp = list(face_positions) or [0.0]
    ed = list(edge_divisions) or [2]
    edl = list(edge_division_len)
    ils = list(insertion_lines)

    if mesh is None:
        raw = make_default_vda_mesh(face_thickness, fp, ed, edl, ils, rect_width, rect_height, rect_thickness)
    else:
        verts, faces = mesh
        raw = make_vda_mesh(verts, faces, face_thickness, fp, ed, edl, ils, rect_width, rect_height, rect_thickness)

    f_polylines = [[polyline_from_cpp(pl) for pl in row] for row in raw.f_polylines]
    f_frames = [[frame_from_cpp(pl) if pl is not None else None for pl in row] for row in raw.f_polylines_planes]
    f_index = [list(row) for row in raw.f_polylines_index]
    e_polylines = [[polyline_from_cpp(pl) for pl in row] for row in raw.e_polylines]
    e_frames = [[frame_from_cpp(pl) if pl is not None else None for pl in row] for row in raw.e_polylines_planes]
    e_index = [list(row) for row in raw.e_polylines_index]

    return f_polylines, f_frames, f_index, e_polylines, e_frames, e_index
