<div class="wood-viewer">
<iframe src="../../assets/viewer/index.html?scene=scenes/contact_detection_tf_stress.json"
        title="Stress-test contact detection on the full floor model in session_viewer" loading="lazy"></iframe>
</div>

# Contact detection stress test

!!! note "This example needs a STEP file"

    The floor model is not part of this repository. Point `COMPAS_WOOD_STEP` at your
    own copy of `cantilevers_baked_model.stp`, or drop it in `data/` next to the
    example, then run `invoke scenes`. Without it the example is skipped and this
    page has no geometry to show.

<div class="wood-run" data-scene="contact_detection_tf_stress"></div>

```python
--8<-- "examples/solver/contact_detection_tf_stress.py"
```
