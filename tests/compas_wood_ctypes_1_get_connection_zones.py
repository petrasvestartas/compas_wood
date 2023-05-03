import data_set_plates

# from compas_wood.viewer_helpers import display
import time
from ctypes import *
from compas_wood.ctypes_conversion import *
from compas_wood.viewer_helpers import display


def test_connection_detection():
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

    ############################################################################################################
    # Load inputs and perform conversion
    ############################################################################################################

    # input geometry
    input_polylines = data_set_plates.annen_small_polylines()
    # print(len(input_polylines))
    (
        in_polyline_pairs_f,
        in_polyline_pairs_f_s,
        in_polyline_pairs_v,
        in_polyline_pairs_v_s,
    ) = list_polylines_coord(input_polylines)

    input_vectors = data_set_plates.annen_small_edge_directions()

    (
        in_vectors_f,
        in_vectors_f_s,
        in_vectors_v,
        in_vectors_v_s,
    ) = lists_vectors_coord(input_vectors)

    # print(in_vectors_f[0])
    # print(in_vectors_f[1])
    # # print(in_vectors_f[2])
    # # print(in_vectors_f[3])
    # print(in_vectors_f_s.value)
    # print(in_vectors_v[0])
    # print(in_vectors_v[1])
    # print(in_vectors_v[2])
    # print(in_vectors_v_s.value)

    input_joints = data_set_plates.annen_small_edge_joints()
    # print(len(input_joints))
    (
        in_joints_f,
        in_joints_f_s,
        in_joints_v,
        in_joints_v_s,
    ) = lists_numbers_coord(input_joints)

    input_three_valence = (
        data_set_plates.annen_small_three_valance_element_indices_and_instruction()
    )
    # print(len(input_three_valence))

    (
        in_three_valence_f,
        in_three_valence_f_s,
        in_three_valence_v,
        in_three_valence_v_s,
    ) = lists_numbers_coord(input_three_valence)

    input_adjacency = []

    (
        in_adjancency_v,
        in_adjancency_v_s,
    ) = list_ints_coord(input_adjacency)

    # input parameters
    (
        in_joint_parameters_v,
        in_joint_parameters_v_s,
    ) = list_numbers_coord(joint_parameters)

    in_search_type = c_int(0)
    (
        in_scale_v,
        in_scale_v_s,
    ) = list_numbers_coord([1, 1, 1])

    in_output_type = c_int(3)

    # output

    # out polylines
    out_plines_groups_f = POINTER(c_size_t)()
    out_plines_groups_f_s = c_size_t()
    out_plines_out_f = POINTER(c_size_t)()
    out_plines_out_f_s = c_size_t()
    out_plines_out_v = POINTER(c_float)()
    out_plines_out_v_s = c_size_t()

    # out types
    out_types_f = POINTER(c_size_t)()
    out_types_f_s = c_size_t()
    out_types_v = POINTER(c_int)()
    out_types_v_s = c_size_t()

    # global_parameters
    (
        in_joint_volume_parameters_v,
        in_joint_volume_parameters_v_s,
    ) = list_numbers_coord([0, 0, 0])

    ############################################################################################################
    # Load library and declare function signature
    ############################################################################################################
    lib = cdll.LoadLibrary(
        "C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll"
    )
    # lib = cdll.LoadLibrary(
    #     "C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll"
    # )
    lib.ctypes_get_connection_zones.restype = None
    lib.ctypes_get_connection_zones.argtypes = [
        # input geometry
        # polylines
        POINTER(c_size_t),
        POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
        POINTER(c_float),
        POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
        # vectors
        POINTER(c_size_t),
        POINTER(c_size_t),
        POINTER(c_float),
        POINTER(c_size_t),
        # joints
        POINTER(c_size_t),
        POINTER(c_size_t),
        POINTER(c_int),
        POINTER(c_size_t),
        # three valence
        POINTER(c_size_t),
        POINTER(c_size_t),
        POINTER(c_int),
        POINTER(c_size_t),
        # adjacency
        POINTER(c_int),
        POINTER(c_size_t),
        # input parameters
        POINTER(c_float),
        POINTER(c_size_t),
        # search_type
        POINTER(c_int),
        # scale
        POINTER(c_float),
        POINTER(c_size_t),
        # output_type
        POINTER(c_int),
        # output
        # polylines
        POINTER(POINTER(c_size_t)),
        POINTER(c_size_t),
        POINTER(POINTER(c_size_t)),
        POINTER(c_size_t),
        POINTER(POINTER(c_float)),
        POINTER(c_size_t),
        # types
        POINTER(POINTER(c_size_t)),
        POINTER(c_size_t),
        POINTER(POINTER(c_int)),
        POINTER(c_size_t),
        # global_parameters
        POINTER(c_float),
        POINTER(c_size_t),
    ]

    # generate joints
    start_time = time.time()
    ############################################################################################################
    # Call the function
    # https://stackoverflow.com/questions/4145775/how-do-i-convert-a-python-list-into-a-c-array-by-using-ctypes
    # https://stackoverflow.com/questions/53265216/python-ctypes-type-conversion
    ############################################################################################################
    lib.ctypes_get_connection_zones(
        # input geometry
        # polylines
        in_polyline_pairs_f,
        byref(in_polyline_pairs_f_s),
        in_polyline_pairs_v,
        byref(in_polyline_pairs_v_s),
        # vectors
        in_vectors_f,
        byref(in_vectors_f_s),
        in_vectors_v,
        byref(in_vectors_v_s),
        # joints
        in_joints_f,
        byref(in_joints_f_s),
        in_joints_v,
        byref(in_joints_v_s),
        # three_valence
        in_three_valence_f,
        byref(in_three_valence_f_s),
        in_three_valence_v,
        byref(in_three_valence_v_s),
        # adjacency
        in_adjancency_v,
        byref(in_adjancency_v_s),
        # input parameters
        # joint_parameters
        in_joint_parameters_v,
        byref(in_joint_parameters_v_s),
        # search_type
        byref(in_search_type),
        # scale
        in_scale_v,
        byref(in_scale_v_s),
        # output_type
        byref(in_output_type),
        # output
        # polylines
        byref(out_plines_groups_f),
        byref(out_plines_groups_f_s),
        byref(out_plines_out_f),
        byref(out_plines_out_f_s),
        byref(out_plines_out_v),
        byref(out_plines_out_v_s),
        # types
        byref(out_types_f),
        byref(out_types_f_s),
        byref(out_types_v),
        byref(out_types_v_s),
        # global_parameters
        in_joint_volume_parameters_v,
        byref(in_joint_volume_parameters_v_s),
    )

    out_polylines = coord_polylines_lists(
        out_plines_groups_f,
        out_plines_groups_f_s,
        out_plines_out_f,
        out_plines_out_f_s,
        out_plines_out_v,
        out_plines_out_v_s,
    )

    out_types = coord_numbers_lists(
        out_types_f, out_types_f_s, out_types_v, out_types_v_s
    )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )

    ############################################################################################################
    # release the memory
    ############################################################################################################

    # release output of polylines
    lib.release_size_t(byref(out_plines_groups_f), True)
    lib.release_size_t(byref(out_plines_out_f), True)
    lib.release_float(byref(out_plines_out_v), True)

    # release output of types
    lib.release_size_t(byref(out_types_f), True)
    lib.release_int(byref(out_types_v), True)

    ############################################################################################################
    # display
    ############################################################################################################

    # display
    result_flat_list = [item for sublist in out_polylines for item in sublist]
    display(result_flat_list, None, None, 0.001, 0, 0, 0, False)


test_connection_detection()
