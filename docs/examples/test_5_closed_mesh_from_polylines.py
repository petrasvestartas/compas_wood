# compas_wood
from compas_wood.joinery import get_connection_zones
from compas_wood.joinery import closed_mesh_from_polylines
import data_set_plates

# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create joints and mesh them
# ==============================================================================


def test_connection_detection():

    # Parameters
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

    # Generate connections
    result = get_connection_zones(
        data_set_plates.annen_small_polylines(),
        data_set_plates.annen_small_edge_directions(),
        data_set_plates.annen_small_edge_joints(),
        data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
        None,
        joint_parameters,
    )

    # Mesh Polylines
    meshes = []
    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])
        meshes.append(mesh_result)

    # Display via Compas_View2
    display(None, None, meshes, 0.01, 2, 0)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_connection_detection()
