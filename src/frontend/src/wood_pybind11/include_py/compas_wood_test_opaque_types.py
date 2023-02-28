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

####################################################################################################################################
# test_opaque_types_wood_vector_int
####################################################################################################################################


from wood_pybind11 import test_opaque_types_wood_vector_int
from wood_pybind11 import WoodVectorInt

# this is std::vector<int>
list_of_integers = WoodVectorInt([0, 2, 4])
test_opaque_types_wood_vector_int(list_of_integers)
print(list_of_integers)

# print(list_of_integers[0])
# print(list(list_of_integers))
# list(list_of_integers)

####################################################################################################################################
# test_opaque_types_wood_nested_vector_int
####################################################################################################################################

from wood_pybind11 import test_opaque_types_wood_nested_vector_int
from wood_pybind11 import WoodNestedVectorInt

# this is std::vector<std::vector<int>>
list_of_integers_0 = WoodVectorInt([0, 1, 2])
list_of_integers_1 = WoodVectorInt([3])
list_of_integers_2 = WoodVectorInt([6, 7])
nested_list_of_integers = WoodNestedVectorInt([list_of_integers_0, list_of_integers_1, list_of_integers_2])
test_opaque_types_wood_nested_vector_int(nested_list_of_integers)
print(nested_list_of_integers[0], nested_list_of_integers[1], nested_list_of_integers[2])

####################################################################################################################################
# test_opaque_types_wood_vector_double
####################################################################################################################################


from wood_pybind11 import test_opaque_types_wood_vector_double
from wood_pybind11 import WoodVectorDouble

# this is std::vector<double>
list_of_doubles = WoodVectorDouble([0.5, 1.4, 2.3])
test_opaque_types_wood_vector_double(list_of_doubles)
print(list_of_doubles)

####################################################################################################################################
# test_opaque_types_wood_nested_vector_double
####################################################################################################################################

from wood_pybind11 import test_opaque_types_wood_nested_vector_double
from wood_pybind11 import WoodNestedVectorDouble

# this is std::vector<std::vector<double>>
list_of_double_0 = WoodVectorDouble([0.5, 1.4, 2.3])
list_of_double_1 = WoodVectorDouble([3.4, 4.1, 4.1])
nested_list_of_doubles = WoodNestedVectorDouble([list_of_double_0, list_of_double_1])
test_opaque_types_wood_nested_vector_double(nested_list_of_doubles)
print(list_of_double_0, list_of_double_1)


