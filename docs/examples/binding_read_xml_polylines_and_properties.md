# Read XML Polylines and Properties

Example demonstrating reading polylines and their properties from XML files.

<figure markdown="span">
  ![Read XML Properties](binding_read_xml_polylines_and_properties.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import read_xml_polylines_and_properties
from compas.geometry import Line
import compas_wood
import os

dataset_index = 0
module_path = os.path.dirname(compas_wood.__file__)
foldername = os.path.join(module_path, "datasets")
foldername = os.path.join(module_path, "datasets") + os.sep
files = os.listdir(foldername)

file_names_without_extensions = [os.path.splitext(file)[0] for file in files]
filename = file_names_without_extensions[dataset_index % len(file_names_without_extensions)]
filename = "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small"


(
    polylines,
    vectors_lists,
    input_joints_types,
    input_three_valence_element_indices_and_instruction,
    input_adjacency,
) = read_xml_polylines_and_properties(foldername, filename)
print(input_adjacency)

print("input_three_valence_element_indices_and_instruction:", input_three_valence_element_indices_and_instruction)
print(filename)
```
