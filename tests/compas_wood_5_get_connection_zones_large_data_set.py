from compas_wood.joinery import get_connection_zones
import time
from compas_wood.joinery import filenames_of_datasets
from compas_wood.joinery import read_xml_polylines_and_properties
from compas_wood.cpp import rpc_test



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


    # read the xml file and get a data-set
    foldername = (
        "../compas_wood/src/frontend/src/wood/dataset/"
    )

    filename_of_dataset = filenames_of_datasets[32]

        
    # generate joints
    
    output_polylines, output_vectors,output_joints_types, output_three_valence_element_indices_and_instruction, output_adjacency = read_xml_polylines_and_properties(foldername, filename_of_dataset)
    

    result = get_connection_zones(
        output_polylines,
        output_vectors,
        output_joints_types,
        output_three_valence_element_indices_and_instruction,
        output_adjacency,
        joint_parameters,
        0,
        [1, 1, 1],
        4   
    )

    start_time = time.time()
    rpc_test()

    print(
         "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )


    # display
    result_flat_list = [item for sublist in result for item in sublist]
    print(result_flat_list)

test_connection_detection()
