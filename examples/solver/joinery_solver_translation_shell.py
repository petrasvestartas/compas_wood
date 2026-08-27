"""Translation shell joinery - FACE_TO_FACE search with finger joints (type 1, division 50)."""

from __future__ import annotations

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


def compute(thickness=10.0, chamfer=1.0, chamfer_angle=180.0):
    _, ts_elements = translation_shell_elements(thickness=thickness, chamfer=chamfer, chamfer_angle=chamfer_angle)
    model = PlateModel.from_elements(ts_elements)
    elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE, joint_params=JOINT_PARAMS)
    print(f"Translation shell: {len(model.plates)} plates in, {len(elements)} elements, {len(joints)} joints.")
    return model, elements, joints


def draw(scene, results):
    _, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=True, name="TranslationShell")


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
