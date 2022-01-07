#compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones
from compas_wood.data.joinery_solver_data_set import *

#viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================
def test_connection_detection():

    # Generate connections
    result = get_connection_zones(
        annen_small_polylines(),
        annen_small_edge_directions(),
        annen_small_edge_joints(),
        annen_small_three_valance_element_indices_and_instruction()
        )

    # Display via Compas_View2
    display(input, result,0.01,1.25)

    #output
    return result

# ==============================================================================
#call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
