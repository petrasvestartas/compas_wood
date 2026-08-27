from compas.datastructures import Mesh
from compas.geometry import Polyline
from wood_nano import _joinery_solver

from compas_wood.convert import mesh_from_cpp


def loft(bottom: list, top: list) -> Mesh:
    """Loft two sets of polylines (bottom + top) into a closed mesh with hole support.

    Index 0 = outer boundary, indices 1+ = holes.
    Accepts compas.geometry.Polyline or raw list-of-[x,y,z].

    Returns
    -------
    :class:`compas.datastructures.Mesh`
    """

    def _to_pts(pl) -> list:
        if isinstance(pl, Polyline):
            return [[float(p[0]), float(p[1]), float(p[2])] for p in pl.points]
        if hasattr(pl, "coords"):
            c = pl.coords
            return [[float(c[i]), float(c[i + 1]), float(c[i + 2])] for i in range(0, len(c), 3)]
        return [[float(p[0]), float(p[1]), float(p[2])] for p in pl]

    bot = [_to_pts(pl) for pl in bottom]
    tp = [_to_pts(pl) for pl in top]
    return mesh_from_cpp(_joinery_solver.loft(bot, tp))
