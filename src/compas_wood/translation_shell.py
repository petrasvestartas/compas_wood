from compas.datastructures import Mesh
from compas.geometry import Polyline
from wood_nano._translation_shell import make_default_translation_shell
from wood_nano._translation_shell import make_translation_shell

from compas_wood.convert import mesh_from_cpp
from compas_wood.wood_element import WoodElement


def translation_shell_elements(
    cross_section: Polyline | None = None,
    profile: Polyline | None = None,
    thickness: float = 10.0,
    chamfer: float = 1.0,
    chamfer_angle: float = 180.0,
) -> tuple[Mesh, list[WoodElement]]:
    """Sweep cross_section along profile → shell mesh + plate elements.

    Parameters
    ----------
    cross_section : compas.geometry.Polyline, optional
        Cross-section curve. None = C++ built-in arch.
    profile : compas.geometry.Polyline, optional
        Sweep path. None = C++ built-in arch.
    thickness : float
        Plate thickness.
    chamfer : float
        Chamfer cut distance. 0 = no chamfer.
    chamfer_angle : float
        Corners sharper than this angle (degrees) are chamfered.

    Returns
    -------
    tuple[Mesh, list[WoodElement]]
    """
    if cross_section is None and profile is None:
        ts = make_default_translation_shell(float(thickness), float(chamfer), float(chamfer_angle))
    else:
        if cross_section is None or profile is None:
            raise ValueError("Provide both cross_section and profile, or neither.")
        c = [[float(p[0]), float(p[1]), float(p[2])] for p in cross_section.points]
        p = [[float(p[0]), float(p[1]), float(p[2])] for p in profile.points]
        ts = make_translation_shell(c, p, float(thickness), float(chamfer), float(chamfer_angle))

    return mesh_from_cpp(ts.mesh), [WoodElement(e) for e in ts.elements]
