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
from wood_pybind11 import WoodNestedVectorInt
from wood_pybind11 import rtree


polylines_coordinates = WoodNestedVectorDouble()
polylines_coordinates.append(
    WoodVectorDouble(
        [
            -33.5860325465656,
            119.739019322598,
            0,
            -104.491745217617,
            97.3466084012926,
            0,
            -91.2513983854723,
            172.439794038809,
            0,
            -33.5860325465656,
            119.739019322598,
            0,
        ]
    )
)

polylines_coordinates.append(
    WoodVectorDouble(
        [
            -33.5860325465656,
            119.739019322598,
            11.3969019659298,
            -104.491745217617,
            97.3466084012926,
            11.3969019659298,
            -91.2513983854723,
            172.439794038809,
            11.3969019659298,
            -33.5860325465656,
            119.739019322598,
            11.3969019659298,
        ]
    )
)

polylines_coordinates.append(
    WoodVectorDouble(
        [
            -163.46337575293,
            142.638847783606,
            0,
            -104.491745217617,
            97.3466084012926,
            0,
            -91.2513983854723,
            172.439794038809,
            0,
            -163.46337575293,
            142.638847783606,
            0,
        ]
    )
)

polylines_coordinates.append(
    WoodVectorDouble(
        [
            -163.46337575293,
            142.638847783606,
            11.3969019659298,
            -104.491745217617,
            97.3466084012926,
            11.3969019659298,
            -91.2513983854723,
            172.439794038809,
            11.3969019659298,
            -163.46337575293,
            142.638847783606,
            11.3969019659298,
        ]
    )
)

# void rtree(
#     std::vector<std::vector<double>> &polylines_coordinates,
#     std::vector<std::vector<int>> elements_neigbhours,
#     std::vector<std::vector<double>> elements_AABB,
#     std::vector<std::vector<double>> elements_OOBB);


elements_neighbours = WoodNestedVectorInt()
elements_AABB = WoodNestedVectorDouble()
elements_OOBB = WoodNestedVectorDouble()

rtree(polylines_coordinates, elements_neighbours, elements_AABB, elements_OOBB)

print(elements_neighbours[0])
print(elements_AABB[0])
print(elements_OOBB[0])
