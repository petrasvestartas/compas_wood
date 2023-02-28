import numpy as np
from compas_cgal._cgal import intersections
from compas.plugins import plugin


@plugin(category="intersections")
def intersection_mesh_mesh(A, B):
    """Compute the intersection of tow meshes.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.

    Returns
    -------
    list[NDArray[(Any, 3), np.float64]]
        A list of intersection polylines, with each polyline an array of points.

    Examples
    --------
    >>> from compas.geometry import Box, Sphere, Polyline
    >>> from compas_cgal.intersections import intersection_mesh_mesh

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> sphere = Sphere([1, 1, 1], 0.5)

    >>> A = box.to_vertices_and_faces(triangulated=True)
    >>> B = sphere.to_vertices_and_faces(u=32, v=32, triangulated=True)

    >>> result = intersection_mesh_mesh(A, B)
    >>> polylines = [Polyline(points) for points in result]

    """
    VA, FA = A
    VB, FB = B
    VA = np.asarray(VA, dtype=np.float64)
    FA = np.asarray(FA, dtype=np.int32)
    VB = np.asarray(VB, dtype=np.float64)
    FB = np.asarray(FB, dtype=np.int32)

    pointsets = intersections.intersection_mesh_mesh(VA, FA, VB, FB)

    return pointsets
