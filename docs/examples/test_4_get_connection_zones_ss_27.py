# compas_wood
from compas_wood.joinery import get_connection_zones
from compas_wood.joinery import closed_mesh_from_polylines
import data_set_plates


# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Generate Parameters
    division_length = 300
    joint_parameters = [
        division_length,
        0.5,
        9,
        division_length * 1.5,
        0.5,
        19,
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

    # Generate connections
    input = data_set_plates.ss_27()
    input.reverse()
    result = get_connection_zones(
        input,
        None,
        None,
        None,
        None,
        joint_parameters,
        0,
        300,
        0.6,
        4,
    )

    # Mesh Polylines
    meshes = []
    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])
        meshes.append(mesh_result)

    # data_set_plates_annen.annen_three_valance_element_indices_and_instruction(),
    result_flat_list = [item for sublist in result for item in sublist]
    # result_flat_list = result[0]
    # Display via Compas_View2
    display(result_flat_list, None, meshes, 0.01, 0, 0, 0, False)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_connection_detection()
