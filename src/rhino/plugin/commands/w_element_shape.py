#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, Element, add_sub_layer
import System

def my_callback(input_dict, dataset_name):

    if not input_dict["elements"]:
        return
    
    elements : list[Element] = input_dict["elements"]

    for idx, element in enumerate(elements):
        element.shape = input_dict["shape"][idx%len(input_dict["shape"])]
        

if __name__ == "__main__":

    dataset_name = "beam"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "elements" : ([], List[Element]),
        "shape" : ([], List[Rhino.Geometry.Brep]),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False, False)