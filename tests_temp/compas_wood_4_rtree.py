# compas_wood
from compas_wood.joinery import rtree
import data_set_plates

# viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Get Adjancency between Elements using RTree Search
# ==============================================================================


def test_rtree(boxes_AABB_or_boxes_OOBB=False):
    # Get a list of polyline pairs
    input = data_set_plates.annen_small_polylines()

    # Compute Rtree
    neighbours, boxes_AABB, boxes_OOBB = rtree(input)

    # get aabb or oobb boxes
    selected_id = 22
    boxes_selected = []
    boxes = boxes_AABB if boxes_AABB_or_boxes_OOBB else boxes_OOBB

    # get first and its neighbors
    boxes_selected.append(boxes[selected_id])
    print(neighbours[selected_id])
    for i in neighbours[selected_id]:
        boxes_selected.append(boxes[i])

    # Display via Compas_View2
    # display(input, None, boxes_selected, 0.0001, 0, 0, 0, True)

    # output
    return neighbours


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_rtree(False)
