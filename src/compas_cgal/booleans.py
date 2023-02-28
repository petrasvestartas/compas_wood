import numpy as np
from compas.plugins import plugin
from compas_cgal._cgal import booleans


def _boolean(A, B, operation):
    """Wrapper for all boolean operations.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.
    operation : Literal['union', 'difference', 'intersection']
        The type of boolean operation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the boolean mesh.
    NDArray[(Any, 3), np.int32]
        The faces of the boolean mesh.

    Raises
    ------
    NotImplementedError
        If the operation type is not supported.

    """
    VA, FA = A
    VB, FB = B
    VA = np.asarray(VA, dtype=np.float64)
    FA = np.asarray(FA, dtype=np.int32)
    VB = np.asarray(VB, dtype=np.float64)
    FB = np.asarray(FB, dtype=np.int32)

    if operation == "union":
        result = booleans.boolean_union(VA, FA, VB, FB)
    elif operation == "difference":
        result = booleans.boolean_difference(VA, FA, VB, FB)
    elif operation == "intersection":
        result = booleans.boolean_intersection(VA, FA, VB, FB)
    elif operation == "split":
        result = booleans.split(VA, FA, VB, FB)
    else:
        raise NotImplementedError

    return result


@plugin(category="booleans", pluggable_name="boolean_union_mesh_mesh")
def boolean_union(A, B):
    """Boolean union of two meshes.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.
    operation : Literal['union', 'difference', 'intersection']
        The type of boolean operation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the boolean mesh.
    NDArray[(Any, 3), np.int32]
        The faces of the boolean mesh.

    Examples
    --------
    >>> from compas.geometry import Box, Sphere, Polyhedron
    >>> from compas_cgal.booleans import boolean_union

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> sphere = Sphere([1, 1, 1], 0.5)

    >>> A = box.to_vertices_and_faces(triangulated=True)
    >>> B = sphere.to_vertices_and_faces(u=32, v=32, triangulated=True)

    >>> C = boolean_union(A, B)
    >>> shape = Polyhedron(*C)

    """
    return _boolean(A, B, "union")


@plugin(category="booleans", pluggable_name="boolean_difference_mesh_mesh")
def boolean_difference(A, B):
    """Boolean difference of two meshes.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.
    operation : Literal['union', 'difference', 'intersection']
        The type of boolean operation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the boolean mesh.
    NDArray[(Any, 3), np.int32]
        The faces of the boolean mesh.

    Examples
    --------
    >>> from compas.geometry import Box, Sphere, Polyhedron
    >>> from compas_cgal.booleans import boolean_difference

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> sphere = Sphere([1, 1, 1], 0.5)

    >>> A = box.to_vertices_and_faces(triangulated=True)
    >>> B = sphere.to_vertices_and_faces(u=32, v=32, triangulated=True)

    >>> C = boolean_difference(A, B)
    >>> shape = Polyhedron(*C)

    """
    return _boolean(A, B, "difference")


@plugin(category="booleans", pluggable_name="boolean_intersection_mesh_mesh")
def boolean_intersection(A, B):
    """Boolean intersection of two meshes.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.
    operation : Literal['union', 'difference', 'intersection']
        The type of boolean operation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the boolean mesh.
    NDArray[(Any, 3), np.int32]
        The faces of the boolean mesh.

    Examples
    --------
    >>> from compas.geometry import Box, Sphere, Polyhedron
    >>> from compas_cgal.booleans import boolean_intersection

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> sphere = Sphere([1, 1, 1], 0.5)

    >>> A = box.to_vertices_and_faces(triangulated=True)
    >>> B = sphere.to_vertices_and_faces(u=32, v=32, triangulated=True)

    >>> C = boolean_intersection(A, B)
    >>> shape = Polyhedron(*C)

    """
    return _boolean(A, B, "intersection")


@plugin(category="booleans", pluggable_name="split_mesh_mesh")
def split(A, B):
    """Split one mesh with another.

    Parameters
    ----------
    A : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh A.
    B : tuple[Sequence[[float, float, float] | :class:`~compas.geometry.Point`], Sequence[[int, int, int]]]
        Mesh B.
    operation : Literal['union', 'difference', 'intersection']
        The type of boolean operation.

    Returns
    -------
    NDArray[(Any, 3), np.float64]
        The vertices of the boolean mesh.
    NDArray[(Any, 3), np.int32]
        The faces of the boolean mesh.

    Examples
    --------
    >>> from compas.geometry import Box, Sphere, Polyhedron
    >>> from compas_cgal.booleans import split

    >>> box = Box.from_width_height_depth(1, 1, 1)
    >>> sphere = Sphere([1, 1, 1], 0.5)

    >>> A = box.to_vertices_and_faces(triangulated=True)
    >>> B = sphere.to_vertices_and_faces(u=32, v=32, triangulated=True)

    >>> V, F = split(A, B)
    >>> mesh = Mesh.from_vertices_and_faces(V, F)

    """
    return _boolean(A, B, "split")
