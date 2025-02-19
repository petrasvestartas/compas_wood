#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, Element

def my_callback(input_dict, dataset_name):

    if not input_dict["shape"]:
        return


    Element.add_element(
        shape = input_dict["shape"][0],
        name = input_dict["name"],
        features = input_dict["features"], 
        axes = input_dict["axes"], 
        radii = input_dict["radii"], 
        thickness = input_dict["thickness"], 
        insertion = input_dict["insertion"], 
        joint_types = input_dict["joint_types"], 
    )


if __name__ == "__main__":

    dataset_name = "beam"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "shape" : ([], List[Rhino.Geometry.Brep]),
        "name" : (["beam", "plate"], List[str]),
        "features" : ([], List[Rhino.Geometry.Brep]), 
        "axes": ([], List[Rhino.Geometry.Polyline]),
        "radii": ([], List[float]),
        "thickness": ([], List[float]),
        "insertion": ([], List[Rhino.Geometry.Line]), #
        "joint_types": ([], List[Rhino.Geometry.TextDot]),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False, True)


