from typing_extensions import Literal

from compas_cgal.types import VerticesFaces
from compas_cgal.types import VerticesFacesNumpy

def _boolean(
    A: VerticesFaces,
    B: VerticesFaces,
    operation: Literal["union", "difference", "intersection", "split"],
) -> VerticesFacesNumpy:
    pass

def boolean_union(A: VerticesFaces, B: VerticesFaces) -> VerticesFacesNumpy:
    pass

def boolean_difference(A: VerticesFaces, B: VerticesFaces) -> VerticesFacesNumpy:
    pass

def boolean_intersection(A: VerticesFaces, B: VerticesFaces) -> VerticesFacesNumpy:
    pass

def split(A: VerticesFaces, B: VerticesFaces) -> VerticesFacesNumpy:
    pass
