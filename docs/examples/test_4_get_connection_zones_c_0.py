# compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones
import data_set_plates


# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Input pairs of polylines
    input = data_set_plates.c_0()

    # Generate connections
    result = get_connection_zones(input, None, None, None, None, None, 2, 300, 0.6, 4)
    result_flat_list = [item for sublist in result for item in sublist]

    # Display via Compas_View2
    display(result_flat_list, None, None, 0.01, 0)

    # output
    # return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
