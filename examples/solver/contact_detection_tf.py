"""Contact detection on the compas_tf timber floor - no joinery generation.

Loads the baked STEP export, keeps the plate-like solids, and runs the
face-to-face search on their outlines. Each detected contact is drawn as a red
filled polygon with a black outline - exactly what compas_tf consumes.
"""

import os
from pathlib import Path

from compas.colors import Color
from compas.geometry import Brep
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import PlateModel
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.viewer import area_mesh


def _compas_tf_file(relative: str) -> str:
    """Locate a data file in a compas_tf checkout.

    The repo lives at a different path on every machine, so honour
    ``COMPAS_TF_DIR`` (repo root) first and fall back to the known checkouts.
    """
    roots = [os.environ.get("COMPAS_TF_DIR"), "C:/brg/compas_tf", Path.home() / "code/code_py/compas_tf"]
    for root in roots:
        if root and (Path(root) / relative).is_file():
            return str(Path(root) / relative)
    return str(Path("C:/brg/compas_tf") / relative)


step = _compas_tf_file("data/cantilevers_baked_model.stp")
search_type = SEARCH_FACE_TO_FACE

brep = Brep.from_step(step)
solids = list(getattr(brep, "solids", None) or [brep])

# Relaxed pair tolerances cover the tapered plates (wedges, t-sections);
# min_pair_fraction rejects curved-dominated solids (screws, dowels);
# pairs="all" + orientations="both" close the kernel's representation and
# orientation sensitivities. Measured: 95% of ground-truth contacts, 0 false
# positives, on this model.
model = PlateModel.from_breps(
    solids,
    skip_invalid=True,
    angle_tol_deg=30.0,
    area_ratio=0.25,
    min_pair_fraction=0.2,
    pairs="all",
    orientations="both",
    max_pairs=6,
    min_face_area=5000.0,
    slab_faces_min_area=5000.0,
)
elements, joints = model.solve(search_type=search_type)
contacts = model.contacts_by_source(joints)
print(f"{len(solids)} solids -> {len(model.plates)} search plates, {len(contacts)} contacts")

RED = Color(0.9, 0.1, 0.1)
BLACK = Color(0, 0, 0)

scene = SessionScene("contact_detection_tf")
root = scene.add_group(name="Contacts")
for joint in contacts.values():
    a, b = joint.element_ids
    filled = area_mesh(joint.area)
    if filled is not None:
        scene.add(filled, parent=root, name=f"contact_{a}_{b}", facecolor=RED, show_lines=False)
    if len(joint.area.points) >= 2:
        scene.add(joint.area, parent=root, name=f"outline_{a}_{b}", linecolor=BLACK)

publish(scene, "contact_detection_tf")
