import numpy as np
from compas.plugins import plugin
from compas_cgal._cgal import triangulations


@plugin(category="traingulation", requires=["compas_cgal"])
def delaunay_triangulation(points):
    """Construct a Delaunay triangulation from a set of points.

    Parameters
    ----------
    points : list[:class:`~compas.geometry.Point`]
        Points of the triangulation.

    Returns
    -------
    NDArray[(Any, 3), np.int32]
        The faces of the triangulation.

    Examples
    --------
    >>> from compas.geometry import Pointcloud
    >>> from compas.datastructures import Mesh
    >>> from compas_cgal.triangulation import delaunay_triangulation

    >>> points = Pointcloud.from_bounds(8, 5, 0, 17)
    >>> triangles = delaunay_triangulation(points)

    >>> mesh = Mesh.from_vertices_and_faces(points, triangles)

    """
    vertices = np.asarray(points, dtype=np.float64)
    return triangulations.delaunay_triangulation(vertices)


@plugin(category="triangulation", requires=["compas_cgal"])
def constrained_delaunay_triangulation(boundary, points=None, holes=None, curves=None):
    """Construct a Constrained Delaunay triangulation.

    Parameters
    ----------
    boundary : :class:`~compas.geometry.Polygon`
        The boundary of the triangulation.
    points : list[:class:`~compas.geometry.Point`], optional
        Additional internal points.
    holes : list[:class:`~compas.geometry.Polygon`], optional
        Internal boundary polygons.
    curves : list[:class:`~compas.geometry.Polyline`], optional
        Internal constraint curves.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the triangulation.
    NDArray[(Any, 3), np.int32]
        The faces of the triangulation.

    """
    boundary = np.asarray(boundary, dtype=np.float64)

    points = points or []
    points = np.asarray(points, dtype=np.float64)

    if holes:
        holes = [np.asarray(hole, dtype=np.float64) for hole in holes]
    else:
        holes = []

    if curves:
        curves = [np.asarray(curve, dtype=np.float64) for curve in curves]
    else:
        curves = []

    return triangulations.constrained_delaunay_triangulation(
        boundary, points, holes, curves, is_conforming=False
    )


@plugin(category="triangulation", requires=["compas_cgal"])
def conforming_delaunay_triangulation(boundary, points=None, holes=None, curves=None):
    """Construct a Conforming Delaunay triangulation.

    Parameters
    ----------
    boundary : :class:`~compas.geometry.Polygon`
        The boundary of the triangulation.
    points : list[:class:`~compas.geometry.Point`], optional
        Additional internal points.
    holes : list[:class:`~compas.geometry.Polygon`], optional
        Internal boundary polygons.
    curves : list[:class:`~compas.geometry.Polyline`], optional
        Internal constraint curves.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the triangulation.
    NDArray[(Any, 3), np.int32]
        The faces of the triangulation.

    """
    boundary = np.asarray(boundary, dtype=np.float64)

    points = points or []
    points = np.asarray(points, dtype=np.float64)

    if holes:
        holes = [np.asarray(hole, dtype=np.float64) for hole in holes]
    else:
        holes = []

    if curves:
        curves = [np.asarray(curve, dtype=np.float64) for curve in curves]
    else:
        curves = []

    return triangulations.constrained_delaunay_triangulation(
        boundary, points, holes, curves, is_conforming=True
    )


def refined_delaunay_mesh(
    boundary, points=None, holes=None, curves=None, maxlength=None, is_optimized=False
):
    """Construct a refined Delaunay mesh.

    Parameters
    ----------
    boundary : :class:`~compas.geometry.Polygon`
        The boundary of the triangulation.
    points : list[:class:`~compas.geometry.Point`], optional
        Additional internal points.
    holes : list[:class:`~compas.geometry.Polygon`], optional
        Internal boundary polygons.
    curves : list[:class:`~compas.geometry.Polyline`], optional
        Internal constraint curves.
    maxlength : float, optional
        The maximum length of the triangle edges.
    is_optimized : bool, optional
        Apply LLoyd's optimisation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the triangulation.
    NDArray[(Any, 3), np.int32]
        The faces of the triangulation.

    References
    ----------
    .. [1] https://doc.cgal.org/latest/Mesh_2/index.html#secMesh_2_meshes

    """
    boundary = np.asarray(boundary, dtype=np.float64)

    points = points or []
    points = np.asarray(points, dtype=np.float64)

    if holes:
        holes = [np.asarray(hole, dtype=np.float64) for hole in holes]
    else:
        holes = []

    if curves:
        curves = [np.asarray(curve, dtype=np.float64) for curve in curves]
    else:
        curves = []

    maxlength = maxlength or 0.0

    return triangulations.refined_delaunay_mesh(
        boundary, points, holes, curves, maxlength=maxlength, is_optimized=is_optimized
    )
