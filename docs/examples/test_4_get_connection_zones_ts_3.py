# compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import get_connection_zones
from compas_wood.joinery import closed_mesh_from_polylines
import data_set_plates


# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================


def test_connection_detection():

    # Input pairs of polylines
    input = data_set_plates.ts_3()

    # Generate connections
    result = get_connection_zones(input, None, None, None, None, 0)
    result_flat_list = [item for sublist in result for item in sublist]

    # Generate mesh
    meshes = []
    # result_empty = []
    for i in range(0, len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])

        # print(len(list(mesh_result.vertices())))
        if len(list(mesh_result.vertices())) != 0:
            # result_empty.extend(result[i])
            meshes.append(mesh_result)

    # Display via Compas_View2
    display(input, result_flat_list, meshes, 0.01, 1.25)

    # output
    return result


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test()
test_connection_detection()
