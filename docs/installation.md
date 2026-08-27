# Installation

`compas_wood` requires **Python >= 3.12** and installs from PyPI:

```bash
pip install compas_wood
```

This pulls in [compas](https://compas.dev) and the compiled
[wood_nano](https://github.com/petrasvestartas/wood_nano) kernel wheel.

## Extras

| Extra | Installs | Use it for |
|-------|----------|------------|
| `viewer` | [compas_viewer](https://github.com/compas-dev/compas_viewer) | Drawing the examples with `compas_wood.viewer` |
| `brep` | [compas_occt](https://github.com/petrasvestartas/compas_occt) | The optional `compas_wood.brep` backend (Brep to outline pairs) |
| `dev` | pytest, ruff, invoke, build tooling | Working on compas_wood itself |
| `docs` | mkdocs, mkdocs-material, mkdocstrings | Building this documentation |

```bash
pip install compas_wood[viewer]
pip install compas_wood[viewer,brep]
pip install compas_wood[dev,docs]
```

The library never imports the viewer or the Brep backend at module level, so
the base install stays lean.

## Building wood_nano from source

PyPI ships `wood-nano` as **cp312-abi3** wheels, which may lag behind the
kernel source. When you need the latest kernel API - or a platform without a
wheel - build it from a local checkout with
[uv](https://docs.astral.sh/uv/):

```bash
git clone https://github.com/petrasvestartas/wood_nano
uv venv --python 3.13
uv pip install nanobind scikit-build-core ninja numpy
uv pip install --no-build-isolation <path to wood_nano checkout>
uv pip install compas_wood
```

`--no-build-isolation` reuses the build tools installed in the step before, so
the C++ extension compiles against the environment you actually run. A C++
compiler is required (MSVC on Windows, clang/gcc elsewhere).

## Rhino / Grasshopper

A Rhino plugin also named **compas_wood** ships from the
[wood_nano](https://github.com/petrasvestartas/wood_nano) repository through
the Yak package manager. It bundles its own `wood_nano` install and does
**not** depend on this Python package - install it from Rhino's package
manager, not from PyPI. For the COMPAS world, this page replaces the plugin's
`w_install` command.
