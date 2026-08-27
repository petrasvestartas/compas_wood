# Breps

wood_nano's contact detection works on **outline polylines**: every plate is a
pair of closed bottom/top loops (plus optional hole loops). CAD models,
however, usually arrive as Breps. The `compas_wood.brep` module (install the
`brep` extra for its [compas_occt](https://github.com/petrasvestartas/compas_occt)
backend) bridges the two worlds: it finds the two large parallel faces of a
plate-like Brep, discretizes their boundary loops into polylines, and pairs
the holes - exactly what a compas_tf-style Brep-native model needs before it
can run wood_nano joinery.

The key functions, documented in the
[API reference](../api/compas_wood.brep.md):

- `plate_faces(brep)` - pick the opposite top/bottom faces of a plate Brep;
- `outline_from_face(face)` - one face to (outer outline, hole outlines),
  with curvature-refined edge discretization;
- `brep_outlines(brep)` - the full plate: bottom/top outlines plus paired
  hole loops;
- `plate_from_brep(brep, plate_id)` - the same, wrapped as a
  [`Plate`](../api/compas_wood.model.md) ready for a `PlateModel`.

## `examples/templates/brep_outlines.py`

The conversion step in isolation. An OCC boolean (box minus cylinder) makes a
plate-like solid with a hole; `brep_outlines` extracts the bottom/top outer
outlines, the paired hole outlines and the plate thickness - the exact
polylines the joinery solver consumes. Bottom/top outlines are drawn blue,
hole outlines red. Use it to check that face picking and discretization behave
before running the solver on a real model.

## `examples/solver/joinery_solver_from_breps.py`

The full Brep-to-joinery pipeline:

1. start from solid plate Breps (built with `Brep.from_box`, or loaded from a
   STEP file via `step=PATH` and `Brep.from_step`);
2. `PlateModel.from_breps(breps)` converts every Brep top/bottom face pair to
   outline polylines via `brep_outlines`;
3. `model.solve(search_type=...)` runs the wood_nano contact detection and
   joinery solver on those outlines;
4. the resulting `JoineryElement` / `JointResult` objects are drawn with
   `compas_wood.viewer.add_joinery`.

Three built-in configs pick the search type and exercise one joint family
each: `"cross"` (two vertical plates crossing in an X - `SEARCH_CROSS_JOINT`,
family 30-39), `"stack"` (two stacked overlapping slabs -
`SEARCH_FACE_TO_FACE`, top-to-top family 40-49) and `"corner"` (an L of two
plates meeting at an edge - `SEARCH_FACE_TO_FACE`, top-to-side family 20-29).

This mirrors what Brep-native models (e.g. compas_tf timber floors) need: the
Breps stay the authoring geometry, while wood_nano only ever sees the outline
pairs derived from their faces.

```python
from compas_wood import PlateModel

model = PlateModel.from_breps(breps)
elements, joints = model.solve(search_type=0)
```

The solver parameters (`search_type`, `joint_params`, `joint_volume_ext`) are
the same as everywhere else - see the [solver examples](solver.md).
