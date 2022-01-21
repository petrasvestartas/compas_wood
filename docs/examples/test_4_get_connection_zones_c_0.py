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

    # Input pairs of polylines
    input = data_set.c_9()

    # Generate connections
    result = get_connection_zones(input, None, None, None, None, 1)
    result_flat_list = [item for sublist in result for item in sublist]

    # Display via Compas_View2
    display(input, result_flat_list, None, 0.01, 1.25)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
