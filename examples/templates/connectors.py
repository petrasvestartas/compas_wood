"""Connectors template (port of plugin command w_template_connectors).

Face plates and edge connectors from the kernel's default VDA mesh, collected
into a :class:`compas_wood.PlateModel`. Face plates blue, edge connectors orange.
"""

from compas.colors import Color

from compas_wood import PlateModel
from compas_wood import connectors_elements

FACE_COLOR = Color(0.3, 0.6, 0.9)
EDGE_COLOR = Color(0.9, 0.5, 0.1)


def compute(
    face_thickness=20.0,
    face_positions=(0.0,),
    edge_divisions=(2,),
    edge_division_len=(),
    insertion_lines=(),
    rect_width=200.0,
    rect_height=300.0,
    rect_thickness=20.0,
):
    f_pl, f_frames, f_index, e_pl, e_frames, e_index = connectors_elements(
        face_thickness=face_thickness,
        face_positions=list(face_positions),
        edge_divisions=list(edge_divisions),
        edge_division_len=list(edge_division_len),
        insertion_lines=list(insertion_lines),
        rect_width=rect_width,
        rect_height=rect_height,
        rect_thickness=rect_thickness,
    )
    model = PlateModel.from_connectors(f_pl, e_pl)
    return {"model": model, "n_faces": len(f_pl)}


def draw(scene, results):
    root = scene.add_group(name="Connectors")
    model = results["model"]
    for pid in sorted(model.plates):
        plate = model.plates[pid]
        color = FACE_COLOR if plate.plate_type == "face" else EDGE_COLOR
        grp = scene.add_group(name=f"{plate.plate_type}_{pid}", parent=root)
        if plate.mesh is not None:
            scene.add(plate.mesh, parent=grp, name="mesh", facecolor=color, show_lines=False)
        scene.add(plate.bottom, parent=grp, name="bot", linecolor=color)
        scene.add(plate.top, parent=grp, name="top", linecolor=color)
    return root


def main(view=True, **params):
    results = compute(**params)
    model = results["model"]
    n_face = sum(1 for p in model.plates.values() if p.plate_type == "face")
    n_edge = sum(1 for p in model.plates.values() if p.plate_type == "edge")
    print(f"faces: {results['n_faces']}  face plates: {n_face}  edge connectors: {n_edge}")
    if view:
        from compas_viewer import Viewer

        viewer = Viewer()
        draw(viewer.scene, results)
        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        zoom_to(viewer, aabbs(*[p.mesh for p in model.plates.values()]))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
