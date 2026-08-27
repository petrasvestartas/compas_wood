import pytest
from compas.geometry import Line
from compas.geometry import Polyline

from compas_wood import PlateModel
from compas_wood import assign_insertion_vectors
from compas_wood import assign_joint_types
from compas_wood import insertion_vectors_from_lines
from compas_wood import match_points_to_plate_edges

# Unit square, CCW in the XY plane: plate normal (Newell) is +Z.
# Edges: 0 = y=0, 1 = x=1, 2 = y=1, 3 = x=0.


def _square(z=0.0, dx=0.0):
    return Polyline([[dx, 0, z], [dx + 1, 0, z], [dx + 1, 1, z], [dx, 1, z], [dx, 0, z]])


def _model():
    model = PlateModel()
    model.add_plate(0, _square(0.0), _square(0.1))
    model.add_plate(1, _square(0.0, dx=5.0), _square(0.1, dx=5.0))
    return model


def test_match_points_to_plate_edges_raw_indices():
    model = _model()
    bottoms = [model.plates[0].bottom, model.plates[1].bottom]
    matches = match_points_to_plate_edges(bottoms, [[0.5, -0.05, 0.0]], snap_radius=0.2)
    assert matches == [(0, 0, 0)]  # (point_idx, plate_idx, edge_idx) - no slot offset


def test_assign_joint_types_slot_is_edge_plus_two():
    model = _model()
    dots = [
        ([0.5, 0.0, 0.0], 1),
        ([1.0, 0.5, 0.0], 2),
        ([0.5, 1.0, 0.0], 3),
        ([0.0, 0.5, 0.0], 4),
    ]
    changed = assign_joint_types(model, dots, snap_radius=0.05)
    assert set(changed) == {0}
    row = changed[0]
    assert len(row) == model.plates[0].n_faces
    assert row == [-1, -1, 1, 2, 3, 4]
    assert model.plates[0].joint_types == row


def test_assign_joint_types_merges_on_second_call():
    model = _model()
    assign_joint_types(model, [([0.5, 0.0, 0.0], 20)], snap_radius=0.05)
    assert model.plates[0].joint_types == [-1, -1, 20, -1, -1, -1]
    changed = assign_joint_types(model, [([1.0, 0.5, 0.0], 30)], snap_radius=0.05)
    assert changed[0] == [-1, -1, 20, 30, -1, -1]
    assert model.plates[0].joint_types == [-1, -1, 20, 30, -1, -1]


def test_insertion_vectors_from_lines_kernel_semantics():
    model = _model()
    # line starts on edge 0, direction +X: iv = normalize(cross(+Z, +X)) = +Y
    results = insertion_vectors_from_lines([model.plates[0].bottom], [Line([0.5, 0, 0], [1.5, 0, 0])], 0.05)
    assert len(results) == 1
    plate_idx, face_slot, ix, iy, iz = results[0]
    assert (plate_idx, face_slot) == (0, 2)  # slot already offset by +2
    assert (ix, iy, iz) == pytest.approx((0.0, 1.0, 0.0))


def test_insertion_vector_zero_when_line_parallel_to_normal():
    model = _model()
    results = insertion_vectors_from_lines([model.plates[0].bottom], [Line([0.5, 0, 0], [0.5, 0, 1])], 0.05)
    assert len(results) == 1
    assert results[0][2:] == pytest.approx((0.0, 0.0, 0.0))


def test_zero_length_line_warns_and_returns_empty():
    model = _model()
    with pytest.warns(UserWarning, match="zero-length"):
        results = insertion_vectors_from_lines([model.plates[0].bottom], [Line([0.5, 0, 0], [0.5, 0, 0])], 0.05)
    assert results == []


def test_assign_insertion_vectors_row_size_and_merge():
    model = _model()
    changed = assign_insertion_vectors(model, [Line([0.5, 0, 0], [1.5, 0, 0])], snap_radius=0.05)
    row = changed[0]
    assert len(row) == model.plates[0].n_faces * 3
    assert row[6:9] == pytest.approx([0.0, 1.0, 0.0])  # slot 2 = edge 0

    # second call on edge 1 (direction +Y -> cross(+Z, +Y) = -X) merges, not overwrites
    changed2 = assign_insertion_vectors(model, [Line([1.0, 0.5, 0], [1.0, 1.5, 0])], snap_radius=0.05)
    row2 = changed2[0]
    assert row2[6:9] == pytest.approx([0.0, 1.0, 0.0])
    assert row2[9:12] == pytest.approx([-1.0, 0.0, 0.0])  # slot 3 = edge 1
    assert model.plates[0].insertion_vectors == row2


def test_assign_zero_length_only_changes_nothing():
    model = _model()
    with pytest.warns(UserWarning, match="zero-length"):
        changed = assign_insertion_vectors(model, [Line([0.5, 0, 0], [0.5, 0, 0])], snap_radius=0.05)
    assert changed == {}
    assert model.plates[0].insertion_vectors == []
