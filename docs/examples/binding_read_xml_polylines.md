# Read XML Polylines

Example demonstrating reading polylines from XML files.

<figure markdown="span">
  ![Read XML Polylines](binding_read_xml_polylines.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import read_xml_polylines
import compas_wood
import os

dataset_index = -1
module_path = os.path.dirname(compas_wood.__file__)
foldername = os.path.join(module_path, "datasets")
foldername = os.path.join(module_path, "datasets") + os.sep
files = os.listdir(foldername)

file_names_without_extensions = [os.path.splitext(file)[0] for file in files]
filename = file_names_without_extensions[dataset_index % len(file_names_without_extensions)]

polylines = read_xml_polylines(foldername, filename)
```
