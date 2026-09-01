"""Serve the built docs with live, runnable code cells - DEVELOPMENT ONLY.

Starts two things and prints one URL:

  * a Jupyter server, which is what actually executes the code the pages send;
  * a static server for ``site/``, so the pages and the scenes are read from the
    same tree the kernel writes into.

The pages use Thebe (https://thebe.readthedocs.io) to turn each example's code
block into a live cell backed by that kernel. Nothing bespoke executes anything:
it is an ordinary Jupyter kernel, so imports, tracebacks and rich output behave
exactly as they do in a notebook or a terminal.

    invoke play

SECURITY: the Jupyter server binds to 127.0.0.1 with a fixed development token
and CORS open to any origin, so any page in your browser could reach it. Do not
run this on a shared machine and do not expose the port.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import threading
from functools import partial
from http.server import SimpleHTTPRequestHandler
from http.server import ThreadingHTTPServer
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SITE = ROOT / "site"

# Fixed so the page can connect without being handed a token at runtime. This is
# the reason the docstring says development only.
DEV_TOKEN = "compas-wood-dev"


class Handler(SimpleHTTPRequestHandler):
    def end_headers(self) -> None:
        # A scene keeps its filename when it is rewritten, so a cached response
        # would show the previous geometry. The page also polls these files to
        # notice a change, which only works if it sees the real Last-Modified.
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def log_message(self, fmt, *args):
        pass


def jupyter_executable() -> str:
    """The `jupyter` that belongs to the interpreter running this script.

    Looking only at PATH finds the wrong one (or none) when the script is
    launched as `.venv/bin/python tools/play.py`, because activating the venv is
    what puts its bin directory on PATH and nobody has to do that to run this.
    """
    local = Path(sys.executable).parent / ("jupyter.exe" if os.name == "nt" else "jupyter")
    if local.exists():
        return str(local)
    found = shutil.which("jupyter")
    if found is None:
        sys.exit("jupyter not found. Install it:  pip install -r requirements-dev.txt")
    return found


def port_is_busy(port: int) -> bool:
    import socket

    with socket.socket() as probe:
        return probe.connect_ex(("127.0.0.1", port)) == 0


def start_jupyter(port: int, scene_dir: Path) -> subprocess.Popen:
    # Refuse rather than add a second server. A leftover kernel from an earlier
    # run holds the library as it was when IT started, so an example would run
    # code that no longer exists on disk and write a scene that disagrees with
    # what the page shows - which looks like a rendering bug, not a stale import.
    if port_is_busy(port):
        sys.exit(
            f"Something is already listening on 127.0.0.1:{port}.\n"
            f"That is probably a Jupyter server from an earlier run, and it holds stale imports.\n"
            f"Stop it first:  pkill -f jupyter-server"
        )


    env = dict(os.environ)
    # publish() writes here, so the kernel and the static server must agree on
    # which tree the pages are reading.
    env["COMPAS_WOOD_SCENE_DIR"] = str(scene_dir)

    return subprocess.Popen(
        [
            jupyter_executable(),
            "server",
            f"--ServerApp.port={port}",
            f"--ServerApp.token={DEV_TOKEN}",
            "--ServerApp.ip=127.0.0.1",
            "--ServerApp.open_browser=False",
            # The pages come from another port, so the kernel websocket is a
            # cross-origin request and XSRF checking would reject it.
            "--ServerApp.allow_origin=*",
            "--ServerApp.disable_check_xsrf=True",
            f"--ServerApp.root_dir={ROOT}",
        ],
        env=env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def link_scenes(site: Path) -> Path:
    """Point the served tree at the SOURCE scenes, and return where to write them.

    `mkdocs build` copies docs/ into site/, so after a build there are two copies
    of every scene. The kernel can only write to one, and whichever server is
    reading the other shows stale geometry with no error - the failure looks
    exactly like "nothing happened". A symlink collapses the two back into one
    file, so a scene written by a cell is the same bytes any server hands out.

    Falls back to the copy inside site/ where symlinks are not available.
    """
    source = ROOT / "docs" / "assets" / "viewer"
    served = site / "assets" / "viewer"
    if not source.is_dir():
        return served

    if served.is_symlink():
        if served.resolve() == source.resolve():
            return source
        served.unlink()
    elif served.exists():
        shutil.rmtree(served)

    try:
        served.parent.mkdir(parents=True, exist_ok=True)
        served.symlink_to(source, target_is_directory=True)
        return source
    except OSError:
        shutil.copytree(source, served, dirs_exist_ok=True)
        return served


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", type=int, default=8787, help="port for the docs")
    parser.add_argument("--jupyter-port", type=int, default=8888)
    parser.add_argument("--site", type=Path, default=SITE)
    args = parser.parse_args()

    if not (args.site / "index.html").exists():
        sys.exit(f"No built docs at {args.site}. Run:  invoke docs --no-serve")

    scene_dir = link_scenes(args.site)
    jupyter = start_jupyter(args.jupyter_port, scene_dir)

    server = ThreadingHTTPServer(("127.0.0.1", args.port), partial(Handler, directory=str(args.site)))
    print(f"docs    http://127.0.0.1:{args.port}/", flush=True)
    print(f"kernel  http://127.0.0.1:{args.jupyter_port}/  (token {DEV_TOKEN})")
    print(f"scenes  {scene_dir}")
    print("DEVELOPMENT ONLY: the kernel accepts any origin. Localhost only.\n")

    threading.Thread(target=server.serve_forever, daemon=True).start()
    try:
        jupyter.wait()
    except KeyboardInterrupt:
        print("\nstopping")
    finally:
        jupyter.terminate()
        server.shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
