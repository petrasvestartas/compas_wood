# compas_wood

COMPAS-friendly interface to the [wood_nano](https://github.com/petrasvestartas/wood_nano)
timber joinery kernel.

The stack has three layers:

1. **wood** - the C++ library that does all the work: plate elements, joinery
   detection, joint volume generation, lofting.
2. **wood_nano** - nanobind bindings of wood, published as a compiled wheel.
3. **compas_wood** (this package) - a pure-Python layer that converts between
   [COMPAS](https://compas.dev) types (`Polyline`, `Mesh`) and the kernel's raw
   containers. No computation happens here.

## Installation

```bash
pip install compas_wood[viewer]
```

PyPI wheels of `wood_nano` may lag behind the kernel API. Until they catch up,
build it from source with [uv](https://docs.astral.sh/uv/):

```bash
git clone https://github.com/petrasvestartas/wood_nano
cd wood_nano
uv pip install .
pip install compas_wood
```

The `viewer` extra installs [compas_viewer](https://github.com/compas-dev/compas_viewer)
for the examples; the library itself never imports it at module level. The
`brep` extra installs `compas_occt` for the optional `compas_wood.brep` backend.

## Quickstart

```python
from compas_wood import translation_shell_elements, joinery_solver_elements

# Sweep a cross-section along a profile into a shell of plate elements
# (no arguments = the built-in arch).
shell_mesh, elements = translation_shell_elements()

# Detect joinery between the plates and get back merged, joint-carved outlines.
plates, joints = joinery_solver_elements(
    bottom_polylines=[e.bottom for e in elements],
    top_polylines=[e.top for e in elements],
)

# Optional: draw the result (pip install compas_wood[viewer]).
from compas_viewer import Viewer

viewer = Viewer()
for plate in plates:
    viewer.scene.add(plate.loft_mesh())
viewer.show()
```

More examples live in the `docs/examples` section of the
[documentation](https://petrasvestartas.github.io/compas_wood).

## Rhino / Grasshopper

A Rhino plugin also named **compas_wood** ships from the
[wood_nano](https://github.com/petrasvestartas/wood_nano) repository via the
Yak package manager. It does not depend on this Python package - install it
from Rhino's package manager, not from PyPI.

## 3.0 is a clean break

compas_wood 3.0 is rewritten on the wood_nano element API and is not
backwards compatible with 2.x (`compas_wood.binding` is gone). See the
[migration page](https://petrasvestartas.github.io/compas_wood/migration/)
in the documentation for the mapping from the old API to the new one. The 2.x
code remains available on the `v2` branch and the 2.x tags.

## License

MIT
