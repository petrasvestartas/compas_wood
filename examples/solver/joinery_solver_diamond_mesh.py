"""Diamond mesh joinery - explicit 21-float joint_params and an extended cut volume.

``joint_volume_ext=[0, 0, -200]`` shrinks/extends the joint volume 200 mm along its
length axis; the side-to-side out-of-plane family (types 10-19) does the joining here.
"""

from __future__ import annotations

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import diamond_mesh_elements

# explicit full 21-float list: 7 families x [division_length, shift, type].
# fmt: off
JOINT_PARAMS = [
    300, 0.5, 1,    # ss_e_ip  SIDE-TO-SIDE IN-PLANE
    450, 0.64, 15,  # ss_e_op  SIDE-TO-SIDE OUT-OF-PLANE - active on this model
    450, 0.5, 20,   # ts_e_p
    300, 0.5, 30,   # cr_c_ip
    6, 0.95, 40,    # tt_e_p
    300, 0.5, 58,   # ss_e_r
    300, 1.0, 60,   # b
]
# fmt: on

JOINT_VOLUME_EXT = [0.0, 0.0, -200.0]


def compute(u_div=5, v_div=2, thickness=-15.0, chamfer=30.0, chamfer_angle=180.0):
    _, dm_elements = diamond_mesh_elements(
        u_div=u_div, v_div=v_div, thickness=thickness, chamfer=chamfer, chamfer_angle=chamfer_angle
    )
    model = PlateModel.from_elements(dm_elements)
    elements, joints = model.solve(
        search_type=SEARCH_FACE_TO_FACE,
        joint_params=JOINT_PARAMS,
        joint_volume_ext=JOINT_VOLUME_EXT,
    )
    print(f"Diamond mesh: {len(model.plates)} plates in, {len(elements)} elements, {len(joints)} joints.")
    return model, elements, joints


def draw(scene, results):
    _, elements, joints = results
    from compas_wood.viewer import add_joinery

    return add_joinery(scene, elements, joints, draw_meshes=True, name="DiamondMesh")


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
