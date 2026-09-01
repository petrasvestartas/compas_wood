"""Contact detection on the whole compas_tf floor, timed.

Same detection as contact_detection_tf.py, but over every solid and with
SEARCH_BOTH, printing where the time goes and which joint types come out.
"""

import os
import time
import warnings
from collections import Counter
from pathlib import Path

from compas.colors import Color
from compas.geometry import Brep
from compas_wood import SEARCH_BOTH
from compas_wood import PlateModel
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import area_mesh


# The one input this example needs is a STEP file of the floor - not a checkout of
# the project it came from. Point COMPAS_WOOD_STEP at your own copy, or drop the
# file in ./data next to this script.
STEP = os.environ.get("COMPAS_WOOD_STEP") or "data/cantilevers_baked_model.stp"

step = str(Path(STEP))
search_type = SEARCH_BOTH
angle_tol_deg = 30.0
area_ratio = 0.25

t0 = time.perf_counter()
brep = Brep.from_step(step)
solids = list(getattr(brep, "solids", None) or [brep])
t_load = time.perf_counter() - t0

t0 = time.perf_counter()
with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    model = PlateModel.from_breps(
        solids,
        skip_invalid=True,
        angle_tol_deg=angle_tol_deg,
        area_ratio=area_ratio,
        min_pair_fraction=0.2,
        pairs="all",
        orientations="both",
        max_pairs=6,
        min_face_area=5000.0,
        slab_faces_min_area=5000.0,
    )
t_extract = time.perf_counter() - t0

t0 = time.perf_counter()
elements, joints = model.solve(search_type=search_type)
contacts = model.contacts_by_source(joints)
t_solve = time.perf_counter() - t0

sources = {plate.name for plate in model.plates.values()}
histogram = Counter(joint.joint_type for joint in contacts.values())
print(f"  load    {len(solids):4d} solids          {t_load:6.2f}s")
print(f"  extract {len(model.plates):4d} search plates from {len(sources)} solids  {t_extract:6.2f}s")
print(f"  solve   {len(contacts):4d} contacts        {t_solve:6.2f}s")
print(f"  types   {dict(sorted(histogram.items()))}")

RED = Color(0.9, 0.1, 0.1)
BLACK = Color(0, 0, 0)

scene = SessionScene("contact_detection_tf_stress")
root = scene.add_group(name="Contacts")
for joint in contacts.values():
    a, b = joint.element_ids
    filled = area_mesh(joint.area)
    if filled is not None:
        scene.add(filled, parent=root, name=f"contact_{a}_{b}", facecolor=RED, show_lines=False)
    if len(joint.area.points) >= 2:
        scene.add(joint.area, parent=root, name=f"outline_{a}_{b}", linecolor=BLACK)

publish(scene, "contact_detection_tf_stress")
