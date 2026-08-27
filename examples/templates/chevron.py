"""Chevron template (port of plugin command w_template_chevron).

surface_idx = -1 uses the kernel's built-in flat surface; 0..22 picks an Annen
NURBS surface. When ``save`` is a path, the elements, loft meshes, and joinery
metadata are stored as a :class:`compas_wood.PlateModel` json.
"""

import compas
from compas.colors import Color

from compas_wood import PlateModel
from compas_wood import chevron_elements
from compas_wood import chevron_elements_annen
from compas_wood import unweld_mesh
from compas_wood.viewer import PLATE_FACE
from compas_wood.viewer import add_shell

OUTLINE = Color(0 / 255, 120 / 255, 220 / 255)


def compute(
    surface_idx=-1,
    u_div=4,
    v_division_dist=900.0,
    box_height=760.0,
    top_plate_inlet=120.0,
    plate_thickness=40.0,
    edge_rotation=1.0,
    edge_offset=0.5,
    ortho_edge0=1,
    ortho_edge1=1,
    ortho_edge2=1,
    ortho_edge3=1,
    explode=False,
    save=None,
):
    kwargs = dict(
        u_div=u_div,
        v_division_dist=v_division_dist,
        box_height=box_height,
        top_plate_inlet=top_plate_inlet,
        plate_thickness=plate_thickness,
        edge_rotation=edge_rotation,
        edge_offset=edge_offset,
        ortho_edge0=ortho_edge0,
        ortho_edge1=ortho_edge1,
        ortho_edge2=ortho_edge2,
        ortho_edge3=ortho_edge3,
    )
    if surface_idx >= 0:
        shell, elements, loft_meshes, joint_data = chevron_elements_annen(surface_idx=surface_idx, **kwargs)
        label = f"[Annen #{surface_idx}]"
    else:
        shell, elements, loft_meshes, joint_data = chevron_elements(**kwargs)
        label = "[default]"
    if save:
        model = PlateModel.from_elements(elements, loft_meshes, joint_data, unweld=explode)
        compas.json_dump(model, str(save))
    return {
        "shell": shell,
        "elements": elements,
        "loft_meshes": loft_meshes,
        "joint_data": joint_data,
        "explode": explode,
        "label": label,
        "save": save,
    }


def draw(scene, results):
    root = scene.add_group(name="Chevron")
    add_shell(scene, results["shell"], parent=root, show=False)
    for i, (el, mesh) in enumerate(zip(results["elements"], results["loft_meshes"])):
        grp = scene.add_group(name=f"plate_{i}", parent=root)
        m = unweld_mesh(mesh) if results["explode"] else mesh
        scene.add(m, parent=grp, name="mesh", facecolor=PLATE_FACE)
        scene.add(el.top, parent=grp, name="top", linecolor=OUTLINE)
        scene.add(el.bottom, parent=grp, name="bot", linecolor=OUTLINE)
    return root


def main(view=True, **params):
    results = compute(**params)
    joint_data = results["joint_data"]
    n_jt = sum(1 for jf in joint_data["joints_per_face"] if any(x > 0 for x in jf)) if joint_data else 0
    n_tv = len(joint_data["three_valence"]) if joint_data else 0
    n_adj = len(joint_data["adjacency"]) if joint_data else 0
    print(
        f"shell: {results['shell'].number_of_faces()} faces  plates: {len(results['elements'])}  "
        f"{results['label']}  joinery: {n_jt} typed, {n_tv} three_valence, {n_adj} adjacency pairs stored"
    )
    if results["save"]:
        print(f"PlateModel saved to {results['save']}")
    if view:
        from compas_viewer import Viewer

        viewer = Viewer()
        draw(viewer.scene, results)
        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        zoom_to(viewer, aabbs(results["shell"], *results["loft_meshes"]))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
