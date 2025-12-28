# RTree

Example demonstrating RTree spatial indexing.

<figure markdown="span">
  ![RTree](binding_rtree.png){ loading=lazy style="max-height: 400px" }
</figure>

```python
from compas_wood.binding import rtree
from compas.geometry import Polyline, Point


# Example
polyline0 = Polyline([Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0), Point(0, 0, 0)])
polyline1 = Polyline([Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1), Point(0, 0, 1)])

polyline2 = Polyline([Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1), Point(0, 0, 1)])
polyline3 = Polyline([Point(0, 0, 2), Point(1, 0, 2), Point(1, 1, 2), Point(0, 1, 2), Point(0, 0, 2)])
polylines = [polyline0, polyline1, polyline2, polyline3]

neighbors, aabb, obb = rtree(polylines)

print(neighbors)
print(aabb)
print(obb)
```
