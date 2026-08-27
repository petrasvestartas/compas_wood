"""Capture a PNG screenshot of every example's viewer scene.

Each example runs in its own subprocess; the viewer window opens for a moment,
the framebuffer is saved to docs/assets/images/examples/<name>.png, and the
window closes itself. Usage::

    python tools/screenshot_examples.py            # all examples
    python tools/screenshot_examples.py --single examples/hello.py out.png
"""

from __future__ import annotations

import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUT_DIR = ROOT / "docs" / "assets" / "images" / "examples"
CAPTURE_DELAY_MS = 2500


def capture_single(example: str, out_png: str) -> None:
    import importlib.util

    from compas_viewer import Viewer

    original_show = Viewer.show

    def show_and_capture(self):
        from PySide6.QtCore import QTimer

        def _fit_then_grab():
            from compas_viewer.commands import zoom_selected

            zoom_selected(self)
            self.renderer.update()

            def _grab():
                image = self.renderer.grabFramebuffer()
                Path(out_png).parent.mkdir(parents=True, exist_ok=True)
                image.save(out_png, "PNG")
                self.app.quit()

            QTimer.singleShot(500, _grab)

        QTimer.singleShot(CAPTURE_DELAY_MS, _fit_then_grab)
        original_show(self)

    Viewer.show = show_and_capture
    path = ROOT / example
    spec = importlib.util.spec_from_file_location("example_under_capture", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.main(view=True)


def main() -> None:
    examples = sorted(p for p in (ROOT / "examples").rglob("*.py") if "__pycache__" not in p.parts)
    failed = []
    for i, example in enumerate(examples, 1):
        rel = example.relative_to(ROOT).as_posix()
        name = "_".join(example.relative_to(ROOT / "examples").with_suffix("").parts) + ".png"
        out = OUT_DIR / name
        print(f"[{i}/{len(examples)}] {rel} -> {out.name}", flush=True)
        t0 = time.time()
        result = subprocess.run(
            [sys.executable, __file__, "--single", rel, str(out)],
            cwd=ROOT,
            capture_output=True,
            text=True,
            timeout=300,
        )
        ok = result.returncode == 0 and out.exists()
        print(f"    {'ok' if ok else 'FAIL'} in {time.time() - t0:.1f}s", flush=True)
        if not ok:
            failed.append(rel)
            print((result.stderr or "")[-1500:], flush=True)
    print(f"DONE - {len(examples) - len(failed)}/{len(examples)} captured, failed: {failed or 'none'}")


if __name__ == "__main__":
    if len(sys.argv) > 2 and sys.argv[1] == "--single":
        capture_single(sys.argv[2], sys.argv[3])
    else:
        main()
