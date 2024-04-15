from wood_nano import get_connection_zones as wood_nano_get_connection_zones
from wood_nano import cut_type2 as wood_nano_cut_type2
from wood_nano import point3 as wood_nano_point3
from wood_nano import joint_parameters_and_types as wood_nano_joint_parameters_and_types
from wood_nano.conversions_python import to_int2
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import to_double1
from wood_nano.conversions_python import from_cut_type2
from compas_wood.conversions_compas import to_point2
from compas_wood.conversions_compas import to_vector2
from compas_wood.conversions_compas import from_point3
from math import floor


def get_connection_zones(
    input_polyline_pairs,
    input_insertion_vectors=[],
    input_joint_types=[],
    input_three_valence_element_indices_and_instruction=[],
    input_adjacency=[],
    input_joint_parameters_and_types=[],
    input_search_type=0,
    input_scale=[1, 1, 1],
    input_output_type=3,
    input_joint_volume_parameters=[0, 0, 0],
    face_to_face_side_to_side_joints_all_treated_as_rotated=False,
    input_custom_joints=[],
    input_custom_joints_types=[],
):
    """
    Get connection zones for the given input parameters.

    Parameters
    ----------
    input_polyline_pairs : list[list[compas.geometry.Polyline]]
        List of polylines that represents the timber elements.
    input_insertion_vectors : list[compas.geometry.Vector], optional
        List of insertion vectors.
    input_joint_types : list[int], optional
        List of joint types.
    input_three_valence_element_indices_and_instruction : list[int], optional
        List of three valence element indices and instruction.
    input_adjacency : list[int], optional
        List of adjacency.
    input_joint_parameters_and_types : list[float], optional
        List of joint parameters and types.
    input_search_type : int, optional
        Search type.
    input_scale : list[float], optional
        Scale.
    input_output_type : int, optional
        Output type.
    input_joint_volume_parameters : list[float], optional
        Joint volume parameters.
    face_to_face_side_to_side_joints_all_treated_as_rotated : bool, optional
        Face to face side to side joints all treated as rotated.
    input_custom_joints : list[compas.geometry.Point], optional
        Custom joints.
    input_custom_joints_types : list[int], optional
        Custom joints types.

    Returns
    -------
    list[list[compas.geometry.Polyline]]
        List of polylines that represents the timber elements.
    list[wood_nano.CutType2]
        List of cut types.
    """

    joint_parameters = wood_nano_joint_parameters_and_types()
    for i in range(0, len(input_joint_parameters_and_types), 3):
        insertion_id = floor(input_joint_parameters_and_types[i*3+2]/10)
        joint_parameters[insertion_id*3+0] = input_joint_parameters_and_types[i*3+0]
        joint_parameters[insertion_id*3+1] = input_joint_parameters_and_types[i*3+1]
        joint_parameters[insertion_id*3+2] = input_joint_parameters_and_types[i*3+2]
        

    w_output_plines = wood_nano_point3()
    w_output_types = wood_nano_cut_type2()

    wood_nano_get_connection_zones(
        to_point2(input_polyline_pairs),
        to_vector2(input_insertion_vectors),
        to_int2(input_joint_types),
        to_int2(input_three_valence_element_indices_and_instruction),
        to_int1(input_adjacency),
        to_double1(input_joint_parameters_and_types),
        int(input_search_type),
        to_double1(input_scale),
        int(input_output_type),
        w_output_plines,
        w_output_types,
        to_double1(input_joint_volume_parameters),
        bool(face_to_face_side_to_side_joints_all_treated_as_rotated),
        to_point2(input_custom_joints),
        to_int1(input_custom_joints_types),
    )

    return from_point3(w_output_plines), from_cut_type2(w_output_types)


if __name__ == "__main__":

    from compas.geometry import Polyline
    from compas_wood import data_sets_plates

    # joinery parameters
    division_length = 300
    joint_parameters = [
        division_length,
        0.5,
        9,
        division_length * 1.5,
        0.65,
        10,
        division_length * 1.5,
        0.5,
        21,
        division_length,
        0.95,
        30,
        division_length,
        0.95,
        40,
        division_length,
        0.95,
        50,
    ]

    # generate joints
    output_polylines, output_types = get_connection_zones(
        data_sets_plates.annen_small_polylines(),
        data_sets_plates.annen_small_edge_directions(),
        data_sets_plates.annen_small_edge_joints(),
        data_sets_plates.annen_small_three_valance_element_indices_and_instruction(),
        [],
        joint_parameters,
        0,
        [1, 1, 1],
        4,
    )

    import sys

    if sys.version_info >= (3, 9):

        from compas_viewer import Viewer
        from compas.geometry import Scale

        scale = 1e-3
        scale_transform = Scale.from_factors([scale, scale, scale])
        viewer = Viewer(show_grid=False)
        for polylines in output_polylines:
            for polyline in polylines:
                polyline.transform(scale_transform)
                viewer.scene.add(Polyline(polyline), show_points=False)

        viewer.show()
