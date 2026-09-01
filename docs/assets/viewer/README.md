# The embedded viewer

`index.html` + `session_viewer-*.js` + `session_viewer-*_bg.wasm` are a **committed
build artefact** of the [session_viewer](https://github.com/petrasvestartas/session)
crate. They are in git on purpose: a docs deploy then needs no Rust toolchain and no
checkout of another repository, and the same bytes ship every time.

`pb/` and `scenes/` next to them are *not* committed - `tools/build_scenes.py` writes
one `.pb` and one `.json` manifest per example on every docs build.

## What the pages ask of it

Each example page embeds `index.html?scene=scenes/<example>.json`. Two things about
that were broken upstream and are fixed in this build:

1. **`?scene=` is read.** The crate on `session@main` hard-codes
   `scenes/drawings.json` and ignores the query string, so every embed showed the
   same geometry. `state.rs` here resolves the manifest from the page URL, rejecting
   anything that is not a relative `scenes/<name>.json`.
2. **Manifests are JSON.** The viewer parses them with `serde_json`; nothing on the
   Rust side ever read TOML. `compas_wood.session_scene.publish` writes `.json` to
   match.

## Rebuilding

The viewer on `session@main` is mid-refactor: commit `a96fda48` ("WIP") comments out
the geometry upload in `state.rs`

```rust
// files.push((Gpu::walk_session(&session), place));
```

and `Gpu::walk_session` no longer exists, so a build of `main` renders an empty grey
canvas. Until that refactor lands, the base is the last complete snapshot,
`session_viewer/docs/34h_colors_widths` - which differs from `main` in only four
files, all of them the half-migrated scene path.

`session_viewer.patch` in this directory is the exact diff on top of that base:

```bash
SESSION=~/brg/code_rust/session
cd $SESSION

# Stage a crate: the snapshot's sources, everything else from session_viewer/.
# It must sit beside session_rust/ - Cargo.toml resolves it as ../session_rust.
rm -rf session_viewer_docsbuild && mkdir session_viewer_docsbuild
cp session_viewer/{Cargo.toml,Cargo.lock,Trunk.toml,index.html} session_viewer_docsbuild/
cp -r session_viewer/docs/34h_colors_widths/src session_viewer_docsbuild/src

cd session_viewer_docsbuild
patch -p0 < <compas_wood>/docs/assets/viewer/session_viewer.patch
trunk build --release

cd <compas_wood>
SESSION_VIEWER_DIST=$SESSION/session_viewer_docsbuild/dist invoke scenes
```

`SESSION_VIEWER_DIST` makes `tools/build_scenes.py` copy the new app in and prune the
stale content-hashed bundles Trunk leaves behind; without it the committed app is
reused and only the scenes are rewritten.

## Verifying

`session_py` writes the `.pb` and `session_rust` reads it, so a schema drift between
the two would show up as an empty canvas and nothing else. The current build was
checked by parsing every generated scene with `session_rust::Session::pb_loads` and
`serde_json` against the viewer's own `Manifest` struct - 18 scenes, 13 k to 2.1 M,
all non-empty.
