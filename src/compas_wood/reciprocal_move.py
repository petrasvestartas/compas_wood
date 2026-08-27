from compas.datastructures import Mesh
from compas.geometry import Polyline
from wood_nano import _reciprocal_move
from wood_nano._reciprocal_move import make_default_reciprocal_move_typed
from wood_nano._reciprocal_move import make_reciprocal_move_from_mesh
from wood_nano._reciprocal_move import make_reciprocal_move_from_surface

from compas_wood.convert import mesh_from_cpp
from compas_wood.convert import polyline_from_cpp


def _unpack(rm, beam_offsets: list[float] | None, unweld_beams: bool = True):
    # Offsets applied by C++ on the still-C++ meshes (was per-point Python
    # arithmetic over numpy rows, in the fourth diverging copy of this
    # logic; it also dropped face_tris/face_holes from offset beams).
    if beam_offsets:
        _reciprocal_move.apply_beam_offsets(rm, [float(o) for o in beam_offsets])

    dome = mesh_from_cpp(rm.dome_mesh)
    # beams_unwelded duplicates vertices per face in C++; read after
    # apply_beam_offsets so offset beams unweld the offset geometry.
    beams = [mesh_from_cpp(m) for m in (rm.beams_unwelded if unweld_beams else rm.beams)]
    side0 = [polyline_from_cpp(p) for p in rm.side0]
    side1 = [polyline_from_cpp(p) for p in rm.side1]
    return dome, beams, side0, side1


def reciprocal_move_elements(
    nx: int = 12,
    ny: int = 10,
    W: float = 12000.0,
    D: float = 10000.0,
    h: float = 3000.0,
    mesh_type: str = "quad",
    angle: float = 50.0,
    beam_w: float = 100.0,
    beam_h: float = 0.0,
    extend_factor: float = 5.0,
    cut_offset_factor: float = 1.0,
    beam_offsets: list[float] | None = None,
    unweld_beams: bool = True,
) -> tuple[Mesh, list[Mesh], list[Polyline], list[Polyline]]:
    """Translation-based reciprocal frame on a sinusoidal dome."""
    rm = make_default_reciprocal_move_typed(
        nx, ny, W, D, h, mesh_type, angle, beam_w, beam_h, extend_factor, cut_offset_factor
    )
    return _unpack(rm, beam_offsets, unweld_beams)


def reciprocal_move_elements_from_surface(
    pts: list[list[float]],
    knots_u: list[float],
    knots_v: list[float],
    degree_u: int,
    degree_v: int,
    n_u: int,
    n_v: int,
    mesh_type: str = "quad",
    u_div: int = 12,
    v_div: int = 10,
    angle: float = 50.0,
    beam_w: float = 100.0,
    beam_h: float = 0.0,
    extend_factor: float = 5.0,
    cut_offset_factor: float = 1.0,
    beam_offsets: list[float] | None = None,
    unweld_beams: bool = True,
) -> tuple[Mesh, list[Mesh], list[Polyline], list[Polyline]]:
    """Translation-based reciprocal frame on a NURBS surface."""
    rm = make_reciprocal_move_from_surface(
        pts,
        knots_u,
        knots_v,
        degree_u,
        degree_v,
        n_u,
        n_v,
        mesh_type,
        u_div,
        v_div,
        angle,
        beam_w,
        beam_h,
        extend_factor,
        cut_offset_factor,
    )
    return _unpack(rm, beam_offsets, unweld_beams)


def reciprocal_move_elements_from_mesh(
    vertices: list[list[float]],
    faces: list[list[int]],
    angle: float = 50.0,
    beam_w: float = 100.0,
    beam_h: float = 0.0,
    extend_factor: float = 5.0,
    cut_offset_factor: float = 1.0,
    beam_offsets: list[float] | None = None,
    unweld_beams: bool = True,
) -> tuple[Mesh, list[Mesh], list[Polyline], list[Polyline]]:
    """Translation-based reciprocal frame on a user-supplied mesh."""
    rm = make_reciprocal_move_from_mesh(vertices, faces, angle, beam_w, beam_h, extend_factor, cut_offset_factor)
    return _unpack(rm, beam_offsets, unweld_beams)
