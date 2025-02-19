#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, Element, add_sub_layer
import System

def my_callback(input_dict, dataset_name):

    if not input_dict["element"]:
        return

    for idx, element in enumerate(input_dict["element"]):

        if len(input_dict["replace"])>0:
            element.features = input_dict["replace"]

        if len(input_dict["add"])>0:
            features = element.features
            features.extend(input_dict["add"])
            element.features = features

        if input_dict["get"]:
            add_sub_layer(element.geometry_plane[0], "features", element.features, [System.Drawing.Color.FromArgb(0,0,255)], idx==0)
        
        if input_dict["clear"]:
            element.clear_features()


if __name__ == "__main__":

    dataset_name = "beam"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "element" : ([], List[Element]),
        "add" : ([], List[Rhino.Geometry.Brep]),
        "replace" : ([], List[Rhino.Geometry.Brep]),
        "get" : (False, bool),
        "clear" : (False, bool),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False, True)

