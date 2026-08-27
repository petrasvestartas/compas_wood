# Templates

The template generators produce plate geometry: a guide shell mesh plus a list
of plate elements (bottom/top outline pairs) ready for the
[joinery solver](solver.md). They mirror the Rhino plugin's `w_template_*`
commands; every script lives in the repository's `examples/` folder.

Each script follows the same pattern:

- `compute(**params)` builds the geometry and returns a dict of results;
- `draw(scene, results)` adds everything to a scene as one group per plate;
- `main(view=True, **params)` runs both - `view=False` draws into a headless
  [`NullScene`](../api/compas_wood.viewer.md) instead of opening a window.

Run any of them directly, e.g.:

```bash
python examples/templates/translation_shell.py
```

## `examples/hello.py`

The minimal starting point: the default translation shell (the kernel's
built-in arch), drawn as one scene group per plate with the shared
grey-face/blue-outline styling. No parameters beyond `view`.

## `examples/templates/translation_shell.py`

Translation shell between two polylines (plugin command
`w_template_translation_shell`): sweep `cross_section` along `profile` into a
shell of plates. With both curves `None` the kernel's built-in arch is used.
Parameters: `cross_section`, `profile`, `thickness`, `chamfer` (corner cut
distance, 0 = none), `chamfer_angle` (corners sharper than this angle in
degrees get chamfered), `explode` (draw unwelded plate meshes).

## `examples/templates/reflex_fold.py`

Folded shell swept between two polylines (plugin command
`w_template_reflex_fold`): the profile is folded along the cross-section into
alternating plates. Same parameter set as the translation shell -
`cross_section`, `profile`, `thickness`, `chamfer` (miter offset applied to
both faces of each plate), `chamfer_angle`.

## `examples/templates/chevron.py`

Chevron shell (plugin command `w_template_chevron`), from the built-in
sinusoidal dome, a user NURBS surface, or the Annen building surfaces
(`chevron_elements` / `chevron_elements_nurbs` / `chevron_elements_annen`).
Besides the plates it returns per-plate loft meshes and the chevron
`joint_data` (joint types, insertion vectors, three-valence groups and
adjacency) that feeds `PlateModel.from_elements`. Parameters: `u_div`,
`v_division_dist`, `shift`, `scale`, `box_height`, `top_plate_inlet`,
`plate_thickness`, `edge_rotation`, `edge_offset`, and the four
`ortho_edge0..3` flags.

## `examples/templates/diamond_mesh.py`

Diamond-pattern triangular shell (plugin command `w_template_diamond_mesh`),
from the built-in arch surface, an Annen NURBS surface, or a user-supplied
surface. Parameters: `u_div`, `v_div`, `thickness`, `chamfer`,
`chamfer_angle`.

## `examples/templates/connectors.py`

Face plates plus edge connector rectangles from any mesh (plugin command
`w_template_connectors`). `connectors_elements` returns six parallel nested
lists - face/edge polylines, frames and index tags - and
`PlateModel.from_connectors` turns the polyline rows into a lofted plate
model. Parameters: `mesh` (vertices+faces, `None` = built-in VDA mesh),
`face_thickness`, `face_positions`, `edge_divisions`, `edge_division_len`,
`insertion_lines`, `rect_width`, `rect_height`, `rect_thickness`.

## `examples/templates/reciprocal_move.py`

Translation-based reciprocal frame, a nexorade (plugin command
`w_template_reciprocal_move`): beams on a sinusoidal dome, or on a NURBS
surface / mesh via the `_from_surface` / `_from_mesh` variants. Parameters:
`nx`, `ny` (grid divisions), `W`, `D`, `h` (dome width/depth/height),
`mesh_type` (`"quad"` etc.), `angle` (translation of beam ends), `beam_w`,
`beam_h`, `extend_factor`, `cut_offset_factor`, `beam_offsets`,
`unweld_beams`.

## `examples/templates/reciprocal_rotation.py`

Rotation-based reciprocal frame (plugin command
`w_template_reciprocal_rotation`): same dome / surface / mesh inputs as the
move variant, but the beams rotate around their midpoints. Parameters as
above, with `angle` (rotation) and the additional `scale` factor.

## Next step

Feed any template's elements to the joinery solver - see the
[solver examples](solver.md) - or convert Breps to plates first with the
[Brep workflow](breps.md).
