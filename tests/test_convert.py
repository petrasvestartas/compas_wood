import numpy as np
from compas.datastructures import Mesh
from compas.geometry import Frame
from compas.geometry import Polyline

from compas_wood.convert import frame_from_cpp
from compas_wood.convert import mesh_from_cpp
from compas_wood.convert import polyline_from_cpp

VERTS = [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [1.0, 1.0, 0.0], [0.0, 1.0, 0.0]]


def test_mesh_without_face_tris_keeps_ngons():
    mesh = mesh_from_cpp({"vertices": VERTS, "faces": [[0, 1, 2, 3]]})
    assert isinstance(mesh, Mesh)
    assert mesh.number_of_vertices() == 4
    assert mesh.number_of_faces() == 1
    assert [len(mesh.face_vertices(f)) for f in mesh.faces()] == [4]


def test_mesh_numpy_vertices():
    mesh = mesh_from_cpp({"vertices": np.array(VERTS), "faces": [[0, 1, 2, 3]]})
    assert mesh.number_of_vertices() == 4
    assert mesh.vertex_coordinates(2) == [1.0, 1.0, 0.0]


def test_mesh_with_face_tris_uses_cdt_triangles():
    data = {"vertices": VERTS, "faces": [[0, 1, 2, 3]], "face_tris": [[[0, 1, 2], [0, 2, 3]]]}
    mesh = mesh_from_cpp(data)
    assert mesh.number_of_faces() == 2
    assert all(len(mesh.face_vertices(f)) == 3 for f in mesh.faces())


def test_mesh_face_tris_empty_entry_falls_back_to_ngon():
    data = {"vertices": VERTS, "faces": [[0, 1, 2, 3]], "face_tris": [[]]}
    mesh = mesh_from_cpp(data)
    assert mesh.number_of_faces() == 1
    assert [len(mesh.face_vertices(f)) for f in mesh.faces()] == [4]


def test_mesh_face_tris_shorter_than_faces_mixes_both():
    verts = VERTS + [[2.0, 0.0, 0.0], [2.0, 1.0, 0.0]]
    data = {
        "vertices": verts,
        "faces": [[0, 1, 2, 3], [1, 4, 5, 2]],
        "face_tris": [[[0, 1, 2], [0, 2, 3]]],
    }
    mesh = mesh_from_cpp(data)
    face_sizes = sorted(len(mesh.face_vertices(f)) for f in mesh.faces())
    assert face_sizes == [3, 3, 4]


def test_polyline_from_cpp():
    pl = polyline_from_cpp([[0, 0, 0], [1, 0, 0], [1, 1, 0]])
    assert isinstance(pl, Polyline)
    assert len(pl.points) == 3
    assert list(pl.points[1]) == [1.0, 0.0, 0.0]


def test_frame_from_cpp():
    frame = frame_from_cpp({"origin": [1, 2, 3], "x_axis": [1, 0, 0], "y_axis": [0, 1, 0]})
    assert isinstance(frame, Frame)
    assert list(frame.point) == [1.0, 2.0, 3.0]
    assert list(frame.xaxis) == [1.0, 0.0, 0.0]
    assert list(frame.yaxis) == [0.0, 1.0, 0.0]
