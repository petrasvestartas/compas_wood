"""Smoke-run every example.

An example is a flat script: importing it builds the geometry, fills a scene and
publishes it. So running the module IS the test - there is no main() to call and
no parameters to pass. ``COMPAS_WOOD_SCENE_DIR`` is redirected to a tmp path so a
test run never writes into the docs tree.
"""

from __future__ import annotations

import importlib.util
import os
import re
from pathlib import Path

import pytest

EXAMPLES_DIR = Path(__file__).resolve().parent.parent / "examples"

# Examples that need the compas_occt Brep backend.
NEEDS_OCCT = {
    "templates/brep_outlines.py",
    "solver/joinery_solver_from_breps.py",
    "solver/contact_detection_tf.py",
    "solver/contact_detection_tf_stress.py",
}

# Examples that read the compas_tf STEP export, which is not in this repo.
NEEDS_STEP = {
    "solver/contact_detection_tf.py",
    "solver/contact_detection_tf_stress.py",
}
STEP_DEFAULT = "data/cantilevers_baked_model.stp"

# compas_viewer is gone: examples draw into a SessionScene and write a .pb for
# session_viewer. Any import of it is a regression.
VIEWER_IMPORT = re.compile(r"(?:from|import)\s+compas_viewer")


def step_file() -> Path | None:
    """Same lookup the two contact-detection examples do, so they skip together."""
    path = Path(os.environ.get("COMPAS_WOOD_STEP") or STEP_DEFAULT)
    return path if path.is_file() else None


def example_files():
    return sorted(p for p in EXAMPLES_DIR.rglob("*.py") if "__pycache__" not in p.parts)


def rel_id(path: Path) -> str:
    return path.relative_to(EXAMPLES_DIR).as_posix()


EXAMPLES = example_files()
IDS = [rel_id(p) for p in EXAMPLES]


def load_module(path: Path):
    name = "example_" + rel_id(path)[:-3].replace("/", "_")
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_examples_found():
    assert len(EXAMPLES) >= 20


@pytest.mark.parametrize("path", EXAMPLES, ids=IDS)
def test_example_runs_and_publishes(path, tmp_path, monkeypatch):
    rel = rel_id(path)
    if rel in NEEDS_OCCT:
        pytest.importorskip("compas_occt")
    if rel in NEEDS_STEP and step_file() is None:
        pytest.skip(f"STEP model not found (set COMPAS_WOOD_STEP, default {STEP_DEFAULT})")

    monkeypatch.setenv("COMPAS_WOOD_SCENE_DIR", str(tmp_path))
    load_module(path)

    # Every example ends in publish(), so a scene and its manifest must exist.
    assert list((tmp_path / "pb").glob("*.pb")), f"{rel}: no scene written"
    assert list((tmp_path / "scenes").glob("*.json")), f"{rel}: no manifest written"


@pytest.mark.parametrize("path", EXAMPLES, ids=IDS)
def test_no_viewer_import(path):
    source = path.read_text(encoding="utf-8")
    assert not VIEWER_IMPORT.search(source), f"{rel_id(path)}: compas_viewer is no longer a dependency"
