#compas_wood
from compas_wood.joinery import wrapper_test
from compas_wood.joinery import get_connection_zones
from compas_wood.data.joinery_solver_sideside import *

#viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================
def test_side_side_connections():

    # Get a list of polyline pairs
    input = SS0();

    # Generate connections
    result = get_connection_zones(input)

    # Display via Compas_View2
    display([], result)

    #output
    return result

# ==============================================================================
#call the compas_wood methods
# ==============================================================================
wrapper_test()
test_side_side_connections()
