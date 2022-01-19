# compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones
import data_set

# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Generate connections
    result = get_connection_zones(
        data_set.annen_small_polylines(),
        data_set.annen_small_edge_directions(),
        data_set.annen_small_edge_joints(),
        data_set.annen_small_three_valance_element_indices_and_instruction(),
    )

    result_flat_list = [item for sublist in result for item in sublist]

    # Display via Compas_View2
    display(None, result_flat_list, None, 0.01, 0, 0, 0, False)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
