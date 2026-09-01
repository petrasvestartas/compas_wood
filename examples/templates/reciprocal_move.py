"""Reciprocal move: a translation-based reciprocal frame on the kernel's dome."""

from compas_wood import PlateModel
from compas_wood import reciprocal_move_elements
from compas_wood import unweld_mesh
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_shell

mesh_type = "quad"
u_div = 6
v_div = 6
move = 200.0  # how far each beam slides along its neighbour
beam_w = 200.0
beam_h = 400.0
explode = True

dome, beams, side0, side1 = reciprocal_move_elements(
    nx=u_div,
    ny=v_div,
    mesh_type=mesh_type,
    angle=move,
    beam_w=beam_w,
    beam_h=beam_h,
    unweld_beams=explode,
)
model = PlateModel.from_beams(beams, side0, side1)
print(f"dome: {dome.number_of_faces()} faces, beams: {len(model.plates)}")

scene = SessionScene("reciprocal_move")
root = add_plate_model(scene, model, name="ReciprocalMove")
add_shell(scene, unweld_mesh(dome) if explode else dome, name="dome", parent=root)

publish(scene, "reciprocal_move")
