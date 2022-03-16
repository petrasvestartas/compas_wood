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

    # Generate Parameters
    division_length = 300
    joint_parameters = [
        division_length,
        0.5,
        8,
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
        58,
    ]

    # Input pairs of polylines
    input = data_set_plates.ss_30()

    # Generate connections
    result = get_connection_zones(
        input,
        None,
        None,
        None,
        None,
        joint_parameters,
        0,
        3,
        0,
        [10, 10, 6, 10, 10, 6, 10, 10, 6, 10, 10, 6, 10, 10, 6, 10, 10, 6],
    )
    result_flat_list = [item for sublist in result for item in sublist]

    # Display via Compas_View2
    id = 15
    display([input[id * 2 + 0], input[id * 2 + 1]], result[id], None, 0.01, 1)

    # display(None, result_flat_list, None, 0.01, 1)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
