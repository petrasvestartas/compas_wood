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
    input = data_set_plates.ss_24()

    # Compute joint polylines
    element_pairs_list, joint_areas_polylines, JOINTS_TYPES = joints(input, 0)
    # print(JOINTS_TYPES)

    # Display via Compas_View2
    display(input, joint_areas_polylines, None, 0.01, 0, 0, 0, False, joint_types)  #


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_joints()
