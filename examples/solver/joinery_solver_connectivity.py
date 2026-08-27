"""Joint-family connectivity table - which elements meet, with what joint type and family."""

from __future__ import annotations

from compas_wood import FAMILY_LABELS
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import translation_shell_elements

# family 0 (ss_e_ip) overridden to division 50 / type 1; other families at kernel defaults.
# fmt: off
JOINT_PARAMS = [
    50, 0.5, 1,     # ss_e_ip  SIDE-TO-SIDE IN-PLANE
    450, 0.64, 15,  # ss_e_op
    450, 0.5, 20,   # ts_e_p
    300, 0.5, 30,   # cr_c_ip
    6, 0.95, 40,    # tt_e_p
    300, 0.5, 58,   # ss_e_r
    300, 1.0, 60,   # b
]
# fmt: on


def joint_family(joint_type: int) -> str:
    index = 0 if joint_type < 10 else joint_type // 10
    return FAMILY_LABELS[index] if 0 <= index < len(FAMILY_LABELS) else "unknown"


def print_connectivity(elements, joints) -> None:
    print(f"{'idx':>4}  {'el_a':>5}  {'el_b':>5}  {'type':>5}  {'area_pts':>8}  family")
    print("-" * 72)
    for idx, joint in enumerate(joints):
        el_a, el_b = joint.element_ids
        print(
            f"{idx:>4}  {el_a:>5}  {el_b:>5}  {joint.joint_type:>5}  "
            f"{len(joint.area.points):>8}  {joint_family(joint.joint_type)}"
        )
    print(f"\n{len(elements)} elements, {len(joints)} joints total")


def compute(thickness=10.0, chamfer=1.0, chamfer_angle=180.0):
    _, ts_elements = translation_shell_elements(thickness=thickness, chamfer=chamfer, chamfer_angle=chamfer_angle)
    model = PlateModel.from_elements(ts_elements)
    elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE, joint_params=JOINT_PARAMS)
    print_connectivity(elements, joints)
    return model, elements, joints


def draw(scene, results):
    _, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=True, name="Connectivity")


def main(view=True, **params):
    results = compute(**params)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        _, elements, _ = results
        zoom_to(viewer, aabbs(*(pl for el in elements for pl in el.bottom_outlines)))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
