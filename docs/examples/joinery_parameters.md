# Solver parameters

`joinery_solver_elements` (and `PlateModel.solve`, which wraps it) take:

- `search_type` - `SEARCH_FACE_TO_FACE` (0), `SEARCH_CROSS_JOINT` (1) or
  `SEARCH_BOTH` (2); `SEARCH_OPTIONS` maps the same ints to strings.
- `joint_params` - 21 floats: 7 joint families x
  `[division distance, shift, type]`. `JOINT_PARAMS_DEFAULTS` mirrors the
  kernel defaults; the families (with their joint-type code ranges) are
  side-to-side in-plane (1-9), side-to-side out-of-plane (10-19), top-to-side
  (20-29), cross (30-39), top-to-top (40-49), side-to-side rotated (50-59)
  and boundary (60-69) - see `FAMILY_LABELS`.
- `joint_volume_ext` - 3 floats extending the joint volume in
  width/height/length (`JOINT_VOLUME_EXT_DEFAULT` is all zero).
- optional per-element `joint_types` / `insertion_vectors` and global
  `three_valence` / `adjacency` metadata - usually carried by a
  [`PlateModel`](../api/compas_wood.model.md).
