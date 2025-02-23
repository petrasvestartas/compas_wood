#! python3
# venv: timber_connections
import Rhino
import Rhino.Input.Custom as ric
import wood_nano
from wood_rui import (
    wood_rui_globals,
    add_joinery,
    add_polylines,
    process_input,
    Element,
)
from wood_nano import get_connection_zones as wood_nano_get_connection_zones
from wood_nano import cut_type2 as wood_nano_cut_type2
from wood_nano import point3 as wood_nano_point3
from wood_nano.conversions_python import to_int2
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import to_double1
from wood_nano.conversions_python import from_cut_type2
from compas_wood.conversions_rhino import to_point2
from compas_wood.conversions_rhino import to_vector2
from compas_wood.conversions_rhino import from_point3
from compas_wood.binding import wood_globals
from typing import List


def closest_points(
    flat_polylines: List[Rhino.Geometry.Polyline],
    points: List[Rhino.Geometry.Point3d],
    points_ids: List[int],
) -> None:
    """component iterates polyline edges and assigns index based on the point list and their type RTree search

    Match the closest lines from the selected dataset to the input lines and update
    the insertion vectors.

    Parameters
    ----------
    flat_polylines : List[Rhino.Geometry.Polyline]
        The flat list of polylines of all element pair_polylines.
    points : List[Rhino.Geometry.Point3d]
        List of points, representing the position of joint types.
    joint_types : List[int]
        List of integers, representing joint types.
    """

    # print(flat_polylines)
    # print(points)
    # print(points_ids)

    ###############################################################################
    # Conversion
    ###############################################################################

    # Iterate over the flat list in steps of 2
    polylines = []
    for i in range(0, len(flat_polylines), 2):
        # Each sublist will contain two consecutive points
        sublist: List[Any] = [flat_polylines[i], flat_polylines[i + 1]]
        polylines.append(sublist)

    ###############################################################################
    # Run the closest point search
    ###############################################################################

    # convert element polylines to a flattened dictionary:
    segments_dictionary = {}
    # stores ids e.g. int , [element_int,bottom_or_top_polyline_int,edge,int, bounding_box, hit_count, point_id]
    joint_types = []
    count = 0
    for i in range(len(polylines)):
        joint_types.append([-1] * (polylines[i][0].SegmentCount + 2))
        for j in range(len(polylines[i])):
            for k in range(polylines[i][j].SegmentCount):
                bbox = polylines[i][j].SegmentAt(k).BoundingBox
                bbox.Inflate(0.02)
                segments_dictionary[count] = [
                    i,
                    j,
                    k,
                    bbox,
                    polylines[i][j].SegmentAt(k),
                ]
                count = count + 1

    # create a tree
    rtree = Rhino.Geometry.RTree()

    # fill the tree
    for i in segments_dictionary:
        rtree.Insert(segments_dictionary[i][3], i)

    # call_backs of rtree
    def search_callback(sender, rtree_event_args):
        data_by_reference.append(rtree_event_args.Id)

    # perform search
    for i in range(len(points)):
        data_by_reference = []
        if rtree.Search(
            Rhino.Geometry.Sphere(points[i], 0), search_callback, data_by_reference
        ):
            for j in data_by_reference:
                if (
                    points[i].DistanceToSquared(
                        segments_dictionary[j][4].ClosestPoint(points[i], True)
                    )
                    < 1.001
                ):
                    joint_types[segments_dictionary[j][0]][
                        segments_dictionary[j][2] + 2
                    ] = points_ids[i]

    ###############################################################################
    # Output
    ###############################################################################
    return joint_types


def callback(selection: dict[str, any], name: str):
    ######################################################################
    # Get elements and other inputs from the Command Line
    ######################################################################
    input_elements: list[Element] = selection["elements"]

    if len(input_elements) == 0:
        print("Attention: no elements selected.")
        return

    if len(selection["joint_types"]) == 0:
        print("Attention: no elements selected.")
        return

    ######################################################################
    # Get pair_polylines and assign joint types.
    # The assignment is made by write type to user string attributes.
    ######################################################################

    polylines: list[Rhino.Geometry.Polyline] = []
    for element in input_elements:
        for pair_polyline in element.pair_polyline:
            polylines.extend(pair_polyline)

    points: list[Rhino.Geometry.Point3d] = []
    joint_types: list[int] = []
    for textdot in selection["joint_types"]:
        point = textdot.Point
        text = textdot.Text

        try:
            joint_type = int(text)
            points.append(point)
            joint_types.append(joint_type)
        except ValueError:
            print("TextDot contains text that is not an integer: ", textdot)

    list_of_joint_types: list[list[int]] = closest_points(
        polylines, points, joint_types
    )

    ######################################################################
    # Set the list of joints types to user strings
    ######################################################################
    count = 0
    for element in input_elements:
        element_joint_types = []
        for pair_polyline in element.pair_polyline:
            element_joint_types.append(list_of_joint_types[count])
            count = count + 1
        element.joint_types = element_joint_types


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], List[Element]),
        "joint_types": ([], List[Rhino.Geometry.TextDot]),
    }

    process_input(selection_types, callback)
