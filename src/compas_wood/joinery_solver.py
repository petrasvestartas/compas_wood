from compas.geometry import Polyline
from wood_nano import _joinery_solver

from compas_wood.wood_element import JoineryElement
from compas_wood.wood_element import JointResult

SEARCH_FACE_TO_FACE = 0
SEARCH_CROSS_JOINT = 1
SEARCH_BOTH = 2

# index == search_type int, for string-based UIs
SEARCH_OPTIONS = ["face_to_face", "cross_joint", "both"]

# mirrors wood_globals.cpp reset_defaults: 7 families x [distance, shift, type]
# fmt: off
JOINT_PARAMS_DEFAULTS = [
    300, 0.5, 3,     # ss_e_ip  SIDE-TO-SIDE IN-PLANE      (types 1-9)
    450, 0.64, 15,   # ss_e_op  SIDE-TO-SIDE OUT-OF-PLANE  (types 10-19)
    450, 0.5, 20,    # ts_e_p   TOP-TO-SIDE                (types 20-29)
    300, 0.5, 30,    # cr_c_ip  CROSS-JOINT IN-PLANE       (types 30-39)
    6, 0.95, 40,     # tt_e_p   TOP-TO-TOP                 (types 40-49)
    300, 0.5, 58,    # ss_e_r   SIDE-TO-SIDE ROTATED       (types 50-59)
    300, 1.0, 60,    # b        BOUNDARY                   (types 60-69)
]
# fmt: on

JOINT_VOLUME_EXT_DEFAULT = [0.0, 0.0, 0.0]

FAMILY_LABELS = [
    "SIDE-TO-SIDE IN-PLANE (1-9)",
    "SIDE-TO-SIDE OUT-OF-PLANE (10-19)",
    "TOP-TO-SIDE (20-29)",
    "CROSS-JOINT IN-PLANE (30-39)",
    "TOP-TO-TOP (40-49)",
    "SIDE-TO-SIDE ROTATED (50-59)",
    "BOUNDARY (60-69)",
]


def joinery_solver_elements(
    bottom_polylines: list,
    top_polylines: list,
    search_type: int = SEARCH_CROSS_JOINT,
    joint_params: list | None = None,
    joint_volume_ext: list | None = None,
    per_element_insertion_vectors: list | None = None,
    per_element_joint_types: list | None = None,
    three_valence: list | None = None,
    adjacency: list | None = None,
    bottom_hole_polylines: list | None = None,
    top_hole_polylines: list | None = None,
) -> tuple[list[JoineryElement], list[JointResult]]:
    """Detect joinery between plate elements defined by bottom/top polyline pairs.

    Each pair (bottom_polylines[i], top_polylines[i]) defines one plate element.
    Accepts compas.geometry.Polyline or raw list-of-[x,y,z].

    Returns
    -------
    tuple[list[JoineryElement], list[JointResult]]
    """
    if len(bottom_polylines) != len(top_polylines):
        raise ValueError(
            f"bottom_polylines ({len(bottom_polylines)}) and "
            f"top_polylines ({len(top_polylines)}) must have the same length."
        )

    def _to_pts(pl) -> list:
        if isinstance(pl, Polyline):
            return [(float(p[0]), float(p[1]), float(p[2])) for p in pl.points]
        if hasattr(pl, "coords"):
            c = pl.coords
            return [(float(c[i]), float(c[i + 1]), float(c[i + 2])) for i in range(0, len(c), 3)]
        return [(float(p[0]), float(p[1]), float(p[2])) for p in pl]

    plates_data = []
    for i, (bot, top) in enumerate(zip(bottom_polylines, top_polylines)):
        plate = [_to_pts(bot), _to_pts(top)]
        if (
            bottom_hole_polylines
            and i < len(bottom_hole_polylines)
            and top_hole_polylines
            and i < len(top_hole_polylines)
        ):
            for h_bot, h_top in zip(bottom_hole_polylines[i], top_hole_polylines[i]):
                plate.append(_to_pts(h_bot))
                plate.append(_to_pts(h_top))
        plates_data.append(plate)

    raw = _joinery_solver.solve_joinery(
        plates_data,
        int(search_type),
        joint_params if joint_params else [],
        joint_volume_ext if joint_volume_ext else [],
        [tuple(float(x) for x in iv) for iv in per_element_insertion_vectors] if per_element_insertion_vectors else [],
        [tuple(int(x) for x in jt) for jt in per_element_joint_types] if per_element_joint_types else [],
        [tuple(int(x) for x in tv) for tv in three_valence] if three_valence else [],
        [(int(a), int(b)) for a, b in adjacency] if adjacency else [],
    )

    elements = [JoineryElement(d) for d in raw["elements"]]
    joints = [JointResult(d) for d in raw["joints"]]
    return elements, joints
