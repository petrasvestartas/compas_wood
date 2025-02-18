#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, Element
import System

def my_callback(string_to_tuplesofitemsandtypes, dataset_name):

    if not string_to_tuplesofitemsandtypes["geometry"]:
        return

    Element.add_element(
        # layer
        layer_name = "unknown",
        element_type = string_to_tuplesofitemsandtypes["element_type"],
        # geometry, plates or beams, use solids
        geometry = string_to_tuplesofitemsandtypes["geometry"][0],
        # features
        features = string_to_tuplesofitemsandtypes["features"],
        insertion = string_to_tuplesofitemsandtypes["insertion"],
        joints_types = string_to_tuplesofitemsandtypes["joints_types"],
        # simplified geometry
        axes = string_to_tuplesofitemsandtypes["axes"],
        radii = string_to_tuplesofitemsandtypes["radii"],
        thickness = string_to_tuplesofitemsandtypes["thickness"],
        # graph
        index = string_to_tuplesofitemsandtypes["index"],
        neighbours = string_to_tuplesofitemsandtypes["neighbours"],
        # tree
        parent = ""
    )


if __name__ == "__main__":

    

    dataset_name = "beamshape"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        # layer
        # "layer_name": ([], "unknown"),
        # geomet-ry, plates or beams, use solids
        "element_type": (["beam", "plate"], List[str]), #
        "geometry": ([], List[Rhino.Geometry.Brep]), #
        # features
        "features": ([], List[Rhino.Geometry.Brep]), #
        "insertion": ([], List[Rhino.Geometry.Line]), #
        "joints_types": ([], List[Rhino.Geometry.TextDot]),
        # simplified geometry
        "axes": ([], List[Rhino.Geometry.Polyline]), #
        "radii": ([], List[float]), #
        "thickness": ([], List[float]), #
        # graph
        "index": (-1, int),
        "neighbours": ([], List[int]), #
        # tree
        # "parent": (str, ""),
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False, True)


