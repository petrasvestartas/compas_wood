# Get Connection Zones

Example demonstrating connection zone detection between timber plates.

<figure markdown="span">
  ![Connection Zones](binding_get_connection_zones.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import get_connection_zones
from compas.geometry import Polyline
from compas_wood import data_sets_plates


# joinery parameters
division_length = 300
joint_parameters = [
    division_length, 0.5, 9,
    division_length * 1.5, 0.65, 10,
    division_length * 1.5, 0.5, 21,
    division_length, 0.95, 30,
    division_length, 0.95, 40,
    division_length, 0.95, 50,
]

# generate joints
polylines_lists, output_types, new_polyline_lists = get_connection_zones(
    data_sets_plates.annen_small_polylines(),
    data_sets_plates.annen_small_edge_directions(),
    data_sets_plates.annen_small_edge_joints(),
    data_sets_plates.annen_small_three_valance_element_indices_and_instruction(),
    [],
    joint_parameters,
    0,
    [1, 1, 1],
    4,
)
```
