import compas
import pytest
from compas.geometry import Polyline

from compas_wood import Plate
from compas_wood import PlateModel
from compas_wood import loft


def _square(z=0.0, dx=0.0, s=1.0):
    return Polyline([[dx, 0, z], [dx + s, 0, z], [dx + s, s, z], [dx, s, z], [dx, 0, z]])


def _model(n=3, spacing=2.0):
    model = PlateModel()
    for i in range(n):
        model.add_plate(i, _square(0.0, dx=spacing * i), _square(0.1, dx=spacing * i))
    return model


def test_plate_counts_closed_square():
    plate = Plate(0, _square(0.0), _square(0.1))
    assert plate.n_edges == 4
    assert plate.n_faces == 6


def test_plate_counts_open_square():
    bottom = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]])
    top = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1]])
    plate = Plate(0, bottom, top)
    assert plate.n_edges == 4
    assert plate.n_faces == 6


def test_json_roundtrip():
    model = PlateModel(name="rt")
    mesh = loft([_square(0.0)], [_square(0.1)])
    model.add_plate(
        0,
        _square(0.0),
        _square(0.1),
        mesh=mesh,
        holes_bottom=[_square(0.0, dx=0.4, s=0.2)],
        holes_top=[_square(0.1, dx=0.4, s=0.2)],
    )
    model.add_plate(1, _square(0.0, dx=2.0), _square(0.1, dx=2.0))
    jt = [-1, -1, 20, 30, -1, -1]
    iv = [1.0] + [0.0] * 17
    model.tag_plate_joinery(0, jt, iv)
    model.set_global_joinery([[0, 1, 0, 1]], [[0, 1]])

    other = compas.json_loads(compas.json_dumps(model))

    assert isinstance(other, PlateModel)
    assert other.name == "rt"
    assert other.plate_ids() == [0, 1]
    plate = other.plates[0]
    assert plate.joint_types == jt
    assert plate.insertion_vectors == iv
    assert isinstance(plate.bottom, Polyline)
    assert [list(p) for p in plate.bottom.points] == [list(p) for p in model.plates[0].bottom.points]
    assert len(plate.holes_bottom) == 1 and len(plate.holes_top) == 1
    assert plate.mesh is not None
    assert plate.mesh.number_of_faces() == mesh.number_of_faces()
    assert other.three_valence == [[0, 1, 0, 1]]
    assert other.adjacency == [[0, 1]]


def test_solver_kwargs_remaps_plate_ids():
    model = _model(3)
    model.set_global_joinery([], [[0, 2]])
    kwargs = model.solver_kwargs(plate_ids=[0, 2])
    assert len(kwargs["bottom_polylines"]) == 2
    assert kwargs["adjacency"] == [[0, 1]]


def test_three_valence_referencing_dropped_plate_disappears():
    model = _model(3)
    model.set_global_joinery([[0, 1, 2, 1]], [[0, 2]])
    kwargs = model.solver_kwargs(plate_ids=[0, 2])
    assert kwargs["three_valence"] == []


def test_bad_iv_row_dropped_with_warning():
    model = _model(2)
    model.tag_plate_joinery(0, [], [0.1] * 21)
    with pytest.warns(UserWarning, match="21 floats"):
        kwargs = model.solver_kwargs()
    assert kwargs["per_element_insertion_vectors"] == [[], []]
    assert kwargs["per_element_joint_types"] is None


def test_jt_is_none_when_only_iv_set():
    model = _model(2)
    iv = [1.0] + [0.0] * 17
    model.tag_plate_joinery(0, [], iv)
    kwargs = model.solver_kwargs()
    assert kwargs["per_element_joint_types"] is None
    assert kwargs["per_element_insertion_vectors"] == [iv, []]


def test_mismatched_hole_lists_raise():
    with pytest.raises(ValueError):
        Plate(0, _square(0.0), _square(0.1), holes_bottom=[_square(0.0, s=0.2)], holes_top=[])


def test_missing_top_skipped_with_warning():
    model = _model(1)
    model.plates[1] = Plate(1, _square(0.0, dx=2.0), None)
    with pytest.warns(UserWarning, match="missing top"):
        kwargs = model.solver_kwargs()
    assert len(kwargs["bottom_polylines"]) == 1


def test_subset_keeps_original_ids():
    model = _model(3)
    model.set_global_joinery([[0, 1, 2, 1]], [[0, 2]])
    sub = model.subset([2, 0])
    assert sub.plate_ids() == [0, 2]
    assert sub.plates[2] is model.plates[2]
    assert sub.three_valence == [[0, 1, 2, 1]]
    assert sub.adjacency == [[0, 2]]
