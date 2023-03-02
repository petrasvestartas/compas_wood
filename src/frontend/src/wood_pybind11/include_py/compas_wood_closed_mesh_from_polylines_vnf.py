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
from wood_pybind11 import WoodVectorFloat
from wood_pybind11 import WoodVectorInt
from wood_pybind11 import closed_mesh_from_polylines_vnf


list_of_polylines = WoodNestedVectorDouble()
list_of_polylines.append(
    WoodVectorDouble(
        [
            -4.2603858296613,
            -1.51051505440138,
            0,
            -11.7046107290298,
            10.1201039242795,
            0,
            3.83430852935093,
            18.3850937565516,
            0,
            5.30530796121568,
            4.29591981594295,
            0,
            -3.59522189874489,
            7.78606412056844,
            0,
            -4.2603858296613,
            -1.51051505440138,
            0,
        ]
    )
)

list_of_polylines.append(
    WoodVectorDouble(
        [
            -3.60737818541092,
            -1.51051505440138,
            0.802311713703479,
            -11.0516030847794,
            10.1201039242795,
            0.802311713703479,
            4.48731617360132,
            18.3850937565516,
            0.802311713703479,
            5.95831560546607,
            4.29591981594295,
            0.802311713703479,
            -2.9422142544945,
            7.78606412056844,
            0.802311713703479,
            -3.60737818541092,
            -1.51051505440138,
            0.802311713703479,
        ]
    )
)

out_vertices = WoodVectorFloat()
out_normals = WoodVectorFloat()
out_triangles = WoodVectorInt()

closed_mesh_from_polylines_vnf(
    list_of_polylines, out_vertices, out_normals, out_triangles
)

print(out_vertices)
print("\n")
print(out_normals)
print("\n")
print(out_triangles)
print("\n")
