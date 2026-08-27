import pytest
from compas.colors import Color
from compas.geometry import Box
from compas.geometry import Frame
from compas.geometry import Polyline

from compas_wood import PlateModel
from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import joinery_solver_elements
from compas_wood import loft
from compas_wood.viewer import NullScene
from compas_wood.viewer import add_joinery
from compas_wood.viewer import add_plate_model
from compas_wood.viewer import add_tags
from compas_wood.viewer import aabbs
from compas_wood.viewer import zoom_to

# Two vertical plates crossing at the origin: one cross joint.
A_BOT = Polyline([[5, 0.5, -2], [-5, 0.5, -2], [-5, 0.5, 2], [5, 0.5, 2], [5, 0.5, -2]])
A_TOP = Polyline([[5, -0.5, -2], [-5, -0.5, -2], [-5, -0.5, 2], [5, -0.5, 2], [5, -0.5, -2]])
B_BOT = Polyline([[0.5, 5, -2], [0.5, -5, -2], [0.5, -5, 2], [0.5, 5, 2], [0.5, 5, -2]])
B_TOP = Polyline([[-0.5, 5, -2], [-0.5, -5, -2], [-0.5, -5, 2], [-0.5, 5, 2], [-0.5, 5, -2]])


@pytest.fixture(scope="module")
def solved():
    elements, joints = joinery_solver_elements([A_BOT, B_BOT], [A_TOP, B_TOP], search_type=SEARCH_CROSS_JOINT)
    assert len(elements) == 2 and len(joints) == 1
    return elements, joints


class _Camera:
    def __init__(self):
        self.position = [-10.0, -10.0, 10.0]
        self.target = [0.0, 0.0, 0.0]
        self.scale = 1.0


class _Viewer:
    def __init__(self):
        self.renderer = type("R", (), {})()
        self.renderer.camera = _Camera()


def test_add_joinery_group_tree(solved):
    elements, joints = solved
    scene = NullScene()
    root = add_joinery(scene, elements, joints)

    groups = [n for n in scene.nodes if n.get("group")]
    geoms = [n for n in scene.nodes if not n.get("group")]
    names = [g["name"] for g in groups]
    assert names[0] == "JoinerySolver"
    assert root is groups[0]
    assert "elem_0" in names and "elem_1" in names and "joint_0" in names
    assert all(n["parent"] is not None for n in geoms)

    mesh_nodes = [n for n in geoms if n["kwargs"].get("name") == "mesh"]
    assert len(mesh_nodes) == len(elements)
    for node in mesh_nodes:
        assert node["kwargs"]["show"] is False  # draw_meshes defaults to hidden
        assert isinstance(node["kwargs"]["facecolor"], Color)
        assert isinstance(node["kwargs"]["linecolor"], Color)
        assert "show_lines" not in node["kwargs"]  # default black edges stay on

    hidden = [n for n in geoms if n["kwargs"].get("name", "").split("_")[0] in ("area", "vol", "line")]
    assert hidden
    assert all(n["kwargs"]["show"] is False for n in hidden)

    outlines = [n for n in geoms if n["kwargs"].get("name", "").split("_")[0] in ("top", "bot")]
    assert outlines
    assert all(isinstance(n["kwargs"]["linecolor"], Color) for n in outlines)


def test_add_joinery_show_flags(solved):
    elements, joints = solved
    scene = NullScene()
    add_joinery(scene, elements, joints, draw_meshes=True, show_areas=True, show_volumes=True, show_lines=True)
    flagged = [n for n in scene.nodes if not n.get("group") and "show" in n["kwargs"]]
    assert flagged
    assert all(n["kwargs"]["show"] is True for n in flagged)


def test_add_plate_model(solved):
    model = PlateModel()
    model.add_plate(0, A_BOT, A_TOP, mesh=loft([A_BOT], [A_TOP]))
    model.add_plate(1, B_BOT, B_TOP)
    scene = NullScene()
    root = add_plate_model(scene, model)

    groups = [n for n in scene.nodes if n.get("group")]
    geoms = [n for n in scene.nodes if not n.get("group")]
    assert [g["name"] for g in groups] == ["Plates", "plate_0", "plate_1"]
    assert root is groups[0]
    assert all(n["parent"] is not None for n in geoms)

    mesh_nodes = [n for n in geoms if n["kwargs"].get("name") == "mesh"]
    assert len(mesh_nodes) == 1
    assert mesh_nodes[0]["kwargs"]["show"] is True
    outline_names = sorted(n["kwargs"]["name"] for n in geoms if n["kwargs"].get("name") != "mesh")
    assert outline_names == ["bot", "bot", "top", "top"]


def test_add_tags_nullscene_warns_and_skips():
    scene = NullScene()
    with pytest.warns(UserWarning, match="NullScene"):
        out = add_tags(scene, [("a", [0, 0, 0])])
    assert out == []


def test_aabbs_skips_none():
    mesh = loft([A_BOT], [A_TOP])
    skew = Polyline([[0, 0, 0], [1, 1, 1], [2, 0, 1]])
    boxes = aabbs(None, mesh, skew, Box(1, 1, 1), None)
    assert len(boxes) == 3
    assert all(isinstance(b, Box) for b in boxes)


def test_aabbs_axis_aligned_planar_polyline():
    # planar outlines have a zero-extent axis; aabbs pads it instead of letting
    # Box.from_bounding_box unitize a zero-length axis (compas 2.15)
    boxes = aabbs(Polyline([[0, 0, 0], [1, 0, 0], [1, 0, 1], [0, 0, 1]]))
    assert len(boxes) == 1
    assert boxes[0].xsize == 1 and boxes[0].zsize == 1


def test_zoom_to_scale_and_target():
    viewer = _Viewer()
    box = Box(2, 4, 6, frame=Frame([10.0, 0.0, 0.0]))
    zoom_to(viewer, [box], tightness=10.0)
    camera = viewer.renderer.camera
    diagonal = (2.0**2 + 4.0**2 + 6.0**2) ** 0.5
    assert camera.scale == pytest.approx(diagonal / 10.0)
    assert camera.target == pytest.approx([10.0, 0.0, 0.0])
    view_len = (3 * 10.0**2) ** 0.5
    expected = [10.0 - 10.0 / view_len * diagonal, -10.0 / view_len * diagonal, 10.0 / view_len * diagonal]
    assert camera.position == pytest.approx(expected)


def test_zoom_to_empty_is_noop():
    viewer = _Viewer()
    zoom_to(viewer, [])
    camera = viewer.renderer.camera
    assert camera.scale == 1.0
    assert camera.target == [0.0, 0.0, 0.0]
    assert camera.position == [-10.0, -10.0, 10.0]
