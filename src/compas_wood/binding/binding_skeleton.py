from wood_nano import beam_skeleton as wood_nano_beam_skeleton
from wood_nano import mesh_skeleton as wood_nano_mesh_skeleton
from compas_wood.conversions_compas import from_point2
from compas_wood.conversions_compas import from_point1
from wood_nano.conversions_python import to_double1
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import from_double1
from wood_nano import double1
from wood_nano import point1
from wood_nano import point2
from compas.datastructures import Mesh
from compas.geometry import Polyline


def beam_skeleton(mesh: Mesh, divisions=10, number_of_neighbours=10, extend_ends=True) -> tuple[Polyline, list[float]]:
    """Get central axis of a mesh whose geometry is closed to a beam.

    Parameters
    ----------
    mesh : :class:`compas.datastructures.Mesh`
        The mesh to get the central axis from.
    divisions : int, optional
        Central axis is subdivded in this number of points.
    number_of_neighbours : int, optional
        The distance value for each polyline point is calculated based on the average distance to this number of neighbours.
    extend_ends : bool, optional
        Whether to extend the ends of the central axis to the mesh using Ray-Mesh Intersection.

    Returns
    -------
    tuple[ :class:`compas.geometry.Polyline`, list[float]]
        The central axis of the beam and the distances of the points to the mesh.
    """

    mesh_copy: Mesh = mesh.copy()
    v, f = mesh_copy.to_vertices_and_faces(triangulated=True)
    input_vertices = to_double1([item for sublist in v for item in sublist])
    input_faces = to_int1([item for sublist in f for item in sublist])

    output_polyline = point1()
    output_distances = double1()
    wood_nano_beam_skeleton(
        input_vertices, input_faces, output_polyline, output_distances, divisions, number_of_neighbours, extend_ends
    )

    polyline = from_point1(output_polyline)
    distances = from_double1(output_distances)

    return polyline, distances


def mesh_skeleton(mesh: Mesh) -> list[Polyline]:
    """Get the skeleton of a mesh.

    Parameters
    ----------
    mesh : :class:`compas.datastructures.Mesh`
        The mesh to get the skeleton from.

    Returns
    -------
    list[ :class:`compas.geometry.Polyline`]
        The skeleton of the mesh.
    """

    mesh_copy: Mesh = mesh.copy()
    v, f = mesh_copy.to_vertices_and_faces(triangulated=True)
    input_vertices = to_double1([item for sublist in v for item in sublist])
    input_faces = to_int1([item for sublist in f for item in sublist])

    output_polylines = point2()
    wood_nano_mesh_skeleton(input_vertices, input_faces, output_polylines)

    polylines = from_point2(output_polylines)

    return polylines