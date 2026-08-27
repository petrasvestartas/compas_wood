from compas.datastructures import Mesh
from compas.geometry import Polyline
from wood_nano._reflex_fold import make_default_reflex_fold
from wood_nano._reflex_fold import make_reflex_fold

from compas_wood.convert import mesh_from_cpp
from compas_wood.wood_element import WoodElement


def reflex_fold_elements(
    cross_section: Polyline | None = None,
    profile: Polyline | None = None,
    thickness: float = 10.0,
    chamfer: float = 20.0,
    chamfer_angle: float = 180.0,
) -> tuple[Mesh, list[WoodElement]]:
    """Fold profile along cross_section → shell mesh + plate elements.

    Parameters
    ----------
    cross_section : compas.geometry.Polyline, optional
        Cross-section curve. None = C++ built-in fold geometry.
    profile : compas.geometry.Polyline, optional
        Sweep path. None = C++ built-in fold geometry.
    chamfer : float
        Miter offset distance applied to both faces of each plate.
    chamfer_angle : float
        Interior corner angle threshold in degrees.

    Returns
    -------
    tuple[Mesh, list[WoodElement]]
    """
    if cross_section is None and profile is None:
        rf = make_default_reflex_fold(float(thickness), float(chamfer), float(chamfer_angle))
    else:
        if cross_section is None or profile is None:
            raise ValueError("Provide both cross_section and profile, or neither.")
        c = [[float(p[0]), float(p[1]), float(p[2])] for p in cross_section.points]
        p = [[float(p[0]), float(p[1]), float(p[2])] for p in profile.points]
        rf = make_reflex_fold(c, p, float(thickness), float(chamfer), float(chamfer_angle))

    return mesh_from_cpp(rf.mesh), [WoodElement(e) for e in rf.elements]
