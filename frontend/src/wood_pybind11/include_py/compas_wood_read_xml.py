####################################################################################################################################
# Link the python module compiled in the release folder
####################################################################################################################################
import os.path as path
import sys

module_folder = path.abspath(path.join(__file__, "../../../..")) + "\\build_win\\Release"
print(module_folder)
sys.path.append(module_folder)
sys.path.append("C://IBOIS57//_Code//Software//Python//compas_wood//frontend//build_win//Release//")


####################################################################################################################################
# Import the compas_wood module
####################################################################################################################################
import wood_pybind11
from wood_pybind11 import WoodVectorDouble
from wood_pybind11 import WoodNestedVectorDouble
from wood_pybind11 import get_data_set

polyline_0 = WoodVectorDouble([0.5, 1.4, 2.3])
polyline_1 = WoodVectorDouble([0.5, 1.4, 2.3])
print(polyline_0)
polylines_coordinates = WoodNestedVectorDouble([polyline_0, polyline_1])

get_data_set("type_plates_name_cross_vda_hexshell_reciprocal", polylines_coordinates)

for i in range(len(polylines_coordinates)):
    print(polylines_coordinates[i])


# print(polylines_coordinates[0])
# print(polylines_coordinates[1])
# print(polylines_coordinates[2])
