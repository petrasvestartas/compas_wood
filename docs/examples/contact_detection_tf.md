<div class="wood-viewer">
<iframe src="../../assets/viewer/index.html?scene=scenes/contact_detection_tf.json"
        title="Detected contacts drawn red on the compas_tf floor Breps in session_viewer" loading="lazy"></iframe>
</div>

# Contact detection on the compas_tf floor

!!! note "This example needs a STEP file"

    The floor model is not part of this repository. Point `COMPAS_WOOD_STEP` at your
    own copy of `cantilevers_baked_model.stp`, or drop it in `data/` next to the
    example, then run `invoke scenes`. Without it the example is skipped and this
    page has no geometry to show.

<div class="wood-run" data-scene="contact_detection_tf"></div>

```python
--8<-- "examples/solver/contact_detection_tf.py"
```
