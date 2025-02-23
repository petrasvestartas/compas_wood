#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import add_sub_layer
from wood_rui import Element
from System.Drawing import Color


def callback(selection: dict[str, any], name: str):
    if len(selection["elements"]) == 0:
        print("Attention: no shape or elements selected!")
        return

    for idx, element in enumerate(selection["elements"]):
        scale = max(0.01, selection["scale"])
        plane_axes: list[Rhino.Geometry.NurbsCurve] = element.plane_axes(scale)
        add_sub_layer(
            element.geometry_plane[0],
            "plane",
            plane_axes,
            [Color.Red, Color.Green, Color.Blue],
            idx == 0,
            True,
        )


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
        "scale": (200, float),
    }

    process_input(selection_types, callback)
