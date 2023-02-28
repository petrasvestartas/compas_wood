from typing import List
from typing import Optional

from compas.geometry import Polygon
from compas.geometry import Polyline

from compas_cgal.types import VerticesFacesNumpy

def constrained_delaunay_triangulation(
    boundary: Polygon,
    holes: Optional[List[Polygon]] = None,
    curves: Optional[List[Polyline]] = None,
    maxlength: Optional[float] = None,
) -> VerticesFacesNumpy: ...
