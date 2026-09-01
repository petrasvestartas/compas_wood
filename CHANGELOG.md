# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

* The built `session_viewer` app is committed under `docs/assets/viewer/`, so a docs
  deploy no longer depends on a checkout of the `petrasvestartas/session` repo. Its
  `README.md` carries the rebuild recipe.
* `invoke scenes` (`tools/build_scenes.py`) as the documented docs asset step.

### Changed

* Scene manifests are written as JSON, not TOML. The viewer parses them with
  `serde_json` and never read TOML, so every example page fetched a manifest the
  viewer could not parse and rendered nothing.
* The embedded viewer reads `?scene=` from the page URL. Upstream it hard-codes
  `scenes/drawings.json`, so all 22 example pages asked for their own scene and got
  none of it.
* The contact-detection examples take a STEP file (`COMPAS_WOOD_STEP`, or `data/`)
  instead of hunting for a `compas_tf` checkout at three hard-coded paths.
* The docs workflow also builds on `dev`.

### Removed

* The Thebe live-cell integration (`docs/assets/js/thebe-init.js`, `tools/play.py`,
  `invoke play`, and the `jupyter-server`/`ipykernel` dev requirements). It pointed
  every reader's browser at a Jupyter server on `127.0.0.1:8888`, which exists only
  on the machine that started it; on the published site it could never connect.
  Live execution returns via Pyodide, which needs no server.

## [3.0.0] 2026-08-27

### Added

* Element generators absorbed from `wood_nano_compas`: `translation_shell`,
  `reflex_fold`, `chevron`, `reciprocal_move`, `reciprocal_rotation`,
  `diamond_mesh`, `connectors`, and `joinery_solver`, with the `WoodElement`,
  `JoineryElement`, and `JointResult` wrappers.
* New modules: `model`, `brep`, `viewer`, `assign_vectors`, `datasets`, `loft`.
* Optional extras: `viewer` (compas_viewer), `brep` (compas_occt).

### Changed

* Rewritten on the wood_nano element API (`wood_nano >= 1.0.29`); the package
  is a pure conversion layer between COMPAS types and the C++ kernel.
* Requires Python >= 3.12.
* Packaging moved to pyproject.toml (setuptools), docs to mkdocs-material.

### Removed

* The legacy `compas_wood.binding` API, whose kernel entry points no longer
  exist in wood_nano: `get_connection_zones`, `joints`, `rtree`,
  `beam_volumes`, `beam_skeleton`, `mesh_skeleton`,
  `closed_mesh_from_polylines`, `mesh_boolean_difference_from_polylines`,
  `read_xml_polylines`, `read_xml_polylines_and_properties`, `test`, and
  `wood_globals`/`Globals`.
* The old Rhino/Grasshopper tree (`binding_rhino`, GH components) - the
  Rhino plugin now ships from the wood_nano repository via Yak.
