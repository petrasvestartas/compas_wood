"""Reciprocal rotation: a rotation-based reciprocal frame on the kernel's dome."""

from compas_wood import PlateModel
from compas_wood import reciprocal_rotation_elements
from compas_wood import unweld_mesh
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_shell

mesh_type = "quad"
u_div = 6
v_div = 6
width = 6000.0
depth = 5000.0
height = 1500.0
angle = 0.2  # how far each beam rotates against its neighbour
beam_w = 50.0
beam_h = 200.0
cut_offset = 2.0
explode = True

dome, beams, side0, side1 = reciprocal_rotation_elements(
    nx=u_div,
    ny=v_div,
    W=width,
    D=depth,
    h=height,
    mesh_type=mesh_type,
    angle=angle,
    beam_w=beam_w,
    beam_h=beam_h,
    cut_offset_factor=cut_offset,
    unweld_beams=explode,
)
model = PlateModel.from_beams(beams, side0, side1)
print(f"dome: {dome.number_of_faces()} faces, beams: {len(model.plates)}")

scene = SessionScene("reciprocal_rotation")
root = add_plate_model(scene, model, name="ReciprocalRotation")
add_shell(scene, unweld_mesh(dome) if explode else dome, name="dome", parent=root)

publish(scene, "reciprocal_rotation")
