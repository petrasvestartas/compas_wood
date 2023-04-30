import data_set_plates

# from compas_wood.viewer_helpers import display
import time
from ctypes import *
from compas_wood.ctypes_conversion import *


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

    # generate joints
    start_time = time.time()

    ############################################################################################################
    # Load inputs and perform conversion
    ############################################################################################################
    input_polylines = data_set_plates.annen_small_polylines()
    (
        in_polyline_pairs_f,
        in_polyline_pairs_f_s,
        in_polyline_pairs_v,
        in_polyline_pairs_v_s,
    ) = list_polylines_coord(input_polylines)

    # data_set_plates.annen_small_edge_directions(),
    # data_set_plates.annen_small_edge_joints(),
    # data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
    input_vectors = data_set_plates.annen_small_edge_directions()
    (
        in_vectors_f,
        in_vectors_f_s,
        in_vectors_v,
        in_vectors_v_s,
    ) = lists_vectors_coord(input_vectors)
   
    ############################################################################################################
    # Load library and declare function signature
    ############################################################################################################
    lib = cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")
    # lib = cdll.LoadLibrary(
    #     "C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll"
    # )
    lib.ctypes_get_connection_zones.restype = None
    lib.ctypes_get_connection_zones.argtypes = [
        # polylines
        POINTER(
            c_size_t
        ),  # size of polylines are transfered as a point, never write POINTER(POINTER(c_size_t)), always arrays will be transfered as pointers
        POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
        POINTER(c_float),
        POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
        # vectors
        POINTER(c_size_t),
        POINTER(c_size_t), 
        POINTER(c_float),
        POINTER(c_size_t), 
    ]

    ############################################################################################################
    # Call the function
    # https://stackoverflow.com/questions/4145775/how-do-i-convert-a-python-list-into-a-c-array-by-using-ctypes
    # https://stackoverflow.com/questions/53265216/python-ctypes-type-conversion
    ############################################################################################################
    # print(in_polyline_pairs_f)
    # a = (c_size_t * len(in_polyline_pairs_f))(*in_polyline_pairs_f)
    # b = c_size_t(in_polyline_pairs_f_s)
    # c = (c_float * len(in_polyline_pairs_v))(*in_polyline_pairs_v)
    # d = c_size_t(in_polyline_pairs_v_s)
    # print(a)
    # print(in_polyline_pairs_f)
    # print(a[0])
    """
    
    
    c = (c_float * len(in_polyline_pairs_v))(*in_polyline_pairs_v)
    d = c_size_t(in_polyline_pairs_v_s)
    lib.get_connection_zones(a,b,c,d)
    """

    # lib.ctypes_get_connection_zones(a, byref(b), c, byref(d))
    lib.ctypes_get_connection_zones(
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
    )

    # for i in range(b.value):
    #     print(a[i])

    # lib.get_connection_zones(byref(in_polyline_pairs_f), byref(in_polyline_pairs_f_s), byref(in_polyline_pairs_v), byref(in_polyline_pairs_v_s))

    # result = get_connection_zones(
    #     data_set_plates.annen_small_polylines(),
    #     data_set_plates.annen_small_edge_directions(),
    #     data_set_plates.annen_small_edge_joints(),
    #     data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
    #     None,
    #     joint_parameters,
    #     0,
    #     [1, 1, 1],
    #     4
    # )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )

    # # display
    # result_flat_list = [item for sublist in result for item in sublist]
    # # print(result_flat_list)
    # display(result_flat_list, None, None, 0.001, 0, 0, 0, False)


test_connection_detection()
