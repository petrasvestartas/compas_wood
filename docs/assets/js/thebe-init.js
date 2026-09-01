/* Live code cells, backed by a real Jupyter kernel.

   The Markdown carries the source (one --8<-- include), so the page is correct
   with JavaScript off and the code stays version-controlled in the example file.
   This script only upgrades what is already there.

   Three independent pieces:

   1. A triangle in the viewer's corner opens the code over the model, so the
      two are not fighting for the same screen.
   2. Thebe turns the code into a live cell against the kernel, and a watcher
      polls the example's .pb so the viewer swaps geometry whenever the scene
      changes - from a cell, a terminal, anything.

   Edits are kept in localStorage per example, so a refresh keeps what you were
   working on; "reset" puts the file's own version back. */

(function () {
  "use strict";

  var JUPYTER = {
    baseUrl: "http://127.0.0.1:8888",
    wsUrl: "ws://127.0.0.1:8888",
    token: "compas-wood-dev",
  };
  var POLL_MS = 1000;
  var STORE_PREFIX = "compas-wood:code:";

  var pristine = {};      // scene -> the source as shipped in the example file
  var lastChangeAt = {};  // scene -> when its .pb last changed

  function pbUrl(scene) {
    return "../../assets/viewer/pb/" + scene + ".pb";   // pages live at examples/<name>/
  }

  function swapGeometry(scene) {
    var frame = document.querySelector(".wood-viewer > iframe");
    if (!frame || !frame.contentWindow) return;
    frame.contentWindow.postMessage(
      { type: "session-viewer:reload-scene", scene: "scenes/" + scene + ".toml" },
      "*"
    );
  }

  /* HEAD is enough: the filename never changes, so a new solve shows up only as
     a new Last-Modified. */
  function watch(scene) {
    var known = null;
    function poll() {
      if (document.hidden) return;
      fetch(pbUrl(scene), { method: "HEAD", cache: "no-store" })
        .then(function (response) {
          if (!response.ok) return;
          var current = response.headers.get("Last-Modified") + "|" + response.headers.get("ETag");
          if (known === null) {
            known = current;               // first look is the baseline
          } else if (current !== known) {
            known = current;
            lastChangeAt[scene] = Date.now();
            swapGeometry(scene);
          }
        })
        .catch(function () {});
    }
    poll();
    setInterval(poll, POLL_MS);
  }

  /* ---- viewer chrome: scroll shield + code drawer ------------------------ */

  function buildStage(scene) {
    var viewer = document.querySelector(".wood-viewer");
    if (!viewer) return null;

    // The heading belongs ON the model, not above it: the viewer is full-bleed
    // and full-height, so a heading in the flow just pushes it off screen.
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

  /* ---- the cell ---------------------------------------------------------- */

  function prepare(marker, drawer) {
    var block = marker.nextElementSibling;
    while (block && !block.classList.contains("highlight")) block = block.nextElementSibling;
    if (!block) return false;

    var code = block.querySelector("code");
    if (!code) return false;

    // Material wraps each line in a <span> that already ends in a newline, so
    // innerText would double-space the listing; textContent is the characters.
    var scene = marker.getAttribute("data-scene");
    pristine[scene] = code.textContent.replace(/\n$/, "");

    var saved = null;
    try { saved = localStorage.getItem(STORE_PREFIX + scene); } catch (e) { saved = null; }

    var cell = document.createElement("pre");
    cell.setAttribute("data-executable", "true");
    cell.setAttribute("data-language", "python");
    cell.textContent = saved !== null ? saved : pristine[scene];

    block.remove();
    (drawer || marker.parentNode).appendChild(cell);
    return true;
  }

  /* Persist edits, and offer a way back to the file's version. */
  function wireEditor(scene) {
    var host = document.querySelector(".thebe-cell .CodeMirror");
    if (!host || !host.CodeMirror) return;
    var editor = host.CodeMirror;

    var timer = null;
    var restoring = false;   // see reset, below
    editor.on("change", function () {
      if (restoring) return;
      clearTimeout(timer);
      timer = setTimeout(function () {
        try { localStorage.setItem(STORE_PREFIX + scene, editor.getValue()); } catch (e) {}
      }, 400);
    });

    var controls = document.querySelector(".thebe-controls");
    if (!controls || controls.querySelector(".wood-reset")) return;

    var runButton = controls.querySelector(".thebe-run-button");
    if (runButton) runButton.textContent = "Run";
    var reset = document.createElement("button");
    reset.type = "button";
    reset.className = "thebe-button wood-reset";
    reset.textContent = "Reset";
    reset.title = "Discard your edits and restore the example as shipped";
    reset.addEventListener("click", function () {
      // Writing the pristine text is itself a change, and the handler above
      // would save it right back - leaving a stored "override" identical to the
      // file. Suppress the save so reset really does clear the override.
      restoring = true;
      clearTimeout(timer);
      editor.setValue(pristine[scene]);
      try { localStorage.removeItem(STORE_PREFIX + scene); } catch (e) {}
      setTimeout(function () { restoring = false; }, 0);
    });
    controls.appendChild(reset);
  }

  /* Say so when a run cannot possibly show up: the page watches a .pb by URL,
     the kernel writes one by PATH, and if those disagree the run succeeds while
     this page sits there. Invisible from the outside, so it is worth saying. */
  function warnIfSceneNeverChanges(scene) {
    document.addEventListener("click", function (event) {
      if (!event.target.closest(".thebe-run-button, .thebe-runall-button")) return;
      var before = lastChangeAt[scene] || 0;
      setTimeout(function () {
        if ((lastChangeAt[scene] || 0) > before) return;
        var controls = document.querySelector(".thebe-controls");
        if (!controls || document.querySelector(".wood-stale-warning")) return;
        var note = document.createElement("p");
        note.className = "wood-stale-warning";
        note.textContent =
          "The code ran, but this page's scene file never changed - so these docs are not " +
          "served from the tree the kernel writes into. Start them with `invoke play` and " +
          "open http://127.0.0.1:8787 (mkdocs serve builds its own copy and cannot work).";
        controls.parentNode.insertBefore(note, controls.nextSibling);
      }, 25000);
    });
  }

  function init() {
    var markers = Array.prototype.slice.call(document.querySelectorAll(".wood-run"));
    if (!markers.length) return;

    var scene = markers[0].getAttribute("data-scene");
    if (!scene) return;

    watch(scene);
    warnIfSceneNeverChanges(scene);

    var drawer = buildStage(scene);
    if (!prepare(markers[0], drawer)) return;
    if (typeof window.thebe === "undefined") return;

    window.thebe
      .bootstrap({
        requestKernel: true,
        useBinder: false,
        useJupyterLite: false,
        mountStatusWidget: true,
        // ONE cell per page, so most of thebe's controls are noise: "run all" is
        // "run" with extra words, and the two restart buttons manage a kernel the
        // reader never asked to think about. Run (and Reset, added below) is the
        // whole vocabulary. A wedged kernel is handled by reloading the page.
        mountRunButton: true,
        mountRunAllButton: false,
        mountRestartButton: false,
        mountRestartAllButton: false,
        // serverSettings is TOP LEVEL and the kernel key is `kernelName`. Nesting
        // serverSettings under kernelOptions (as several examples online do) is
        // silently ignored: thebe keeps its Binder defaults and then fails with
        // "Server not reachable" against a host you never configured.
        kernelOptions: { kernelName: "python3", path: "/" },
        // Do NOT resume a kernel from an earlier visit. Thebe stores a session
        // id for a day and reconnects to it, so a kernel started before the
        // library was edited keeps serving its stale imports - the example then
        // runs OLD compas_wood and writes a scene that does not match the code
        // on screen. A page load should mean a fresh interpreter.
        savedSessions: { enabled: false },
        serverSettings: {
          baseUrl: JUPYTER.baseUrl,
          wsUrl: JUPYTER.wsUrl,
          token: JUPYTER.token,
          appendToken: true,
        },
      })
      .then(function () { wireEditor(scene); });
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
