"""Solve one of the JSON datasets shipped with wood_nano.

Each dataset carries plate outlines plus its own solver parameters, which are
passed straight through. `list_datasets()` gives every available name.
"""

from collections import Counter

from compas_wood import joinery_solver_elements
from compas_wood import list_datasets
from compas_wood import load_dataset
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import add_joinery

name = "type_plates_name_hexbox_and_corner"

bottom, top, params = load_dataset(name)
elements, joints = joinery_solver_elements(bottom, top, **params)

types = Counter(joint.joint_type for joint in joints)
print(f"{len(list_datasets())} datasets available; solving {name!r}")
print(f"{len(bottom)} plates -> {len(elements)} elements, {len(joints)} joints, types={dict(sorted(types.items()))}")

scene = SessionScene("datasets_browser")
add_joinery(scene, elements, joints, draw_meshes=True, name=name)
publish(scene, "datasets_browser")
