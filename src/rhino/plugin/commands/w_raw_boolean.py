#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element
from wood_rui import add_sub_layer
from System.Drawing import Color


def callback(selection: dict[str, any], name: str):
    if not selection["elements"]:
        return

    for idx, element in enumerate(selection["elements"]):
        element.boolean_difference()


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
    }

    process_input(selection_types, callback)
