from wood_nano import closed_mesh_from_polylines as wood_nano_closed_mesh_from_polylines
from wood_nano import point1
from wood_nano import vector1
from wood_nano import int2
from compas_wood.conversions_compas import to_point2
from compas.geometry import Polyline
from compas.datastructures import Mesh


def closed_mesh_from_polylines(input_polyline_pairs):
    """
    Get closed mesh from polylines, where the first two polylines are the top and bottom polylines,
    and the rest are the holes.

    Parameters
    ----------
    input_polyline_pairs : list[list[compas.geometry.Polyline]]
        List of polylines that represents the timber elements.

    Returns
    -------
    compas.datastructures.Mesh
        closed mesh
    """

    vector_of_vector_of_points = to_point2(input_polyline_pairs)
    vertices = point1()
    normals = vector1()
    faces = int2()

    wood_nano_closed_mesh_from_polylines(vector_of_vector_of_points, vertices, normals, faces)

    return Mesh.from_vertices_and_faces(vertices, faces)


if __name__ == "__main__":

    polyline0 = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [0, 0, 0]])
    polyline1 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
    polyline2 = Polyline([[0.25, 0.25, 0], [0.75, 0.25, 0], [0.75, 0.75, 0], [0.25, 0.75, 0], [0.25, 0.25, 0]])
    polyline3 = Polyline([[0.25, 0.25, 1], [0.75, 0.25, 1], [0.75, 0.75, 1], [0.25, 0.75, 1], [0.25, 0.25, 1]])
    polylines = [polyline0, polyline1, polyline2, polyline3]

    mesh = closed_mesh_from_polylines(polylines)

    import sys

    if sys.version_info >= (3, 9):
        from compas_viewer import Viewer

        viewer = Viewer()

        viewer.add(mesh, show_faces=True, show_edges=True, show_points=False)
        viewer.show()
