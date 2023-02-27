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


# compas_wood.test()
wood_pybind11.test()
