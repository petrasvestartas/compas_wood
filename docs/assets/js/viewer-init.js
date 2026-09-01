/* Viewer chrome for the example pages.

   Each example page is a full-bleed session_viewer iframe with the example's
   source behind a corner triangle. The Markdown carries both - one iframe and
   one --8<-- include - so the page is correct with JavaScript off; this script
   only rearranges what is already there.

   It does NOT execute anything. The geometry in the frame was produced by
   running the example for real (tools/build_scenes.py) and serialised to the
   .pb the viewer fetches, so the code on screen and the model beside it come
   from the same run.

   This replaces the Thebe integration, which pointed every reader's browser at
   http://127.0.0.1:8888 - a Jupyter kernel that exists only on the machine that
   ran `invoke play`. On the published site it could never connect, so every
   example page showed a "Server not reachable" cell. Live execution is coming
   back through Pyodide, which needs no server at all. */

(function () {
  "use strict";

  /* The heading belongs ON the model, not above it: the viewer is full-bleed and
     full-height, so a heading in the flow just pushes it off screen. */
  function buildStage() {
    var viewer = document.querySelector(".wood-viewer");
    if (!viewer) return null;

    var heading = document.querySelector(".md-content h1");
    if (heading) {
      heading.classList.add("wood-title");
      viewer.appendChild(heading);
    }

    var toggle = document.createElement("button");
    toggle.type = "button";
    toggle.className = "wood-code-toggle";
    toggle.title = "Show the code (Esc to close)";
    toggle.setAttribute("aria-label", "Show the code");
    viewer.appendChild(toggle);

    var drawer = document.createElement("div");
    drawer.className = "wood-drawer";
    drawer.hidden = true;
    viewer.appendChild(drawer);

    // Own the state rather than reading `drawer.hidden` back at click time -
    // anything else that touches the attribute would otherwise desynchronise
    // the button from the panel.
    var open = false;
    function setOpen(next) {
      open = next;
      drawer.hidden = !open;
      viewer.classList.toggle("wood-viewer--code", open);
      toggle.setAttribute("aria-expanded", String(open));
    }
    setOpen(false);

    toggle.addEventListener("click", function () { setOpen(!open); });
    document.addEventListener("keydown", function (event) {
      if (event.key === "Escape" && open) setOpen(false);
    });

    return drawer;
  }

  /* Move the highlighted listing into the drawer, keeping Material's markup -
     the copy button, line anchors and theming all keep working. */
  function moveCode(marker, drawer) {
    if (!drawer) return;
    var block = marker.nextElementSibling;
    while (block && !block.classList.contains("highlight")) block = block.nextElementSibling;
    if (block) drawer.appendChild(block);
  }

  function init() {
    var marker = document.querySelector(".wood-run");
    if (!marker) return;
    moveCode(marker, buildStage());
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
