# Joints

Example demonstrating joint detection between polylines.

<figure markdown="span">
  ![Joints](binding_joints.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import joints
from compas.geometry import Polyline, Rotation


polyline0 = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [0, 0, 0]])
polyline1 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
polyline2 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
polyline3 = Polyline([[0, 0, 2], [1, 0, 2], [1, 1, 2], [0, 1, 2], [0, 0, 2]])

xform = Rotation.from_axis_and_angle([0, 0, 1], 0.2)
polyline2.transform(xform)
polyline3.transform(xform)

polylines = [polyline0, polyline1, polyline2, polyline3]

element_pairs, joint_areas, joint_types = joints(polylines)
print(element_pairs)
print(joint_types)
```
