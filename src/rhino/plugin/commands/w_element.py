#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_element
import System

def my_callback(string_to_tuplesofitemsandtypes, dataset_name):

    if not string_to_tuplesofitemsandtypes["element"][0]:
        return
    
    ###############################################################################
    # Add these properties as strings
    ###############################################################################
    add_element(string_to_tuplesofitemsandtypes["element"][0][0], "element", string_to_tuplesofitemsandtypes["cuts"][0], "compas_wood")


if __name__ == "__main__":

    dataset_name = "beamshape"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "cuts": ([], List[Rhino.Geometry.Mesh]),
        "element": ([], List[Rhino.Geometry.Mesh]),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False)


