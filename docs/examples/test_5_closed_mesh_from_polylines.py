# compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones
from compas_wood.joinery import closed_mesh_from_polylines
import data_set_plates

# viewer
from compas_wood.viewer_helpers import display

import os

os.system("cls" if os.name == "nt" else "clear")

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Generate connections
    # result = get_connection_zones(c_15())
    result = get_connection_zones(data_set_plates.ss_0())

    """
    result = get_connection_zones(
    annen_small_polylines(),
    annen_small_edge_directions(),
    annen_small_edge_joints(),
    annen_small_three_valance_element_indices_and_instruction()
    )
    """

    # Mesh Polylines
    meshes = []

    count = 0

    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])
        meshes.append(mesh_result)
        print("Mesh" + (str)(count))
        count += 1

    # meshes = [closed_mesh_from_polylines(result[21])]

    # Display via Compas_View2
    display(None, None, meshes, 0.01, 2, 0)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
