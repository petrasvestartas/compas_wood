# API Reference

All public names are re-exported from the `compas_wood` top-level package;
the pages below document them module by module.

## Template generators

| Module | What it makes |
|--------|---------------|
| [translation_shell](compas_wood.translation_shell.md) | Sweep a cross-section along a profile into a shell of plates |
| [reflex_fold](compas_wood.reflex_fold.md) | Folded shell swept between two polylines |
| [chevron](compas_wood.chevron.md) | Chevron shell (built-in dome, NURBS surface, or the Annen surfaces) |
| [diamond_mesh](compas_wood.diamond_mesh.md) | Diamond-pattern triangular shell |
| [connectors](compas_wood.connectors.md) | Face plates + edge connector rectangles from any mesh |
| [reciprocal_move](compas_wood.reciprocal_move.md) | Translation-based reciprocal frame (nexorade) |
| [reciprocal_rotation](compas_wood.reciprocal_rotation.md) | Rotation-based reciprocal frame |

## Joinery

| Module | What it does |
|--------|--------------|
| [joinery_solver](compas_wood.joinery_solver.md) | Detect joints between plates; search types, joint-parameter defaults |
| [wood_element](compas_wood.wood_element.md) | `WoodElement`, `JoineryElement`, `JointResult` wrappers |
| [model](compas_wood.model.md) | `Plate` / `PlateModel`: in-memory plate topology with json round-trip |
| [assign_vectors](compas_wood.assign_vectors.md) | Tag joint types and insertion vectors from points and lines |

## Geometry and data

| Module | What it does |
|--------|--------------|
| [brep](compas_wood.brep.md) | Brep plates to outline pairs (compas_occt backend) |
| [loft](compas_wood.loft.md) | Loft bottom+top polylines (with holes) into a closed mesh |
| [convert](compas_wood.convert.md) | Raw C++ containers to compas geometry |
| [datasets](compas_wood.datasets.md) | Named JSON datasets shipped with wood_nano |
| [viewer](compas_wood.viewer.md) | compas_viewer drawing helpers mirroring the Rhino plugin's layer tree |
