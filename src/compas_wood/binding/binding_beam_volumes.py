from wood_nano import beam_volumes as wood_nano_beam_volumes
from compas_wood.conversions_compas import to_point2
from compas_wood.conversions_compas import to_vector2
from compas_wood.conversions_compas import from_point2
from compas_wood.conversions_compas import from_point3
from wood_nano.conversions_python import to_double2
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import from_int1
from wood_nano.conversions_python import from_int2
from wood_nano.conversions_python import from_double2
from wood_nano import int1
from wood_nano import int2
from wood_nano import double2
from wood_nano import point2
from wood_nano import point3
from wood_nano import cut_type2

# from wood_nano.conversions_python import from_cut_type2


def beam_volumes(
    input_polyline_axes,
    input_polylines_segment_radii,
    input_polylines_segment_direction,
    input_allowed_types_per_polyline=[1],
    input_min_distance=50,
    input_volume_length=500,
    input_cross_or_side_to_end=0.91,
    input_flip_male=1,
    input_compute_joints=False,
    input_division_distance=150,
    input_shift=0.5,
    input_output_geometry_type=0,
    input_use_eccentricities_to_scale_joints=True,
):

    index_polylines = int2()
    index_polylines_segment = int2()
    distance = double2()
    point_pairs = point2()
    volume_pairs = point3()
    joints_areas = point2()
    joints_types = int1()
    w_output_plines = point3()
    w_output_types = cut_type2()

    wood_nano_beam_volumes(
        to_point2(input_polyline_axes),
        to_double2(input_polylines_segment_radii),
        to_vector2(input_polylines_segment_direction),
        to_int1(input_allowed_types_per_polyline),
        float(input_min_distance),
        float(input_volume_length),
        float(input_cross_or_side_to_end),
        int(input_flip_male),
        index_polylines,  # out
        index_polylines_segment,  # out
        distance,  # out
        point_pairs,  # out
        volume_pairs,  # out
        joints_areas,  # out
        joints_types,  # out
        w_output_plines,  # from_point3(w_output_plines)
        w_output_types,  # from_cut_type2(w_output_types))
        input_compute_joints,
        input_division_distance,
        input_shift,
        input_output_geometry_type,
        input_use_eccentricities_to_scale_joints,
    )

    return (
        from_int2(index_polylines),
        from_int2(index_polylines_segment),
        from_double2(distance),
        from_point2(point_pairs),
        from_point3(volume_pairs),
        from_point2(joints_areas),
        from_int1(joints_types),
    )
