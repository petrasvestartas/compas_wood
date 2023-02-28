import numpy as np
from compas_cgal._cgal import subdivision


def catmull_clark(mesh, k=1):
    """Subdivide a mesh withe the Catmull Clark scheme.

    Parameters
    ----------
    mesh : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        The mesh to remesh.
    k : int, optional
        The number of subdivision steps.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the subdivided mesh.
    NDArray[(Any, 4), np.int32]
        The faces of the subdivided mesh.

    Examples
    --------
    >>> from compas.geometry import Box, Polyhedron
    >>> from compas_cgal.subdivision import catmull_clark

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> mesh = box.to_vertices_and_faces()

    >>> result = catmull_clark(mesh, k=3)
    >>> shape = Polyhedron(*result)

    """
    V, F = mesh
    V = np.asarray(V, dtype=np.float64)
    F = np.asarray(F, dtype=np.int32)
    return subdivision.subd_catmullclark(V, F, k)
