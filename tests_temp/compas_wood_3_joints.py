# compas_wood
from compas_wood.joinery import joints
import data_set_plates


# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Get Joints (element pairs and connection areas)
# ==============================================================================


def test_joints():
    # Get a list of polyline pairs
    input = data_set_plates.annen_small_polylines()

    # Compute joint polylines
    element_pairs_list, joint_areas_polylines, joint_types = joints(input, 0)
    print(joint_types)

    # Display via Compas_View2
    # display(input, joint_areas_polylines, None, 0.001, 0, 0, 0, False, joint_types)  #


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_joints()
