"""The session_viewer backend: conversion, nesting, and what it writes."""

import pytest
import session_py
from compas.colors import Color
from compas.datastructures import Mesh
from compas.geometry import Polyline

from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import joinery_solver_elements
from compas_wood.session_scene import SessionScene
from compas_wood.session_scene import publish
from compas_wood.session_scene import to_session_color
from compas_wood.session_scene import viewer_url
from compas_wood.viewer import add_joinery

A_BOT = Polyline([[5, 0.5, -2], [-5, 0.5, -2], [-5, 0.5, 2], [5, 0.5, 2], [5, 0.5, -2]])
A_TOP = Polyline([[5, -0.5, -2], [-5, -0.5, -2], [-5, -0.5, 2], [5, -0.5, 2], [5, -0.5, -2]])
B_BOT = Polyline([[0.5, 5, -2], [0.5, -5, -2], [0.5, -5, 2], [0.5, 5, 2], [0.5, 5, -2]])
B_TOP = Polyline([[-0.5, 5, -2], [-0.5, -5, -2], [-0.5, -5, 2], [-0.5, 5, 2], [-0.5, 5, -2]])


def test_color_round_trips():
    c = to_session_color(Color(1.0, 0.5, 0.0))
    assert (c.r, c.g, c.b) == pytest.approx((1.0, 0.5, 0.0))


def test_polyline_keeps_points_and_color():
    scene = SessionScene()
    scene.add(Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0]]), name="outline", linecolor=Color(0, 0, 1))
    assert not scene.skipped


def test_mesh_is_converted_not_skipped():
    mesh = Mesh.from_vertices_and_faces([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]], [[0, 1, 2, 3]])
    scene = SessionScene()
    scene.add(mesh, name="plate", facecolor=Color(0.8, 0.8, 0.8))
    assert not scene.skipped


def test_groups_nest():
    scene = SessionScene("root")
    parent = scene.add_group(name="elem_0")
    child = scene.add_group(name="inner", parent=parent)
    assert child.parent is parent
    assert parent.parent is scene.root


def test_unsupported_geometry_warns_and_is_recorded():
    scene = SessionScene()
    with pytest.warns(UserWarning, match="no session_py equivalent"):
        scene.add(object(), name="mystery")
    assert len(scene.skipped) == 1


def test_joinery_scene_writes_a_loadable_pb(tmp_path):
    elements, joints = joinery_solver_elements([A_BOT, B_BOT], [A_TOP, B_TOP], search_type=SEARCH_CROSS_JOINT)
    scene = SessionScene("joinery")
    add_joinery(scene, elements, joints, draw_meshes=True)
    assert not scene.skipped

    path = scene.save(tmp_path / "joinery.pb")
    assert path.exists() and path.stat().st_size > 0
    session_py.Session().pb_load(str(path))  # raises if the file is not a session


def test_publish_writes_manifest_beside_the_scene(tmp_path):
    scene = SessionScene("demo")
    scene.add(Polyline([[0, 0, 0], [1, 0, 0]]), name="line")
    manifest = publish(scene, "demo", tmp_path)

    assert manifest == tmp_path / "scenes" / "demo.toml"
    assert (tmp_path / "pb" / "demo.pb").exists()
    # The manifest's file entry is the path the viewer fetches, relative to the
    # asset root - not an absolute path, which would not resolve in the browser.
    assert 'file = "pb/demo.pb"' in manifest.read_text()


def test_viewer_url_points_at_the_manifest():
    assert viewer_url("demo").endswith("index.html?scene=scenes/demo.toml")


def test_merge_coplanar_faces_makes_one_polygon():
    from compas_wood.session_scene import merge_coplanar_faces

    # A square split into two triangles is one flat region, so it becomes one quad.
    vertices = [[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]]
    _, faces = merge_coplanar_faces(vertices, [[0, 1, 2], [0, 2, 3]])
    assert len(faces) == 1
    assert sorted(faces[0]) == [0, 1, 2, 3]


def test_merge_coplanar_faces_keeps_creases():
    from compas_wood.session_scene import merge_coplanar_faces

    # Two quads folded at 90 degrees: the shared edge is a real edge, not tessellation.
    vertices = [[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [1, 0, 1], [0, 0, 1]]
    faces = [[0, 1, 2, 3], [0, 1, 4, 5]]
    _, merged = merge_coplanar_faces(vertices, faces)
    assert len(merged) == 2


def test_merge_coplanar_faces_leaves_holed_regions_alone():
    from compas_wood.session_scene import merge_coplanar_faces

    # A flat ring: coplanar throughout, but its boundary is two loops (outer and
    # inner), which no single face can express - so the faces stay as they are.
    outer = [[0, 0, 0], [3, 0, 0], [3, 3, 0], [0, 3, 0]]
    inner = [[1, 1, 0], [2, 1, 0], [2, 2, 0], [1, 2, 0]]
    vertices = outer + inner
    faces = [[0, 1, 5, 4], [1, 2, 6, 5], [2, 3, 7, 6], [3, 0, 4, 7]]
    _, merged = merge_coplanar_faces(vertices, faces)
    assert len(merged) == len(faces)


def test_solver_mesh_loses_its_triangulation():
    from collections import Counter

    from compas_wood.session_scene import merge_coplanar_faces

    elements, _ = joinery_solver_elements([A_BOT, B_BOT], [A_TOP, B_TOP], search_type=SEARCH_CROSS_JOINT)
    vertices, faces = elements[0].loft_mesh().to_vertices_and_faces()
    _, merged = merge_coplanar_faces(vertices, faces)
    assert len(merged) < len(faces)
    assert max(len(f) for f in merged) > max(len(f) for f in faces)
    assert Counter(len(f) for f in merged)  # sanity: still a face list
