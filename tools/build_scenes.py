"""Write every example's scene for session_viewer, into one asset tree.

Replaces the old ``view_all_examples`` / ``screenshot_examples`` pair. There is
no desktop viewer to open a window per example any more: importing an example
runs it and ends in ``publish()``, which serialises its
:class:`~compas_wood.session_scene.SessionScene`, so the output is an asset tree
the WASM viewer can fetch::

    <out>/pb/<example>.pb        the geometry
    <out>/scenes/<example>.json  the manifest, loaded as ?scene=scenes/<example>.json

Usage::

    python tools/build_scenes.py [OUT_DIR]

``OUT_DIR`` defaults to ``docs/assets/viewer``, which is where the documentation
embeds the viewer from, so this is also the docs asset step.
"""

from __future__ import annotations

import os
import sys
import traceback
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXAMPLES_DIR = ROOT / "examples"
DEFAULT_OUT = ROOT / "docs" / "assets" / "viewer"

# The built WASM app (index.html + .js + .wasm) is COMMITTED under docs/assets/viewer,
# so the docs build has nothing to fetch and a deploy is reproducible. Rebuilding it is
# a deliberate act - see docs/assets/viewer/README.md for the recipe - and
# SESSION_VIEWER_DIST still points this script at a fresh trunk build when you do.
VIEWER_DIST = os.environ.get("SESSION_VIEWER_DIST")

# Examples that read a STEP model of the compas_tf timber floor. The file is not in
# this repo (it belongs to that project), so these two are skipped unless it is on hand.
NEEDS_STEP = {
    "solver/contact_detection_tf.py",
    "solver/contact_detection_tf_stress.py",
}
STEP_DEFAULT = "data/cantilevers_baked_model.stp"


def step_file():
    """The STEP the contact-detection examples read, or None - same rule they use."""
    path = Path(os.environ.get("COMPAS_WOOD_STEP") or STEP_DEFAULT)
    return path if path.is_file() else None


def copy_viewer_app(dist: Path, out_dir: Path) -> bool:
    """Refresh the committed viewer app from a fresh trunk build.

    Only the app is copied - ``dist/pb`` and ``dist/scenes`` are the session
    project's demo assets, and this tree supplies its own. A no-op unless
    SESSION_VIEWER_DIST is set: the normal build reuses what is committed.
    """
    import shutil

    if not dist.is_dir():
        print(f"viewer app: {dist} does not exist, keeping the committed one")
        return False
    out_dir.mkdir(parents=True, exist_ok=True)

    # Trunk names its bundles by content hash, so a rebuild lands beside the old
    # one instead of replacing it. index.html only ever points at the newest, so
    # the rest are dead weight - 7.7 MB of wasm each, and confusing to find in a
    # directory listing when chasing which build is live.
    fresh = {item.name for item in dist.iterdir() if item.is_file()}
    for old_file in out_dir.glob("session_viewer-*"):
        if old_file.name not in fresh:
            old_file.unlink()

    copied = 0
    for item in dist.iterdir():
        if item.is_file():
            shutil.copy2(item, out_dir / item.name)
            copied += 1
    print(f"viewer app: copied {copied} file(s) from {dist}")
    return True


def load_module(path: Path):
    import importlib.util

    spec = importlib.util.spec_from_file_location(f"_scene_{path.stem}", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main(out_dir: Path = DEFAULT_OUT) -> int:
    # publish() reads this; setting it here keeps the examples themselves free
    # of any docs-specific path.
    os.environ["COMPAS_WOOD_SCENE_DIR"] = str(out_dir)

    paths = [EXAMPLES_DIR / "hello.py"]
    paths += sorted((EXAMPLES_DIR / "templates").glob("*.py"))
    paths += sorted((EXAMPLES_DIR / "solver").glob("*.py"))

    written, skipped, failed = [], [], []
    for path in paths:
        rel = path.relative_to(EXAMPLES_DIR).as_posix()
        try:
            if rel in NEEDS_STEP and step_file() is None:
                skipped.append((rel, f"STEP model not found (set COMPAS_WOOD_STEP, default {STEP_DEFAULT})"))
                continue
            load_module(path)
            # A flat example publishes on import - loading it is the whole job.
            written.append(rel)
        except Exception as exc:  # one bad example must not lose the rest
            failed.append((rel, f"{type(exc).__name__}: {exc}"))
            traceback.print_exc()

    if VIEWER_DIST:
        copy_viewer_app(Path(VIEWER_DIST), out_dir)

    print(f"\nwrote {len(written)} scene(s) to {out_dir}")
    for rel, why in skipped:
        print(f"  skipped {rel}: {why}")
    for rel, why in failed:
        print(f"  FAILED  {rel}: {why}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_OUT))
