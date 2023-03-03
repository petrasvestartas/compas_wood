from compas_wood.joinery import get_connection_zones
from compas_wood.joinery import closed_mesh_from_polylines
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
    result = get_connection_zones(
        data_set_plates.annen_small_polylines(),
        data_set_plates.annen_small_edge_directions(),
        data_set_plates.annen_small_edge_joints(),
        data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
        None,
        joint_parameters,
    )

    # display
    result_flat_list = [item for sublist in result for item in sublist]
    # display(result_flat_list, None, None, 0.001, 0, 0, 0, False)

    # mesh polylines

    meshes = []
    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])
        meshes.append(mesh_result)
    display(result_flat_list, None, meshes, 1, 0, 0, 0, False)


test_connection_detection()
