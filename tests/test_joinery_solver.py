import pytest
from compas.datastructures import Mesh
from compas.geometry import Polyline

from compas_wood import FAMILY_LABELS
from compas_wood import JOINT_PARAMS_DEFAULTS
from compas_wood import JOINT_VOLUME_EXT_DEFAULT
from compas_wood import JoineryElement
from compas_wood import JointResult
from compas_wood import SEARCH_BOTH
from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import SEARCH_FACE_TO_FACE
from compas_wood import SEARCH_OPTIONS
from compas_wood import joinery_solver_elements
from compas_wood import load_dataset
from compas_wood import loft
from compas_wood import translation_shell_elements


@pytest.fixture(scope="module")
def hexbox_solved():
    bottom, top, _ = load_dataset("type_plates_name_hexbox_and_corner")
    return joinery_solver_elements(bottom, top, search_type=SEARCH_FACE_TO_FACE)


def test_constants():
    assert len(JOINT_PARAMS_DEFAULTS) == 21
    assert JOINT_VOLUME_EXT_DEFAULT == [0.0, 0.0, 0.0]
    assert len(FAMILY_LABELS) == 7
    assert SEARCH_OPTIONS[SEARCH_FACE_TO_FACE] == "face_to_face"
    assert SEARCH_OPTIONS[SEARCH_CROSS_JOINT] == "cross_joint"
    assert SEARCH_OPTIONS[SEARCH_BOTH] == "both"


def test_hexbox_solve(hexbox_solved):
    elements, joints = hexbox_solved
    assert len(elements) == 11
    assert len(joints) > 0
    assert all(isinstance(el, JoineryElement) for el in elements)
    assert all(isinstance(j, JointResult) for j in joints)


def test_joint_result_fields(hexbox_solved):
    _, joints = hexbox_solved
    joint = joints[0]
    assert isinstance(joint.element_ids, tuple)
    assert len(joint.element_ids) == 2
    assert all(isinstance(i, int) for i in joint.element_ids)
    assert isinstance(joint.joint_type, int)
    assert isinstance(joint.area, Polyline)
    assert isinstance(joint.volumes, list)
    assert all(isinstance(v, Polyline) for v in joint.volumes)
    assert isinstance(joint.lines, list)
    assert all(isinstance(ln, Polyline) for ln in joint.lines)


def test_joinery_element_outlines(hexbox_solved):
    elements, _ = hexbox_solved
    for el in elements:
        assert el.top_outlines and el.bottom_outlines
        assert all(isinstance(pl, Polyline) for pl in el.top_outlines + el.bottom_outlines)


def test_loft_on_joinery_element(hexbox_solved):
    elements, _ = hexbox_solved
    el = elements[0]
    mesh = loft(el.top_outlines, el.bottom_outlines)
    assert isinstance(mesh, Mesh)
    assert mesh.number_of_faces() > 0
    assert mesh.number_of_vertices() > 0


def test_translation_shell_pipeline():
    _, wood_elements = translation_shell_elements()
    assert len(wood_elements) == 110
    bottoms = [el.bottom for el in wood_elements]
    tops = [el.top for el in wood_elements]
    elements, joints = joinery_solver_elements(bottoms, tops, search_type=SEARCH_FACE_TO_FACE)
    assert len(elements) == 110
    assert len(joints) > 0


def test_mismatched_lengths_raise():
    with pytest.raises(ValueError):
        joinery_solver_elements([Polyline([[0, 0, 0], [1, 0, 0]])], [])
