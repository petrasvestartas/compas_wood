# compas_wood
from compas_wood.joinery import beam_volumes
import data_set_beams

# viewer
from compas_wood.viewer_helpers import display

import os

# from data_set_beams import polylines_0

os.system("cls" if os.name == "nt" else "clear")

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_beam_volumes():

    polylines = data_set_beams.polylines_1()
    segment_radii = data_set_beams.polylines_radii_1()
    # segment_normals = data_set_beams.polylines_vector_0()

    # print(segment_normals)

    allowed_types = []
    for i in range(len(polylines)):
        allowed_types.append(1)

    # Generate connections
    # Parameter t explanation
    # x<0 - all cross
    # x > 1.0 all side-end
    #
    # how close the point is to the middle from
    # (closest to the middle) 0 <= x <= 1 (closest to the edge)
    # possible output: one is above another below - side-to-end joint  e.g. 0.9999
    # possible output: when both are above, the smaller is female, other is male 0.75
    # possible output: when both are below, then cross, e.g. 0.0001

    division_length = 300
    joint_parameters = [
        division_length,
        0.5,
        9,
        division_length * 1.5,
        0.65,
        10,
        division_length,
        0.5,
        29,
        division_length,
        0.5,
        30,
        division_length,
        0.5,
        40,
        division_length,
        0.5,
        15,
    ]

    # segment_normals
    (
        neighbours,
        pair_points,
        volume_pairs,
        joint_areas_list,
        joint_types_list,
        joint_geometry_list,
    ) = beam_volumes(
        polylines,
        segment_radii,
        None,
        allowed_types,
        20,
        30,
        0.91,
        1,
        joint_parameters,
    )
    joint_areas_list.extend(polylines)

    """
    # Display normals
    temp_id = 27
    p0 = midpoint_point_point(polylines[temp_id][0], polylines[temp_id][1])
    v = scale_vector(segment_normals[temp_id], 15)
    p1 = Point(p0[0] + v[0], p0[1] + v[1], p0[2] + v[2])
    pline = Polyline([p0, p1])

    polylines = [polylines[temp_id], pline]

    normals = []
    for i in range(len(polylines)):
        p0 = midpoint_point_point(polylines[i][0], polylines[i][1])
        v = scale_vector(segment_normals[i], 15)
        p1 = Point(p0[0] + v[0], p0[1] + v[1], p0[2] + v[2])
        pline = Polyline([p0, p1])
        normals.append(pline)
    polylines.extend(normals)
    """
    # print(joint_preview_list)
    # Display via Compas_View2
    """
    types = []
    for i in joint_areas_list:
        types.append(11)
    """

    volume_pairs.extend(polylines)
    display(
        joint_geometry_list,
        joint_areas_list,
        None,
        0.01,
        0,
        0,
        0,
        False,
        joint_types_list,
    )


# ==============================================================================
# call the compas_wood methods
# ==============================================================================

test_beam_volumes()
