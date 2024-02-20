# compas_wood
from compas_wood.joinery import rtree
from compas_wood.data_set_plates import annen_small_polylines


# ==============================================================================
# Get Adjancency between Elements using RTree Search
# ==============================================================================


def test_rtree(boxes_AABB_or_boxes_OOBB=False):
    # Get a list of polyline pairs
    input = annen_small_polylines()

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

    # output
    return neighbours


# ==============================================================================
# call the compas_wood methods
# ==============================================================================
test_rtree(False)
