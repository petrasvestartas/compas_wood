# Development environment — compas_wood

COMPAS-friendly wrapper around the `wood_nano` joinery kernel. Pure Python — no
compiler needed, so this is the quick one to set up.

Requires Python **3.13** (pinned in `.python-version`; the package floor is 3.12).
The system Python here is 3.14, so let `uv` supply the right one.

## Setup (run once)

```bash
cd compas_wood
uv venv                          # reads .python-version -> CPython 3.13
uv pip install -e ".[dev,docs]"  # editable install + dev and docs extras
```

Extras, all defined as `requirements-*.txt` in the repo root:

| Extra  | Pulls in | For |
|---|---|---|
| *(none)* | `wood_nano`, `compas` | just using the library |
| `dev`  | pytest, ruff, invoke, build, twine | tests, linting, release |
| `docs` | mkdocs-material, mkdocstrings, mike, … | building the documentation site |
| `brep` | `compas_occt` | the optional OCCT Brep backend (`compas_wood.brep`) |

## Activate

```bash
source .venv/bin/activate
```

Or prefix commands with `uv run` and skip activation.

## Verify

```bash
uv run python -c "import compas_wood, wood_nano, compas; print(compas_wood.__version__)"
uv run pytest      # 101 passed, 5 skipped (the 5 need the optional `brep` extra)
```

## Common tasks

```bash
uv run pytest                    # tests (config lives in pyproject.toml)
uv run ruff check .              # lint
uv run ruff format .             # format
uv run invoke --list             # project tasks from tasks.py
uv run mkdocs serve              # docs at http://127.0.0.1:8000
```

## Using your local wood_nano instead of the PyPI wheel

By default `requirements.txt` pulls `wood_nano >=1.0.29` as a prebuilt wheel from
PyPI. To test against the sibling checkout in `wood_project/wood_nano` — set that
one up first (see its `SETUP.md`, it compiles C++), then point this venv at it:

```bash
uv pip install --no-build-isolation -e ../wood_nano
```

This overwrites the PyPI `wood_nano` in *this* venv. Reverse it with
`uv pip install --force-reinstall wood_nano`.

## Docs: the example pages

Each example page is a full-bleed `session_viewer` frame with the example's source
behind the corner triangle. Build it with:

```bash
invoke scenes     # run every example, write docs/assets/viewer/{pb,scenes}
invoke serve      # mkdocs serve on 127.0.0.1:8001
```

`invoke scenes` is required first: the pages load
`assets/viewer/index.html?scene=scenes/<example>.json`, and those files are
generated, not committed.

The viewer itself (`docs/assets/viewer/index.html` + `.js` + `.wasm`, ~7 MB) **is**
committed — a deploy needs no Rust toolchain and no checkout of the
`petrasvestartas/session` repo it is built from. `docs/assets/viewer/README.md` has
the rebuild recipe and explains why it is built from a lesson snapshot rather than
that repo's `main`.

There are no runnable cells at the moment. The previous ones used Thebe against a
Jupyter server on `127.0.0.1:8888`, which exists only on the machine that started
it — on the published site every example showed "Server not reachable". Live
execution is coming back through Pyodide, which needs no server; it is blocked on
an Emscripten build of the `wood_nano` extension.

## Notes

- `.venv/` self-ignores (uv writes a `.gitignore` containing `*` inside it), so it
  never appears in `git status`.
- Editable install: Python edits under `src/compas_wood/` take effect immediately,
  no reinstall.
