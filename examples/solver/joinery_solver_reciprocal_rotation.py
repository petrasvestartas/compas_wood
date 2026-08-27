"""Reciprocal frame (rotation) joinery - quad grid, CROSS_JOINT search on beam side faces.

The solver runs on the side0/side1 polylines via :meth:`PlateModel.from_beams`. The
displayed meshes are the solver's CARVED lofts (``JoineryElement.loft_mesh()`` - the
volumetric beam with the cross-joint notches cut in; verified same bounding box as the
stock beam). The uncut stock beams are added hidden under a "Stock" group for comparison.
"""

from __future__ import annotations

from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import PlateModel
from compas_wood import reciprocal_rotation_elements


def compute(
    nx=6,
    ny=6,
    W=6000.0,
    D=5000.0,
    h=1500.0,
    mesh_type="quad",
    angle=0.2,
    beam_w=50.0,
    beam_h=200.0,
    cut_offset_factor=2.0,
    beam_offsets=None,
):
    _, beams, side0, side1 = reciprocal_rotation_elements(
        nx=nx,
        ny=ny,
        W=W,
        D=D,
        h=h,
        mesh_type=mesh_type,
        angle=angle,
        beam_w=beam_w,
        beam_h=beam_h,
        cut_offset_factor=cut_offset_factor,
        beam_offsets=list(beam_offsets) if beam_offsets else None,
    )
    model = PlateModel.from_beams(beams, side0, side1)
    elements, joints = model.solve(search_type=SEARCH_CROSS_JOINT)
    print(f"Reciprocal rotation: {len(model.plates)} beams in, {len(elements)} elements, {len(joints)} joints.")
    return model, elements, joints


def draw(scene, results):
    model, elements, joints = results
    from compas_wood.viewer import add_joinery
    from compas_wood.viewer import add_solid

    root = add_joinery(scene, elements, joints, draw_meshes=True, name="ReciprocalRotation")
    stock = scene.add_group(name="Stock", parent=root)
    for pid in sorted(model.plates):
        mesh = model.plates[pid].mesh
        if mesh is not None:
            add_solid(scene, mesh, parent=stock, name=f"beam_{pid}", show=False)
    return root


def main(view=True, **params):
    results = compute(**params)
    if view:
        from compas_viewer import Viewer

        from compas_wood.viewer import aabbs
        from compas_wood.viewer import zoom_to

        viewer = Viewer()
        draw(viewer.scene, results)
        model, _, _ = results
        zoom_to(viewer, aabbs(*(plate.mesh for plate in model.plates.values())))
        viewer.show()
    else:
        from compas_wood.viewer import NullScene

        draw(NullScene(), results)
    return results


if __name__ == "__main__":
    main()
