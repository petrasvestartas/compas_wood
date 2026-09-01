"""Write every example's scene for session_viewer, into one asset tree.

Replaces the old ``view_all_examples`` / ``screenshot_examples`` pair. There is
no desktop viewer to open a window per example any more: each example is run
with ``view=True`` and its :class:`~compas_wood.session_scene.SessionScene` is
serialised, so the output is an asset tree the WASM viewer can fetch::

    <out>/pb/<example>.pb        the geometry
    <out>/scenes/<example>.toml  the manifest, loaded as ?scene=scenes/<example>.toml

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

# The built WASM app (trunk build --release output of the session_viewer crate).
# Not vendored in this repo: it is ~7.7 MB of build artefact from another
# project, so it is copied in at docs-build time from a checkout of
# https://github.com/petrasvestartas/session
DEFAULT_VIEWER_DIST = Path(
    os.environ.get("SESSION_VIEWER_DIST", Path.home() / "code/code_rust/session/session_viewer/dist")
)

# Examples that read the compas_tf STEP export, which is not in this repo.
NEEDS_STEP = {
    "solver/contact_detection_tf.py",
    "solver/contact_detection_tf_stress.py",
}
STEP_RELATIVE = "data/cantilevers_baked_model.stp"


def compas_tf_step():
    """Same lookup the contact-detection examples do, so they skip together."""
    roots = [os.environ.get("COMPAS_TF_DIR"), "C:/brg/compas_tf", Path.home() / "code/code_py/compas_tf"]
    for root in roots:
        if root and (Path(root) / STEP_RELATIVE).is_file():
            return Path(root) / STEP_RELATIVE
    return None


def copy_viewer_app(dist: Path, out_dir: Path) -> bool:
    """Copy the viewer's own files (index.html + js + wasm) next to the scenes.

    Only the app is copied - ``dist/pb`` and ``dist/scenes`` are the session
    project's demo assets, and this tree supplies its own.
    """
    import shutil

    if not dist.is_dir():
        print(f"viewer app not copied: {dist} does not exist (set SESSION_VIEWER_DIST)")
        _write_placeholder(out_dir)
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


def _write_placeholder(out_dir: Path) -> None:
    """Stand in for the viewer app when no build of it was available.

    The docs pages embed the viewer by URL, so without this the iframes would
    just 404 and the reader would be left staring at a blank frame with no idea
    why. The scenes themselves are still written and still downloadable.
    """
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "index.html").write_text(
        "<!DOCTYPE html>\n"
        '<html lang="en"><head><meta charset="utf-8"><title>Viewer not bundled</title>\n'
        "<style>body{font:1rem/1.5 system-ui;margin:0;display:grid;place-items:center;"
        "height:100vh;background:#111;color:#eee;text-align:center}a{color:#6cf}</style>\n"
        "</head><body><div><p><strong>The 3D viewer was not bundled with these docs.</strong></p>\n"
        "<p>The scene is still here - run the viewer locally to see it:<br>\n"
        '<a href="https://github.com/petrasvestartas/session">session_viewer</a></p>\n'
        "</div></body></html>\n",
        encoding="utf-8",
    )
    print(f"wrote a placeholder index.html in {out_dir}")


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
            if rel in NEEDS_STEP and compas_tf_step() is None:
                skipped.append((rel, f"compas_tf checkout not found ({STEP_RELATIVE})"))
                continue
            load_module(path)
            # A flat example publishes on import - loading it is the whole job.
            written.append(rel)
        except Exception as exc:  # one bad example must not lose the rest
            failed.append((rel, f"{type(exc).__name__}: {exc}"))
            traceback.print_exc()

    copy_viewer_app(DEFAULT_VIEWER_DIST, out_dir)

    print(f"\nwrote {len(written)} scene(s) to {out_dir}")
    for rel, why in skipped:
        print(f"  skipped {rel}: {why}")
    for rel, why in failed:
        print(f"  FAILED  {rel}: {why}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_OUT))
