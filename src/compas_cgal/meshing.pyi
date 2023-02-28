from typing import List
from typing import Tuple

from compas_cgal.types import VerticesFaces
from compas_cgal.types import VerticesFacesNumpy

def remesh(
    mesh: VerticesFaces,
    target_edge_length: float,
    number_of_iterations: int = 10,
    do_project: bool = True,
) -> VerticesFacesNumpy:
    pass

def remesh_constrained(
    mesh: VerticesFaces,
    target_edge_length: float,
    protected_edges: List[Tuple[int, int]],
    number_of_iterations: int = 10,
    do_project: bool = True,
) -> VerticesFacesNumpy:
    pass
