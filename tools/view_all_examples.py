"""Run every example in compas_viewer, one window at a time.

Each example runs in its own subprocess (clean Qt lifecycle); close the viewer
window to advance to the next example. Pass a log path as argv[1] to also
record progress and failures there.
"""

import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PY = ROOT / ".venv" / "Scripts" / "python.exe"

EXAMPLES = (
    [ROOT / "examples" / "hello.py"]
    + sorted((ROOT / "examples" / "templates").glob("*.py"))
    + sorted((ROOT / "examples" / "solver").glob("*.py"))
)


def main() -> None:
    log = open(sys.argv[1], "w", encoding="utf-8") if len(sys.argv) > 1 else sys.stdout
    failed = []
    for i, ex in enumerate(EXAMPLES, 1):
        rel = ex.relative_to(ROOT)
        log.write(f"[{i}/{len(EXAMPLES)}] {rel} - close the viewer window to continue\n")
        log.flush()
        t0 = time.time()
        r = subprocess.run([str(PY), str(ex)], cwd=ROOT, capture_output=True, text=True)
        log.write(f"    exit {r.returncode} in {time.time() - t0:.1f}s\n")
        if r.returncode != 0:
            failed.append(str(rel))
            log.write((r.stderr or "")[-2000:] + "\n")
        log.flush()
    log.write(f"ALL DONE - {len(EXAMPLES) - len(failed)}/{len(EXAMPLES)} ok, failed: {failed or 'none'}\n")
    if log is not sys.stdout:
        log.close()


if __name__ == "__main__":
    main()
