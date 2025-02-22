#! python3
# venv: timber_connections
import Rhino
import Rhino.Input.Custom as ric
import wood_nano
from wood_rui import (
    wood_rui_globals,
    add_joinery,
    add_polylines,
    generalized_input_method,
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

######################################################################
# Select Elements:
# a) get indices and neighbors since selection is made in a random order.
# b) store joints in elements.
# c) create lofts and cylinders from the polyline geometry and store them in features.
# d) repeat the process for side-to-end, end-to-end, and cross connections as examples.
######################################################################


def my_callback(
    name_value_type,
    dataset_name,
):
    ######################################################################
    # Get elements and other inputs from the Command Line
    ######################################################################
    input_elements: list[Element] = name_value_type["elements"]

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
    index_to_group: dict[int, int] = {}
    for group_id, element in enumerate(input_elements):
        for polylines in element.volumes:
            flat_id: int = int(len(ir_polylines) * 0.5)
            index_to_group[flat_id] = group_id
            ir_polylines.extend(polylines)

    iw_polylines: wood_nano.wood_nano_ext.point2 = to_point2(ir_polylines)

    ######################################################################
    # input_insertion_vectors
    # insertion - lists of face vectors
    # [[Vector(x=0.0, y=0.0, z=0.0), Vector(x=0.0, y=0.0, z=0.0), ... ]
    ######################################################################

    ir_insertion = []

    for group_id, element in enumerate(input_elements):
        for volume in element.volumes:
            ir_insertion.append(element.joint_types)

    print(ir_insertion)

    iw_insertion: wood_nano.wood_nano_ext.vector2 = to_vector2(ir_insertion)

    ######################################################################
    # input_joint_types
    # joints_per_face - lists of joint types
    # [[0, 0, 20, 20, 20, 20], [0, 0, 20, 20, 20, 20] ... ]
    ######################################################################
    # for group_id, element in enumerate(input_elements):
    #     if element.element_type == "plate":
    #         for volume in element.volumes:
    #             number_of_faces : int = volume[0].Count+1
    #             current_insertion_vector = [Rhino.Geometry.Vector3d(0, 0, 0)] * number_of_faces
    #             ir_insertion.append(current_insertion_vector)

    ######################################################################
    # input_three_valence_element_indices_and_instruction
    # three_valence
    # [[0], [0, 2, 4, 2], [1, 2, 5, 2], [0, 3, 36, 3], ... ]
    ######################################################################

    ######################################################################
    # input_adjacency
    # adjacency_flat_list
    # current element, next element, current element face, next element face
    # For example: [0, 2, -1, -1, 0, 3, -1, -1, 1, 2, -1, -1 ... ]
    # If the adjacency is empty, it is computed in the solver.
    ######################################################################

    ######################################################################
    # input_joint_parameters_and_types
    # wood_globals.joints_parameters_and_types
    #  [division_length, 0.5, 9, ...]
    ######################################################################

    ######################################################################
    # input_search_type
    # wood_rui_globals.search_type
    # 0 - face detection, 1 - cross, 2 - both
    ######################################################################

    ######################################################################
    # input_scale
    # wood_rui_globals.scale
    # default: [1, 1, 1]
    ######################################################################

    ######################################################################
    # input_output_type
    # wood_globals.output_geometry_type
    # 0 - , 1 - , 2 - , 3 - , 4 -
    ######################################################################

    ######################################################################
    # input_joint_volume_parameters
    # wood_rui_globals.joint_volume_extension
    # default: [0, 0, 0]
    ######################################################################

    ######################################################################
    # input_custom_joints
    ######################################################################

    ######################################################################
    # input_custom_joints_types
    ######################################################################

    # w_output_plines: wood_nano_point3 = wood_nano_point3()
    # w_output_types: wood_nano_cut_type2 = wood_nano_cut_type2()

    # adjacency_flat_list: List[int] = []
    # adjacency_data: List[List[int]] = (
    #     wood_rui_globals[dataset_name]["adjacency"]
    #     if wood_rui_globals[dataset_name]["adjacency"]
    #     else wood_rui_globals.adjacency
    # )

    # for sublist in adjacency_data:
    #     for item in sublist:
    #         adjacency_flat_list.append(item)

    ######################################################################
    # Three Valence
    ######################################################################

    # print(wood_rui_globals[dataset_name]["joints_per_face"])
    # print(wood_rui_globals[dataset_name]["three_valence"])

    # # select the three valence depending on the dataset dictionary property or the global property
    # three_valence = (
    #     wood_rui_globals[dataset_name]["three_valence"]
    #     if wood_rui_globals[dataset_name]["three_valence"]
    #     else wood_rui_globals.three_valence
    # )
    # three_valence = (
    #     wood_rui_globals[dataset_name]["three_valence"]
    #     if wood_rui_globals[dataset_name]["three_valence"]
    #     else wood_rui_globals.three_valence
    # )

    # print(wood_rui_globals[dataset_name]["polylines"])

    # wood_nano_get_connection_zones(
    #     to_point2(wood_rui_globals[dataset_name]["polylines"]),
    #     to_vector2(wood_rui_globals[dataset_name]["insertion_vectors"]),
    #     to_int2(
    #         wood_rui_globals[dataset_name]["joints_per_face"]
    #     ),  # to_int2(wood_rui_globals[dataset_name]["joints_dots"]),
    #     to_int2(
    #         three_valence
    #     ),  # to_int2(wood_rui_globals[dataset_name]["three_valence_element_indices_and_instruction"]),
    #     to_int1(adjacency_flat_list),
    #     to_double1(wood_globals.joints_parameters_and_types),
    #     int(wood_rui_globals.search_type),
    #     to_double1(wood_rui_globals.scale),
    #     int(wood_globals.output_geometry_type),
    #     w_output_plines,
    #     w_output_types,
    #     to_double1(wood_rui_globals.joint_volume_extension),
    # )

    # add_joinery(from_point3(w_output_plines), dataset_name)
    # print(wood_rui_globals[dataset_name]["joinery"])

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

    # Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


if __name__ == "__main__":
    # Define the input dictionary based on your initial dataset
    input_dict = {
        "elements": ([], List[Element]),
    }

    # Call the generalized input method with the dataset name and input dictionary
    dataset_name = "beams"
    wood_rui_globals.init_data(dataset_name)
    generalized_input_method(dataset_name, input_dict, my_callback)
