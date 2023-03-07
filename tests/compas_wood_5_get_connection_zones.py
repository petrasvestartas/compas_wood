# import faulthandler; faulthandler.enable()
# /Applications/anaconda3/envs/wood-dev/bin/python -Xfaulthandler /Users/vestarta/Documents/compas_wood/tests/compas_wood_5_get_connection_zones.py
# PYTHONFAULTHANDLER=1 /Applications/anaconda3/envs/wood-dev/bin/python /Users/vestarta/Documents/compas_wood/tests/compas_wood_5_get_connection_zones.py


from compas_wood.joinery import get_connection_zones
import data_set_plates
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

    # generate joints
<<<<<<< HEAD
=======
    # result = get_connection_zones(
    #     data_set_plates.annen_small_polylines(),
    #     data_set_plates.annen_small_edge_directions(),
    #     data_set_plates.annen_small_edge_joints(),
    #     data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
    #     None,
    #     joint_parameters,
    # )

>>>>>>> 63df54590b08fed34e8f3398a0d29c12eca06f44
    result = get_connection_zones(
        data_set_plates.annen_small_polylines(),
        data_set_plates.annen_small_edge_directions(),
        data_set_plates.annen_small_edge_joints(),
        data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
        None,
        joint_parameters,
        0,
        [1,1,1],
        4
        
    )

<<<<<<< HEAD

    # mesh polylines
    # meshes = []
    # for i in range(len(result)):
    #     mesh_result = closed_mesh_from_polylines(result[i])
    #     meshes.append(mesh_result)
    # display(result_flat_list, None, meshes, 0.01, 0, 0, 0, False)

=======
>>>>>>> 63df54590b08fed34e8f3398a0d29c12eca06f44
    # display
    
    result_flat_list = [item for sublist in result for item in sublist]
    print(result_flat_list)
    display(result_flat_list, None, None, 0.001, 0, 0, 0, False)


test_connection_detection()
