# compas_wood — agent instructions

Pure-Python COMPAS layer over `wood_nano`. Layout, install and examples: `README.md`, `SETUP.md`.

## Core rule

**No computation here.** Every module in `src/compas_wood/` converts COMPAS types
(`Polyline`, `Mesh`, `Frame`) to the `wood_nano` containers, calls the kernel, and converts
back. Geometry, math or search that is missing from the kernel goes into `wood` (C++) and
then `wood_nano` — never into this package as a "temporary" Python fallback.

## Layout

- One module per kernel entry point, mirroring `wood_nano/src/wood_nano/<name>.py`
  (`joinery_solver.py`, `wood_element.py`, `loft.py`, `connectors.py`, ...).
- `convert.py` holds the COMPAS ↔ kernel conversions; `session_scene.py` writes viewer scenes;
  `brep.py` is the optional `compas_occt` backend, never imported at module level.
- Examples under `examples/` are the documentation: `invoke scenes` renders their viewer
  assets into `docs/assets/viewer`, `invoke docs` builds the mkdocs site.

## Working against a local kernel

`requirements.txt` pins `wood_nano >= 1.0.29`; the PyPI wheel may lag. In the superproject,
build `../wood_nano` from source (`uv pip install --no-build-isolation -e .`) into the same
interpreter and re-run `pytest`. A change that needs a new kernel function is three commits,
in order: `wood` → `wood_nano` → here (see `../README.md`; `pushmono` does the whole chain).
