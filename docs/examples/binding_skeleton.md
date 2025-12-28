# Skeleton

Example demonstrating skeleton extraction.

<figure markdown="span">
  ![Skeleton](binding_skeleton.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import beam_skeleton, mesh_skeleton
from compas.datastructures import Mesh
from pathlib import Path


file_path = Path("data/beam_skeleton_sample_0.ply")
mesh_beam = Mesh.from_ply(file_path)
polyline, distances = beam_skeleton(mesh_beam, divisions=7, number_of_neighbours=10, extend_ends=True)

file_path = Path("data/fork_skeleton_sample_0.ply")
mesh_fork = Mesh.from_ply(file_path)
polylines = mesh_skeleton(mesh_fork)
```
