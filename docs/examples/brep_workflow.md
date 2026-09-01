# Brep workflow

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

The examples below go from Breps to detected contacts: outline extraction,
joinery on Brep input, and contact detection on a full timber-floor model.

## Comparing against ground truth

`tools/compare_contacts_tf.py` audits the Brep detection against the design
outline pairs exported from the compas_tf model: it draws the untouched Breps
with the **detected** contacts as filled red patches and the **missing** pairs
(in ground truth but not detected from the Breps) as filled yellow patches,
each with a yellow line connecting the two solids' centers. For every missing
pair it prints the carved gap between the solids' bounding boxes, separating
design-only adjacencies (the carved solids do not touch) from real misses.
