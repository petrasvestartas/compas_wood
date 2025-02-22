#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element
from wood_rui import add_sub_layer
from System.Drawing import Color


def callback(selection: dict[str, any], name: str):
    if not selection["element"]:
        return

    for idx, element in enumerate(selection["element"]):
        if len(selection["replace"]) > 0:
            element.features = input_dict["replace"]

        if len(selection["add"]) > 0:
            features = element.features
            features.extend(selection["add"])
            element.features = features

        if selection["get"]:
            add_sub_layer(
                element.geometry_plane[0],
                "features",
                element.features,
                [Color.FromArgb(0, 0, 255)],
                idx == 0,
            )

        if selection["clear"]:
            element.clear_features()

    return

if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "element": ([], list[Element]),
        "add": ([], list[Rhino.Geometry.Brep]),
        "replace": ([], list[Rhino.Geometry.Brep]),
        "get": (False, bool),
        "clear": (False, bool),
    }

    process_input(selection_types, callback)
