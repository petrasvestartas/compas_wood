"""Reciprocal move template (port of plugin command w_template_reciprocal_move).

Translation-based reciprocal frame on the kernel's sinusoidal dome; plugin
``u_div``/``v_div``/``move`` map to the wrapper's ``nx``/``ny``/``angle``.
"""

from compas_wood import PlateModel
from compas_wood import reciprocal_move_elements
from compas_wood import unweld_mesh
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_shell


def compute(
    mesh_type="quad",
    u_div=6,
    v_div=6,
    move=200.0,
    beam_w=200.0,
    beam_h=400.0,
    beam_offsets=None,
    explode=True,
):
    dome, beams, side0, side1 = reciprocal_move_elements(
        nx=u_div,
        ny=v_div,
        mesh_type=mesh_type,
        angle=move,
        beam_w=beam_w,
        beam_h=beam_h,
        beam_offsets=beam_offsets,
        unweld_beams=explode,
    )
    model = PlateModel.from_beams(beams, side0, side1)
    return {"dome": dome, "model": model, "explode": explode}


def draw(scene, results):
    root = add_plate_model(scene, results["model"], name="ReciprocalMove")
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
