# Migrating from 2.x

compas_wood 3.0 is a clean break: the package is rewritten on the wood_nano
element API and `compas_wood.binding` is gone. The 2.x code remains available
on the `v2` branch and the 2.x tags.

## API mapping

| 2.x (`compas_wood.binding`) | 3.0 |
|-----------------------------|-----|
| `get_connection_zones` | [`joinery_solver_elements`](api/compas_wood.joinery_solver.md) or [`PlateModel.solve`](api/compas_wood.model.md). The input shape differs: separate `bottom_polylines` / `top_polylines` lists instead of one interleaved list, and the result is `JoineryElement` / `JointResult` objects instead of nested polyline lists. |
| `joints` | [`joinery_solver_elements`](api/compas_wood.joinery_solver.md) - the solver returns the detected joints (`JointResult` with `element_ids`, `joint_type`, `area`, `volumes`, `lines`) alongside the carved elements. |
| `wood_globals` / `Globals` | [`JOINT_PARAMS_DEFAULTS`](api/compas_wood.joinery_solver.md) plus the `joint_params` argument of `joinery_solver_elements` / `PlateModel.solve`. There is no mutable global state; pass the 21 floats (7 families x [division distance, shift, type]) per call. |
| `read_xml_polylines`, `read_xml_polylines_and_properties` | [`load_dataset`](api/compas_wood.datasets.md) - the datasets now ship as JSON inside the wood_nano wheel; `list_datasets()` enumerates them. |
| `rtree` | Removed - no kernel support in wood_nano >= 1.0. |
| `beam_volumes` | Removed - no kernel support in wood_nano >= 1.0. |
| `beam_skeleton`, `mesh_skeleton` | Removed - no kernel support in wood_nano >= 1.0. |
| `closed_mesh_from_polylines` | Removed; [`loft`](api/compas_wood.loft.md) covers the bottom+top (with holes) case. |
| `mesh_boolean_difference_from_polylines` | Removed - no kernel support in wood_nano >= 1.0. |

## Runtime

| | 2.x | 3.0 |
|-|-----|-----|
| Python floor | 3.9 | 3.12 |
| Kernel | bundled pybind11 build | `wood_nano >= 1.0` wheel (nanobind) |

## Before / after

```python
# 2.x
from compas_wood.binding import get_connection_zones

polylines = [bot0, top0, bot1, top1, ...]
result = get_connection_zones(polylines, ...)
```

```python
# 3.0
from compas_wood import joinery_solver_elements

elements, joints = joinery_solver_elements(
    bottom_polylines=[bot0, bot1, ...],
    top_polylines=[top0, top1, ...],
)
```

## Rhino / Grasshopper

The 2.x Rhino/Grasshopper tree (`binding_rhino`, the GH components) is gone
from this repository. The Rhino plugin now lives in the
[wood_nano](https://github.com/petrasvestartas/wood_nano) repository and ships
via the Yak package manager - see [Installation](installation.md).
