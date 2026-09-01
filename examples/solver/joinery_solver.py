"""The generic joinery solver, on the built-in translation shell."""

from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood import translation_shell_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

# Which plates to solve: None for all of them, or a list of ids.
plate_ids = None
draw_meshes = True

_, plates = translation_shell_elements()
model = PlateModel.from_elements(plates)
elements, joints = model.solve(search_type=SEARCH_FACE_TO_FACE, plate_ids=plate_ids)

scene = SessionScene("joinery_solver")
add_joinery(scene, elements, joints, draw_meshes=draw_meshes, name="JoinerySolver")
publish(scene, "joinery_solver")
