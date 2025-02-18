#! python3
# venv: timber_connections
import Rhino
import Rhino.Input.Custom as ric
from wood_rui import wood_rui_globals, add_joinery, add_polylines, generalized_input_method, Element
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



def solver(dataset_name: str) -> None:
    """
    Run the solver with the selected dataset, using properties from global parameters.

    Args:
        dataset_name (str): Name of the dataset to solve.
    """

    w_output_plines: wood_nano_point3 = wood_nano_point3()
    w_output_types: wood_nano_cut_type2 = wood_nano_cut_type2()

    adjacency_flat_list: List[int] = []
    adjacency_data: List[List[int]] = (
        wood_rui_globals[dataset_name]["adjacency"]
        if wood_rui_globals[dataset_name]["adjacency"]
        else wood_rui_globals.adjacency
    )

    for sublist in adjacency_data:
        for item in sublist:
            adjacency_flat_list.append(item)

        # print(wood_rui_globals[dataset_name]["joints_per_face"])
        # print(wood_rui_globals[dataset_name]["three_valence"])

    # select the three valence depending on the dataset dictionary property or the global property
    three_valence = (
        wood_rui_globals[dataset_name]["three_valence"]
        if wood_rui_globals[dataset_name]["three_valence"]
        else wood_rui_globals.three_valence
    )
    three_valence = (
        wood_rui_globals[dataset_name]["three_valence"]
        if wood_rui_globals[dataset_name]["three_valence"]
        else wood_rui_globals.three_valence
    )

    print(wood_rui_globals[dataset_name]["polylines"])

    wood_nano_get_connection_zones(
        to_point2(wood_rui_globals[dataset_name]["polylines"]),
        to_vector2(wood_rui_globals[dataset_name]["insertion_vectors"]),
        to_int2(
            wood_rui_globals[dataset_name]["joints_per_face"]
        ),  # to_int2(wood_rui_globals[dataset_name]["joints_dots"]),
        to_int2(
            three_valence
        ),  # to_int2(wood_rui_globals[dataset_name]["three_valence_element_indices_and_instruction"]),
        to_int1(adjacency_flat_list),
        to_double1(wood_globals.joints_parameters_and_types),
        int(wood_rui_globals.search_type),
        to_double1(wood_rui_globals.scale),
        int(wood_globals.output_geometry_type),
        w_output_plines,
        w_output_types,
        to_double1(wood_rui_globals.joint_volume_extension),
    )

    add_joinery(from_point3(w_output_plines), dataset_name)
    print(wood_rui_globals[dataset_name]["joinery"])

    # Add joint types if the output type is 3
    is_output_type_3: bool = int(wood_globals.output_geometry_type) == 3

    if is_output_type_3:
        joint_types: List[List[int]] = from_cut_type2(w_output_types)

    # Before assigning the joint type, be sure to have correct number of valid joinery polylines
    number_added_polylines: int = 0
    for i in range(len(wood_rui_globals[dataset_name]["joinery"])):
        number_added_polylines += len(wood_rui_globals[dataset_name]["joinery"][i])

    if number_added_polylines == len(wood_rui_globals[dataset_name]["joinery_guid"]):
        counter: int = 0
        for i, output_type in enumerate(w_output_types):
            for j in range(len(wood_rui_globals[dataset_name]["joinery"][i])):
                guid: str = wood_rui_globals[dataset_name]["joinery_guid"][counter]
                rhino_object: Rhino.DocObjects.RhinoObject = Rhino.RhinoDoc.ActiveDoc.Objects.Find(guid)

                if is_output_type_3:
                    joint_type: int = joint_types[i][int(j * 0.5)]
                    rhino_object.Attributes.SetUserString("joint_type", str(joint_type))

                rhino_object.Attributes.SetUserString("element_id", str(i))
                rhino_object.Attributes.SetUserString("face_id", str(j))
                counter += 1
    else:
        print("Number of added joinery is not equal to number joinery_guids. There are invalid polylines.")

    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def my_callback(
    name_value_type,
    dataset_name,
):

    print(name_value_type)
    input_elements = name_value_type["elements"]

    if len(input_elements) == 0:
        print("Attention: no elements selected.")
        return

if __name__ == "__main__":

    # Define the input dictionary based on your initial dataset
    input_dict = {
        "elements" : ([], List[Element]),
    }

    # Call the generalized input method with the dataset name and input dictionary
    dataset_name = "beams"
    wood_rui_globals.init_data(dataset_name)
    generalized_input_method(dataset_name, input_dict, my_callback)


# if __name__ == "__main__":
#     dataset_name: str = command_line_input()
#     if dataset_name:
#         solver(dataset_name)
