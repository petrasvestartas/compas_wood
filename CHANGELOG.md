# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

### Changed

### Removed

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
