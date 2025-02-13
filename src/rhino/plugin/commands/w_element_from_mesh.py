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

    if not string_to_tuplesofitemsandtypes["geometry"][0]:
        return

    add_element(
        # layer
        layer_name = "unknown",
        # geometry, plates or beams, use solids
        geometry =  string_to_tuplesofitemsandtypes["geometry"][0][0],
        # features
        features = string_to_tuplesofitemsandtypes["features"][0],
        insertion_lines = string_to_tuplesofitemsandtypes["insertion_lines"][0],
        joints_types = string_to_tuplesofitemsandtypes["joints_types"][0],
        insertion_vectors = string_to_tuplesofitemsandtypes["insertion_vectors"][0],
        # simplified geometry
        thickness = string_to_tuplesofitemsandtypes["thickness"][0],
        simplified_shape = string_to_tuplesofitemsandtypes["simplified_shape"][0],
        # graph
        index = string_to_tuplesofitemsandtypes["index"][0],
        neighbours = string_to_tuplesofitemsandtypes["neighbours"][0],
        # tree
        parent = ""
    )


if __name__ == "__main__":

    dataset_name = "beamshape"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        # layer
        # "layer_name": ([], "unknown"),
        # geometry, plates or beams, use solids
        "geometry": ([], List[Rhino.Geometry.Mesh]), #
        # features
        "features": ([], List[Rhino.Geometry.Mesh]), #
        "insertion_lines": ([], List[Rhino.Geometry.Line]), #
        "joints_types": ([], List[Rhino.Geometry.TextDot]),
        "insertion_vectors": ([], List[Rhino.Geometry.Line]), #
        # simplified geometry
        "thickness": (10.0, List[float]), #
        "simplified_shape": ([], List[Rhino.Geometry.Polyline]), #
        # graph
        "index": (-1, int),
        "neighbours": ([], List[int]), #
        # tree
        # "parent": (str, ""),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False)


