from compas_wood.joinery import filenames_of_datasets
from compas_wood.joinery import get_connection_zones
from compas_wood.viewer_helpers import display
from compas_wood.joinery import read_xml_polylines_and_properties
from compas_wood.joinery import closed_mesh_from_polylines

def test_connection_detection():

    # joinery parameters
    joint_parameters = [
        300, 0.5, 3,   # 1-9 ss_e_ip (side-to-side edge in-plane)
        50, 0.64, 15,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
        450, 0.5, 20,  # 20-29 ts_e_p (top-to-side edge plane)
        300, 0.5, 30,  # 30-39 cr_c_ip (cross cutting in-plane)
        6, 0.95, 40,   # 40-49 tt_e_p  (top-to-top edge plane)
        300, 0.5, 58,  # 50-59 ss_e_r (side-to-side edge rotated)
        300, 1.0, 60   # 60-69 b (boundary)
    ]

    # read the xml file
    foldername = (
            "../compas_wood/src/frontend/src/wood/dataset/"
    )

    filename_of_dataset_with_properties = filenames_of_datasets[13]
    (output_polylines_with_properties, output_vectors, output_joints_types, output_three_valence_element_indices_and_instruction, output_adjacency) = read_xml_polylines_and_properties(foldername, filename_of_dataset_with_properties)

    print(foldername)
    print(filename_of_dataset_with_properties)
    print(output_joints_types)

    # result = get_connection_zones(
    #     output_polylines_with_properties,
    #     output_vectors,
    #     output_joints_types,
    #     output_three_valence_element_indices_and_instruction,
    #     output_adjacency,
    #     joint_parameters,
    #     0,
    #     [1, 1, 1],
    #     4,
    #     False,
    #     [0, 0, -10])


    # # display
    
    # result_flat_list = [item for sublist in result for item in sublist]
    # # display(result_flat_list, None, None, 0.001, 0, 0, 0, False)

    # # mesh polylines
    # meshes = []
    # for i in range(len(result)):
    #     mesh_result = closed_mesh_from_polylines(result[i])
    #     meshes.append(mesh_result)
    # display(result_flat_list, None, meshes, 0.001, 0, 0, 0, False)


test_connection_detection()
