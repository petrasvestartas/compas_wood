"""Smoke-run every example under examples/ headlessly (main(view=False)) with shrunk parameters."""

from __future__ import annotations

import importlib.util
import re
from pathlib import Path

import pytest

EXAMPLES_DIR = Path(__file__).resolve().parent.parent / "examples"

# Per-file kwargs passed to main(view=False, **overrides) to shrink heavy parameters.
OVERRIDES = {
    "templates/chevron.py": {"u_div": 2},
    "templates/diamond_mesh.py": {"u_div": 2, "v_div": 2},
    "templates/reciprocal_move.py": {"u_div": 3, "v_div": 3},
    "templates/reciprocal_rotation.py": {"u_div": 3, "v_div": 3},
    "solver/joinery_solver_chevron.py": {"u_div": 2},
    "solver/joinery_solver_diamond_mesh.py": {"u_div": 2, "v_div": 2},
    "solver/joinery_solver_reciprocal_move.py": {"nx": 3, "ny": 3},
    "solver/joinery_solver_reciprocal_rotation.py": {"nx": 3, "ny": 3},
    "solver/joinery_solver.py": {"plate_ids": list(range(8))},
}

# Examples whose compute path requires the compas_occt Brep backend.
NEEDS_OCCT = {
    "templates/brep_outlines.py",
    "solver/joinery_solver_from_breps.py",
}

VIEWER_IMPORT = re.compile(r"^(?:from|import)\s+compas_viewer", re.MULTILINE)


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
def test_example_runs_headless(path):
    rel = rel_id(path)
    if rel in NEEDS_OCCT:
        pytest.importorskip("compas_occt")
    module = load_module(path)
    result = module.main(view=False, **OVERRIDES.get(rel, {}))
    assert result


@pytest.mark.parametrize("path", EXAMPLES, ids=IDS)
def test_no_module_level_viewer_import(path):
    source = path.read_text(encoding="utf-8")
    assert not VIEWER_IMPORT.search(source), f"{rel_id(path)}: compas_viewer imported at module level"
