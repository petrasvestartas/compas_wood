"""compas wrappers for C++ WoodElement, JoineryElement, and JointResult."""

from compas.datastructures import Mesh
from compas.geometry import Polyline

from wood_nano import _joinery_solver
from compas_wood.convert import mesh_from_cpp, polyline_from_cpp


class WoodElement:
    """compas adapter for C++ ``wood_session::WoodElement``."""

    def __init__(self, cpp_el):
        self._el = cpp_el

    @property
    def bottom(self) -> Polyline:
        return polyline_from_cpp(self._el.bottom)

    @property
    def top(self) -> Polyline:
        return polyline_from_cpp(self._el.top)

    @property
    def thickness(self) -> float:
        return float(self._el.thickness)

    def loft_mesh(self) -> Mesh:
        # C++ guarantees unified winding + outward normals (unify_winding +
        # orient_outward in the binding), so the previous pure-Python
        # unify_cycles / centroid / flip_cycles pass - three full-mesh Python
        # traversals of geometry math per element - is gone.
        return mesh_from_cpp(self._el.loft_mesh())


class JoineryElement:
    """Merged plate element returned by the joinery solver."""

    def __init__(self, data: dict):
        self.top_outlines: list[Polyline] = [
            polyline_from_cpp(pts) for pts in data["top_outlines"]
        ]
        self.bottom_outlines: list[Polyline] = [
            polyline_from_cpp(pts) for pts in data["bottom_outlines"]
        ]
        # None unless solve_joinery ran with include_loft_mesh=True; kept so
        # an eagerly computed loft is not recomputed on first access.
        self._mesh_data = data.get("loft_mesh")
        self._raw_top = data["top_outlines"]
        self._raw_bottom = data["bottom_outlines"]

    def loft_mesh(self) -> Mesh:
        # Lazy loft in C++ from the raw exported rings (same rings, same
        # top/bottom order as the solver's eager path). The C++ side
        # guarantees unified winding + outward normals, so the previous
        # Python unify_cycles / centroid / flip pass is gone - this method,
        # like the rest of the layer, only converts formats.
        if self._mesh_data is None:
            self._mesh_data = _joinery_solver.loft(self._raw_top, self._raw_bottom)
        return mesh_from_cpp(self._mesh_data)


class JointResult:
    """Detected joint between two plate elements."""

    def __init__(self, data: dict):
        self.element_ids: tuple[int, int] = tuple(data["el_ids"])
        self.joint_type: int = int(data["joint_type"])
        self.area: Polyline = polyline_from_cpp(data["joint_area"])
        self.volumes: list[Polyline] = [polyline_from_cpp(pts) for pts in data["joint_volumes"]]
        self.lines: list[Polyline] = [polyline_from_cpp(pts) for pts in data["joint_lines"]]
