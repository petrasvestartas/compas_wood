from compas.datastructures import Mesh
from compas.geometry import Frame
from compas.geometry import Polyline

from compas_wood import chevron_elements
from compas_wood import connectors_elements
from compas_wood import diamond_mesh_elements
from compas_wood import reciprocal_move_elements
from compas_wood import reflex_fold_elements
from compas_wood import translation_shell_elements
from compas_wood import WoodElement


def _check_shell_output(mesh, elements):
    assert isinstance(mesh, Mesh)
    assert mesh.number_of_faces() > 0
    assert elements
    for el in elements:
        assert isinstance(el, WoodElement)
        assert isinstance(el.bottom, Polyline)
        assert isinstance(el.top, Polyline)
        assert el.thickness > 0.0


def test_translation_shell_default():
    mesh, elements = translation_shell_elements()
    _check_shell_output(mesh, elements)


def test_translation_shell_loft_mesh_unweld():
    _, elements = translation_shell_elements()
    el = elements[0]
    welded = el.loft_mesh()
    unwelded = el.loft_mesh(unwelded=True)
    assert isinstance(welded, Mesh)
    assert isinstance(unwelded, Mesh)
    assert unwelded.number_of_vertices() > welded.number_of_vertices()


def test_chevron_joint_data_shapes():
    mesh, elements, lofts, joint_data = chevron_elements(u_div=2)
    assert isinstance(mesh, Mesh)
    assert elements
    assert all(isinstance(el, WoodElement) for el in elements)
    assert len(lofts) == len(elements)
    assert all(isinstance(m, Mesh) for m in lofts)

    assert sorted(joint_data.keys()) == ["adjacency", "insertion_vectors", "joints_per_face", "three_valence"]
    assert len(joint_data["joints_per_face"]) == len(elements)
    assert len(joint_data["insertion_vectors"]) == len(elements)
    for row in joint_data["joints_per_face"]:
        assert len(row) == 6
        assert all(isinstance(x, int) for x in row)
    for row in joint_data["insertion_vectors"]:
        assert len(row) == 18
        assert all(isinstance(x, float) for x in row)
    assert joint_data["adjacency"]
    assert all(len(pair) == 2 for pair in joint_data["adjacency"])
    assert joint_data["three_valence"]
    assert all(len(group) == 4 for group in joint_data["three_valence"])


def test_diamond_mesh_default():
    mesh, elements = diamond_mesh_elements()
    _check_shell_output(mesh, elements)


def test_reflex_fold_default():
    mesh, elements = reflex_fold_elements()
    _check_shell_output(mesh, elements)


def test_connectors_default():
    out = connectors_elements()
    assert len(out) == 6
    f_polylines, f_frames, f_index, e_polylines, e_frames, e_index = out
    assert f_polylines and e_polylines
    assert len(f_polylines) == len(f_frames) == len(f_index)
    assert len(e_polylines) == len(e_frames) == len(e_index)
    for rows_pl, rows_fr in ((f_polylines, f_frames), (e_polylines, e_frames)):
        for row in rows_pl:
            assert all(isinstance(pl, Polyline) for pl in row)
        for row in rows_fr:
            assert all(fr is None or isinstance(fr, Frame) for fr in row)


def test_reciprocal_move_unweld_beams():
    dome_u, beams_u, side0_u, side1_u = reciprocal_move_elements(nx=4, ny=3, unweld_beams=True)
    dome_w, beams_w, side0_w, side1_w = reciprocal_move_elements(nx=4, ny=3, unweld_beams=False)
    assert isinstance(dome_u, Mesh)
    assert beams_u and len(beams_u) == len(beams_w)
    assert len(side0_u) == len(side1_u) == len(beams_u)
    assert all(isinstance(m, Mesh) for m in beams_u + beams_w)
    assert all(isinstance(pl, Polyline) for pl in side0_u + side1_u)
    verts_u = sum(m.number_of_vertices() for m in beams_u)
    verts_w = sum(m.number_of_vertices() for m in beams_w)
    assert verts_u > verts_w
