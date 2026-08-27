"""Reciprocal frame (translation) joinery - hex grid, CROSS_JOINT search on beam side faces.

The solver runs on the side0/side1 polylines via :meth:`PlateModel.from_beams`; a
JoineryElement's ``loft_mesh()`` would only loft those two side faces into a flat panel,
so the pre-built volumetric beam meshes are displayed instead. ``beam_offsets`` are
per-direction-group Z shifts (3 groups on a hex grid), applied to beams and side
polylines together.

Measured on this kernel: translation-based frames rest beam-on-beam (top-to-side,
type 20), so the cross-only search reports 0 joints in every tested configuration;
pass ``search_type=SEARCH_BOTH`` to detect the 90 type-20 joints.
"""

from __future__ import annotations

from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import PlateModel
from compas_wood import reciprocal_move_elements


def compute(
    nx=6,
    ny=6,
    mesh_type="hex",
    angle=200.0,
    beam_w=200.0,
    beam_h=400.0,
    beam_offsets=(0.0, 0.0, 0.0),
    search_type=SEARCH_CROSS_JOINT,
):
    _, beams, side0, side1 = reciprocal_move_elements(
        nx=nx,
        ny=ny,
        mesh_type=mesh_type,
        angle=angle,
        beam_w=beam_w,
        beam_h=beam_h,
        beam_offsets=list(beam_offsets) if beam_offsets else None,
    )
    model = PlateModel.from_beams(beams, side0, side1)
    elements, joints = model.solve(search_type=search_type)
    print(f"Reciprocal move: {len(model.plates)} beams in, {len(elements)} elements, {len(joints)} joints.")
    return model, elements, joints


def draw(scene, results):
    model, elements, joints = results
    from compas_wood.viewer import PLATE_FACE
    from compas_wood.viewer import add_joinery

    root = add_joinery(scene, elements, joints, draw_meshes=False, name="ReciprocalMove")
    beams = scene.add_group(name="Beams", parent=root)
    for pid in sorted(model.plates):
        mesh = model.plates[pid].mesh
        if mesh is not None:
            scene.add(mesh, parent=beams, name=f"beam_{pid}", facecolor=PLATE_FACE, show_lines=False)
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
