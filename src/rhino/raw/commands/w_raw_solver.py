#! python3
# venv: timber_connections
import Rhino
import Rhino.Input.Custom as ric
import wood_nano
from wood_rui import (
    wood_rui_globals,
    add_joinery,
    add_sub_layer,
    process_input,
    Element,
    add_sub_layer,
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
import System

######################################################################
# Select Elements:
# a) get indices and neighbors since selection is made in a random order.
# b) store joints in elements.
# c) create lofts and cylinders from the polyline geometry and store them in features.
# d) repeat the process for side-to-end, end-to-end, and cross connections as examples.
######################################################################


def callback(selection: dict[str, any], name: str):
    ######################################################################
    # Get elements and other inputs from the Command Line
    ######################################################################
    input_elements: list[Element] = selection["elements"]

    if len(input_elements) == 0:
        print("Attention: no elements selected.")
        return

    ######################################################################
    # input_polyline_pairs
    # Parameter: polylines - flat list of polylines
    # We will track indices to reassign joints to elements.
    # ir - stands for input rhino
    # iw - stands for input wood
    ######################################################################

    ir_polylines: list[Rhino.Geometry.Polyline] = []
    ir_index_to_sortedindex: dict = {}

    index_to_group: dict[int, (int, int)] = {}
    count = 0
    for group_id, element in enumerate(input_elements):
        for pair_id, pair in enumerate(element.pair_polyline):
            ir_polylines.extend(pair)

        for j in range(len(element.pair_indices)):
            ir_index_to_sortedindex[element.pair_indices[j]] = count
            index_to_group[count] = (group_id, pair_id)
            count = count + 1

    iw_polylines: wood_nano.wood_nano_ext.point2 = to_point2(ir_polylines)

    ######################################################################
    # input_insertion_vectors
    # insertion - lists of face vectors
    # [[Vector(x=0.0, y=0.0, z=0.0), Vector(x=0.0, y=0.0, z=0.0), ... ]
    ######################################################################

    ir_insertion = []

    # for group_id, element in enumerate(input_elements):
    #     ir_insertion.extend(element.insertion)

    iw_insertion: wood_nano.wood_nano_ext.vector2 = to_vector2(ir_insertion)

    ######################################################################
    # input_joint_types
    # joints_per_face - lists of joint types
    # [[0, 0, 20, 20, 20, 20], [0, 0, 20, 20, 20, 20] ... ]
    ######################################################################
    ir_joint_types = []
    for group_id, element in enumerate(input_elements):
        ir_joint_types.extend(element.joint_types)

    # ir_joint_types = [[-1,-1,-1,4,4,4], [-1,-1,-1,-1,-1,-1]]

    iw_joint_types: wood_nano.wood_nano_ext.int2 = to_int2(ir_joint_types)
    print("joint_types: ", ir_joint_types)

    ######################################################################
    # input_three_valence_element_indices_and_instruction
    # could it be set from global attributes?
    # [[0], [0, 2, 4, 2], [1, 2, 5, 2], [0, 3, 36, 3], ... ]
    ######################################################################
    ir_three_valence = []
    iw_three_valence: wood_nano.wood_nano_ext.int2 = to_int2(ir_three_valence)

    ######################################################################
    # input_adjacency
    # adjacency_flat_list
    # current element, next element, current element face, next element face
    # For example: [0, 2, -1, -1, 0, 3, -1, -1, 1, 2, -1, -1 ... ]
    # If the adjacency is empty, it is computed in the solver.
    # TODO: fix adjacency, currently neighbors are per element, not per polyline pair
    ######################################################################

    pair_indices = []

    adjacency_set = set()
    for group_id, element in enumerate(input_elements):
        for pair_id, neighbour_id_and_face in zip(
            element.pair_indices, element.pair_neighbours
        ):
            a = [pair_id, -1]
            b = neighbour_id_and_face

            if a[0] > b[0]:
                a, b = b, a
            if (
                a[0] in ir_index_to_sortedindex.keys()
                and b[0] in ir_index_to_sortedindex.keys()
            ):
                pair = (
                    ir_index_to_sortedindex[a[0]],
                    ir_index_to_sortedindex[b[0]],
                    -1,
                    -1,
                )

                adjacency_set.add(pair)

    ir_adjacency = []
    for a in adjacency_set:
        ir_adjacency.extend(a)
    print("adjancency: ", ir_adjacency)

    iw_adjacency = to_int1(ir_adjacency)

    ######################################################################
    # input_joint_parameters_and_types
    # wood_globals.joints_parameters_and_types
    #  [division_length, 0.5, 9, ...]
    # beams:
    # cross connection: any, 0.33, 33
    ######################################################################
    ir_joints_parameters_and_types = wood_globals.joints_parameters_and_types
    iw_joints_parameters_and_types = to_double1(ir_joints_parameters_and_types)

    ######################################################################
    # input_search_type
    # wood_rui_globals.search_type
    # 0 - face detection, 1 - cross, 2 - both
    # For beams we set to 2
    ######################################################################
    ir_search_type = 2  # wood_rui_globals.search_type
    # ir_search_type = 1 # TODO: remove after the code works
    iw_search_type = ir_search_type

    ######################################################################
    # input_scale
    # wood_rui_globals.scale
    # default: [1, 1, 1]
    ######################################################################
    ir_scale = wood_rui_globals.scale
    iw_scale = to_double1(ir_scale)

    ######################################################################
    # input_output_type
    # wood_globals.output_geometry_type
    # 0 - , 1 - , 2 - , 3 - , 4 -
    ######################################################################
    ir_output_geometry_type = min(3, wood_globals.output_geometry_type)
    iw_output_geometry_type = (
        ir_output_geometry_type  # TODO: remove after the code works
    )

    ######################################################################
    # input_joint_volume_parameters
    # wood_rui_globals.joint_volume_extension
    # default: [0, 0, 0]
    ######################################################################
    ir_joint_volume_extension = wood_rui_globals.joint_volume_extension
    iw_joint_volume_extension = to_double1(ir_joint_volume_extension)

    ######################################################################
    # output
    ######################################################################
    ow_polylines: wood_nano_point3 = wood_nano_point3()
    ow_fab_types: wood_nano_cut_type2 = wood_nano_cut_type2()

    ######################################################################
    # main method
    ######################################################################

    wood_nano_get_connection_zones(
        iw_polylines,
        iw_insertion,
        iw_joint_types,
        iw_three_valence,
        iw_adjacency,
        iw_joints_parameters_and_types,
        iw_search_type,
        iw_scale,
        iw_output_geometry_type,
        ow_polylines,
        ow_fab_types,
        iw_joint_volume_extension,
    )

    ######################################################################
    # output conversion
    ######################################################################
    polylines_lists: list[list[Rhino.Geometry.Polyline]] = from_point3(ow_polylines)
    fab_types: list[list[int]] = from_cut_type2(ow_fab_types)

    ######################################################################
    # Loft the output and add it to elements as features
    # index_to_group[count] = (group_id, pair_id)
    ######################################################################
    if wood_globals.output_geometry_type == 3 and wood_rui_globals.loft:
        # Loft polylines and collect them as list for elments features
        grouped_breps = [[] for _ in input_elements]

        count = 0
        for polylines, types in zip(polylines_lists, fab_types):
            for j in range(0, len(polylines), 2):
                polyline0 = polylines[j]
                polyline1 = polylines[j + 1]
                fab_type = types[int(j * 0.5)]

                print(polyline0.Count)
                print(polyline1.Count)
                print(fab_type)

                if fab_type != "drill" and polyline0.Count != 2:
                    brep = Element.loft_polylines_with_holes(
                        [polyline0.ToNurbsCurve()],
                        [polyline1.ToNurbsCurve()],
                        System.Drawing.Color.Red,
                    )
                    # Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(brep)
                    grouped_breps[index_to_group[count][0]].append(brep)
                elif wood_rui_globals.dowel_radius > 0.01:
                    plane = Rhino.Geometry.Plane(
                        polyline0[0], polyline0.SegmentAt(0).Direction
                    )
                    circle = Rhino.Geometry.Circle(plane, wood_rui_globals.dowel_radius)
                    brep = Rhino.Geometry.Cylinder(
                        circle, polyline0.SegmentAt(0).Length
                    ).ToBrep(True, True)
                    grouped_breps[index_to_group[count][0]].append(brep)
            count = count + 1

        # Add the breps to as features

        for idx, breps in enumerate(grouped_breps):
            # features = input_elements[idx].features + breps
            add_sub_layer(
                input_elements[idx].geometry_plane[0],
                "features",
                breps,
                [System.Drawing.Color.FromArgb(0, 0, 0)],
                idx == 0,
                True,
            )
            input_elements[idx].features = breps

    else:
        for idx, polylines_list in enumerate(polylines_lists):
            polylines = []
            for polyline in polylines_list:
                polylines.append(polyline.ToNurbsCurve())
            add_sub_layer(
                input_elements[idx].geometry_plane[0],
                "features_polylines",
                polylines,
                [System.Drawing.Color.FromArgb(0, 0, 0)],
                idx == 0,
                True,
            )
    return

    # add_joinery(fab_type, dataset_name)

    # # Add joint types if the output type is 3
    # is_output_type_3: bool = int(wood_globals.output_geometry_type) == 3

    # if is_output_type_3:
    #     joint_types: List[List[int]] = from_cut_type2(w_output_types)

    # # Before assigning the joint type, be sure to have correct number of valid joinery polylines
    # number_added_polylines: int = 0
    # for i in range(len(wood_rui_globals[dataset_name]["joinery"])):
    #     number_added_polylines += len(wood_rui_globals[dataset_name]["joinery"][i])

    # if number_added_polylines == len(wood_rui_globals[dataset_name]["joinery_guid"]):
    #     counter: int = 0
    #     for i, output_type in enumerate(w_output_types):
    #         for j in range(len(wood_rui_globals[dataset_name]["joinery"][i])):
    #             guid: str = wood_rui_globals[dataset_name]["joinery_guid"][counter]
    #             rhino_object: Rhino.DocObjects.RhinoObject = Rhino.RhinoDoc.ActiveDoc.Objects.Find(guid)

    #             if is_output_type_3:
    #                 joint_type: int = joint_types[i][int(j * 0.5)]
    #                 rhino_object.Attributes.SetUserString("joint_type", str(joint_type))

    #             rhino_object.Attributes.SetUserString("element_id", str(i))
    #             rhino_object.Attributes.SetUserString("face_id", str(j))
    #             counter += 1
    # else:
    #     print("Number of added joinery is not equal to number joinery_guids. There are invalid polylines.")

    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {"elements": ([], List[Element])}

    process_input(selection_types, callback)
