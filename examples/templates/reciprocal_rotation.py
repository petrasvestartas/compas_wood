"""Reciprocal rotation template (port of plugin command w_template_reciprocal_rotation).

Rotation-based reciprocal frame on the kernel's sinusoidal dome; plugin
``u_div``/``v_div``/``cut_offset`` map to ``nx``/``ny``/``cut_offset_factor``.
"""

from compas_wood import PlateModel
from compas_wood import reciprocal_rotation_elements
from compas_wood import unweld_mesh
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_shell


def compute(
    mesh_type="quad",
    u_div=6,
    v_div=6,
    W=6000.0,
    D=5000.0,
    h=1500.0,
    angle=0.2,
    beam_w=50.0,
    beam_h=200.0,
    cut_offset=2.0,
    beam_offsets=None,
    explode=True,
):
    dome, beams, side0, side1 = reciprocal_rotation_elements(
        nx=u_div,
        ny=v_div,
        W=W,
        D=D,
        h=h,
        mesh_type=mesh_type,
        angle=angle,
        beam_w=beam_w,
        beam_h=beam_h,
        cut_offset_factor=cut_offset,
        beam_offsets=beam_offsets,
        unweld_beams=explode,
    )
    model = PlateModel.from_beams(beams, side0, side1)
    return {"dome": dome, "model": model, "explode": explode}


def draw(scene, results):
    root = add_plate_model(scene, results["model"], name="ReciprocalRotation")
    dome = unweld_mesh(results["dome"]) if results["explode"] else results["dome"]
    add_shell(scene, dome, name="dome", parent=root)
    return root


def main(view=True, **params):
    results = compute(**params)
    print(f"dome: {results['dome'].number_of_faces()} faces  beams: {len(results['model'].plates)}")
    if view:
        from compas_viewer import Viewer

        viewer = Viewer()
        draw(viewer.scene, results)
        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        zoom_to(viewer, aabbs(results["dome"]))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
