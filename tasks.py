import os

from compas_invocations2 import build
from compas_invocations2 import docs as _docs
from compas_invocations2 import style
from compas_invocations2 import tests
from invoke import task
from invoke.collection import Collection


# The docs are mkdocs-material, not Sphinx, so compas_invocations2's docs.docs
# and docs.linkcheck (both sphinx-build) do not apply. Deployment is
# `mkdocs gh-deploy` from .github/workflows/docs.yml.
@task
def docs(ctx, strict=True, serve=True):
    """Build the documentation and serve it locally. Requires requirements-docs.txt."""
    ctx.run(f"mkdocs build{' --strict' if strict else ''}")
    if serve:
        ctx.run("mkdocs serve")


@task
def serve(ctx):
    """Serve the documentation locally with live reload."""
    ctx.run("mkdocs serve")


@task(help={"out": "Where to write the viewer asset tree."})
def scenes(ctx, out="docs/assets/viewer"):
    """Run every example and write its geometry for the embedded viewer.

    One `pb/<example>.pb` plus a `scenes/<example>.json` manifest per example,
    which is what the iframes on the example pages load. Required before
    `docs`/`serve`, and the step CI runs before deploying.
    """
    ctx.run(f"python tools/build_scenes.py {out}", pty=True)


ns = Collection(
    _docs.help,
    style.check,
    style.lint,
    style.format,
    docs,
    serve,
    scenes,
    tests.test,
    tests.testdocs,
    tests.testcodeblocks,
    build.prepare_changelog,
    build.clean,
    build.release,
)
ns.configure(
    {
        "base_folder": os.path.dirname(__file__),
    }
)
