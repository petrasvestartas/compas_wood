#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element
from System.Drawing import Color


def callback(selection: dict[str, any], name: str):
    if not selection["elements"] or not selection["shape"]:
        print("Attention: no shape or elements selected!")
        return

    num_elements = len(selection["elements"])
    num_shapes = len(selection["shape"])

    for idx, element in enumerate(selection["elements"]):
        element.shape = selection["shape"][idx % num_shapes]  # Cycle through shapes
    
    return


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
        "shape": ([], list[Rhino.Geometry.Brep]),
    }

    process_input(selection_types, callback, hide_input=False)
