import pytest

pytest.importorskip("compas_occt")

from compas.geometry import Box
from compas.geometry import Cylinder
from compas.geometry import Frame
from compas.geometry import Sphere
from compas_occt.brep import OCCBrep as Brep

from compas_wood import PlateModel
from compas_wood import SEARCH_CROSS_JOINT
from compas_wood import brep_outlines
from compas_wood import plate_faces
from compas_wood import outline_from_face


def test_plate_faces_of_box():
    brep = Brep.from_box(Box(10, 6, 2, frame=Frame.worldXY()))
    bottom_face, top_face = plate_faces(brep)
    assert bottom_face.area == pytest.approx(60.0)
    assert top_face.area == pytest.approx(60.0)
    for face in (bottom_face, top_face):
        outer, holes = outline_from_face(face)
        assert len(outer.points) == 5
        assert list(outer.points[0]) == pytest.approx(list(outer.points[-1]))
        assert holes == []


def test_brep_outlines_thickness_is_height():
    brep = Brep.from_box(Box(10, 6, 2, frame=Frame.worldXY()))
    bottom, top, holes_bottom, holes_top, thickness = brep_outlines(brep)
    assert thickness == pytest.approx(2.0)
    assert len(bottom.points) == 5 and len(top.points) == 5
    assert holes_bottom == [] and holes_top == []
    # bottom and top rings sit on opposite z-planes of the solid
    z_bottom = {round(p[2], 6) for p in bottom.points}
    z_top = {round(p[2], 6) for p in top.points}
    assert z_bottom != z_top
    assert z_bottom | z_top == {-1.0, 1.0}


def test_box_minus_cylinder_hole_outlines():
    box = Brep.from_box(Box(10, 6, 2, frame=Frame.worldXY()))
    cylinder = Brep.from_cylinder(Cylinder(radius=1.0, height=4.0, frame=Frame.worldXY()))
    holed = box - cylinder
    bottom, top, holes_bottom, holes_top, thickness = brep_outlines(holed)
    assert thickness == pytest.approx(2.0)
    assert len(holes_bottom) == 1
    assert len(holes_top) == 1
    for hole in (holes_bottom[0], holes_top[0]):
        assert len(hole.points) > 4
        assert list(hole.points[0]) == pytest.approx(list(hole.points[-1]))


def test_crossing_box_plates_solve():
    plate_a = Brep.from_box(Box(10, 1, 4, frame=Frame([0, 0, 0])))
    plate_b = Brep.from_box(Box(1, 10, 4, frame=Frame([0, 0, 0])))
    model = PlateModel.from_breps([plate_a, plate_b])
    assert model.plate_ids() == [0, 1]
    elements, joints = model.solve(search_type=SEARCH_CROSS_JOINT)
    assert len(elements) == 2
    assert len(joints) >= 1


def test_sphere_is_not_plate_like():
    sphere = Brep.from_sphere(Sphere(1.0))
    with pytest.raises(ValueError):
        plate_faces(sphere)
