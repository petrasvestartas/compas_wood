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
    in_polyline_pairs_f, in_polyline_pairs_f_s, in_polyline_pairs_v, in_polyline_pairs_v_s  = list_polylines_coord(input_polylines) 

    ############################################################################################################
    # Load library and declare function signature
    ############################################################################################################
    lib = cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")
    lib.get_connection_zones.restype = None
    lib.get_connection_zones.argtypes = [
        POINTER(c_size_t), c_size_t, POINTER(c_float), c_size_t
        ]
    
    ############################################################################################################
    # Call the function
    ############################################################################################################

    lib.get_connection_zones(
        byref(in_polyline_pairs_f), byref(in_polyline_pairs_f_s), byref(in_polyline_pairs_v), byref(in_polyline_pairs_v_s)
        )
    #lib.get_connection_zones(byref(in_polyline_pairs_f), byref(in_polyline_pairs_f_s), byref(in_polyline_pairs_v), byref(in_polyline_pairs_v_s))

    

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
