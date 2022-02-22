# compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones, closed_mesh_from_polylines

# import data_set_plates
# import data_set_plates_annen
import data_set_custom

# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Generate connections
    """
    result = get_connection_zones(
        data_set_plates.annen_small_polylines(),
        data_set_plates.annen_small_edge_directions(),
        data_set_plates.annen_small_edge_joints(),
        data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
    )
    """

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
    # division_length * 1.5,
    result = get_connection_zones(
        data_set_custom.polylines(),
        data_set_custom.insertion_vectors(),
        data_set_custom.joints_per_face(),
        data_set_custom.three_valence(),
        data_set_custom.adjacency(),
        joint_parameters,
        0,
        10000,
        0.6,
        4,
    )

    # Mesh Polylines
    meshes = []
    result_flat_list2 = []

    count = 0

    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])

        if len(list(mesh_result.vertices())) == 0:  # mesh_result.is_valid() == False or
            result_flat_list2.extend(result[i])
            print("Not Valid Mesh")
            break
        else:
            meshes.append(mesh_result)
        count += 1
        # if len(meshes) > 100:
        #    break

    # data_set_plates_annen.annen_three_valance_element_indices_and_instruction(),
    result_flat_list = [item for sublist in result for item in sublist]

    # Display via Compas_View2
    display(result_flat_list, None, None, 0.01, 0, 0, 0, False)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
