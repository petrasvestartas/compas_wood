# Joinery solver

The joinery solver detects contacts between plate elements and produces the
joint-carved outlines (`JoineryElement`) and the joint areas, volumes and cut
lines (`JointResult`). The scripts in `examples/solver/` mirror the Rhino
plugin's `w_solver_*` commands and follow the same
`compute()` / `draw()` / `main(view=True, **params)` pattern as the
[template examples](templates.md).

## Solver parameters

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

## `examples/solver/joinery_solver.py`

The generic solver, a parity port of the plugin command
`w_solver_joinery_solver`. Takes a `PlateModel` (a JSON file path, an
instance, or the built-in translation shell when none is given), runs
`model.solve` with the chosen `search_type` / `joint_params` /
`joint_volume_ext`, times the run, and draws the result as the plugin's
JoinerySolver group tree via `compas_wood.viewer.add_joinery` (plate meshes,
cut outlines, joint areas, volumes and lines as toggleable groups).

## `examples/solver/joinery_solver_translation_shell.py`

Solver on the translation shell template: `SEARCH_FACE_TO_FACE` with finger
joints - family 0 (side-to-side in-plane) overridden to division length 50 and
type 1, the other families at kernel defaults. The minimal example of tuning
`joint_params` for one joint family.

## `examples/solver/joinery_solver_chevron.py`

Solver on the chevron shell: all four generator `joint_data` channels - per
element `insertion_vectors` (18 floats) and `joints_per_face` (6 ints), plus
global `three_valence` groups and `adjacency` pairs - flow through
`PlateModel.from_elements` into `solve`. Demonstrates joinery fully driven by
generator metadata rather than by proximity search alone.

## `examples/solver/joinery_solver_diamond_mesh.py`

Solver on the diamond mesh shell with an explicit 21-float `joint_params`
array and an extended cut volume: `joint_volume_ext=[0, 0, -200]` extends the
joint volume 200 mm along its length axis. Here the side-to-side out-of-plane
family (types 10-19) does the joining.

## `examples/solver/joinery_solver_reciprocal_move.py`

Reciprocal frame (translation) joinery on a hex grid: `SEARCH_CROSS_JOINT` on
the beams' side polylines via `PlateModel.from_beams(beams, side0, side1)`.
The pre-built volumetric beam meshes are displayed instead of the flat
side-face lofts, and `beam_offsets` applies per-direction-group Z shifts
(3 groups on a hex grid) to beams and side polylines together.

## `examples/solver/joinery_solver_reciprocal_rotation.py`

The rotation-based counterpart on a quad grid: same
`PlateModel.from_beams` route, `SEARCH_CROSS_JOINT` on the beam side faces,
volumetric beam meshes for display.

## `examples/solver/joinery_solver_connectivity.py`

Prints the joint-family connectivity table of a solved model: which element
pairs meet, with which joint type and which family (via `FAMILY_LABELS`).
Useful for auditing a model's joinery without opening a viewer.

## `examples/solver/assign_joint_types.py`

Parity port of the plugin command `w_solver_assign_joint_types`. In Rhino,
TextDots are matched to plate edges; here plain `(point, code)` pairs play the
TextDot role and `assign_joint_types(model, dots, snap_radius=...)` writes the
codes into the model. Slot convention: a plate with `n_edges` boundary edges
has a joint-type row of length `n_edges + 2` (slots 0/1 = bottom/top faces);
a dot matching edge `i` writes face slot `i + 2`; `-1` means unset. Family
default codes: 3 = in-plane, 15 = out-of-plane, 20 = top-to-side, 30 = cross,
40 = top-to-top, 58 = rotated, 60 = boundary; in-family variants (e.g. 1-9)
select the joint geometry.

## `examples/solver/assign_insertion_direction.py`

Parity port of the plugin command `w_solver_assign_insertion_direction`.
User-drawn lines become `compas.geometry.Line` objects matched to plate edges
by `assign_insertion_vectors(model, lines, snap_radius=...)`. The stored
vector is `normalize(cross(plate_normal, line_direction))` (zero when the line
parallels the plate normal); a line matches an edge when either endpoint is
within `snap_radius`, and only the best-distance line per (plate, face slot)
is kept.

## `examples/solver/datasets_browser.py`

Browses the JSON datasets shipped inside the wood_nano wheel and solves one:
`list_datasets()` enumerates the names (`--list` on the command line prints
them), `load_dataset(name)` returns
`(bottom_polylines, top_polylines, params)`, and each dataset's own solver
parameters (search type, joint params, insertion vectors, joint types,
adjacency) are passed straight through to `joinery_solver_elements`. The 2.x
XML files (`read_xml_polylines`) are gone - these datasets are their
replacement (see [Migration](../migration.md)).

## Running

```bash
python examples/solver/joinery_solver.py
```

Every script prints element/joint counts and accepts `view=False` for headless
runs via `NullScene`.
