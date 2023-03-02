####################################################################################################################################
# Link the python module compiled in the release folder - be sure that you do not have two module with the same name
####################################################################################################################################
import os.path as path
import sys

module_folder = (
    path.abspath(path.join(__file__, "../../../..")) + "\\build_win\\Release"
)
print(module_folder)
sys.path.append(module_folder)
sys.path.append(
    "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//build_win//Release//"
)


####################################################################################################################################
# Import the compas_wood module
####################################################################################################################################
import wood_pybind11
from wood_pybind11 import WoodVectorDouble
from wood_pybind11 import WoodNestedVectorDouble
from wood_pybind11 import WoodNestedNestedVectorDouble

from wood_pybind11 import WoodVectorInt
from wood_pybind11 import WoodNestedVectorInt
from wood_pybind11 import WoodNestedNestedVectorInt

# from wood_pybind11 import WoodNestedNestedVectorDouble
from wood_pybind11 import read_xml_polylines
from wood_pybind11 import read_xml_polylines_and_properties
from wood_pybind11 import get_connection_zones

polyline_0 = WoodVectorDouble([0.5, 1.4, 2.3])
polyline_1 = WoodVectorDouble([0.5, 1.4, 2.3])
print(polyline_0)
polylines_coordinates = WoodNestedVectorDouble([])

# read polylines
read_xml_polylines(
    "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//src//wood//dataset//",
    "type_plates_name_cross_vda_hexshell_reciprocal",
    polylines_coordinates,
)

# for i in range(len(polylines_coordinates)):
#     print(polylines_coordinates[i])

# read polylines and properties
input_polyline_pairs_coord = WoodNestedVectorDouble([])
input_insertion_vectors_coord = WoodNestedVectorDouble([])
input_JOINTS_TYPES = WoodNestedVectorInt([])
input_three_valence_element_indices_and_instruction = WoodNestedVectorInt([])
input_adjacency = WoodVectorInt([])


read_xml_polylines_and_properties(
    "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//src//wood//dataset//",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small",
    input_polyline_pairs_coord,
    input_insertion_vectors_coord,
    input_JOINTS_TYPES,
    input_three_valence_element_indices_and_instruction,
    input_adjacency,
)

# for i in range(len(input_polyline_pairs_coord)):
#     print(input_polyline_pairs_coord[i])

# for i in range(len(input_insertion_vectors_coord)):
#     print(input_insertion_vectors_coord[i])

# for i in range(len(input_JOINTS_TYPES)):
#     print(input_JOINTS_TYPES[i])

# for i in range(len(input_three_valence_element_indices_and_instruction)):
#     print(input_three_valence_element_indices_and_instruction[i])

# print(input_adjacency)

# 300, 0.5, 3,   // 1-9 ss_e_ip (side-to-side edge in-plane)
# 450, 0.64, 15, // 10-19 ss_e_op (side-to-side edge out-of-plane)
# 450, 0.5, 20,  // 20-29 ts_e_p (top-to-side edge plane)
# 300, 0.5, 30,  // 30-39 cr_c_ip (cross cutting in-plane)
# 6, 0.95, 40,   // 40-49 tt_e_p  (top-to-top edge plane)
# 300, 0.5, 58,  // 50-59 ss_e_r (side-to-side edge rotated)
# 300, 1.0, 60   // 60-69 b (boundary)

pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES = WoodVectorDouble(
    [
        300.0,
        0.5,
        3.0,
        200.0,
        0.66,
        10.0,
        300.0,
        0.5,
        20.0,
        300.0,
        0.5,
        30.0,
        6.0,
        0.95,
        40.0,
        300.0,
        0.5,
        58.0,
        300.0,
        1.0,
        60.0,
    ]
)

search_type = 0
scale = WoodVectorDouble([1, 1, 1])
pybind11_output_plines = WoodNestedNestedVectorDouble([])
pybind11_output_types = WoodNestedVectorInt([])
output_type = 3
get_connection_zones(
    # input
    input_polyline_pairs_coord,
    input_insertion_vectors_coord,
    input_JOINTS_TYPES,
    input_three_valence_element_indices_and_instruction,
    input_adjacency,
    pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES,
    search_type,
    scale,
    output_type,
    # output
    pybind11_output_plines,
    pybind11_output_types,
)

print(pybind11_output_plines[0][0])
# print(pybind11_output_types[0][0])
