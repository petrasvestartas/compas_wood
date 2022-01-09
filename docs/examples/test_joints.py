#compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import joints
from compas_wood.data.joinery_solver_data_set import *

#viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================

def test_joints():


    # Get a list of polyline pairs
    input = c_15()



    #Compute joint polylines
    element_pairs_list, joint_areas_polylines, joint_types = joints(input,2)
    #print((len)(joint_areas_polylines))
    #print((len)(joint_types))
    print(element_pairs_list)

    # Display via Compas_View2
    display(input, joint_areas_polylines, None,0.01,0,0,0,False,joint_types)


    #output
    #return neighbours

# ==============================================================================
#call the compas_wood methods
# ==============================================================================
test_joints()
