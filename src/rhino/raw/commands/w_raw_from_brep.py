#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element


def callback(selection: dict[str, any], name: str):
    if not selection["shape"]:
        print("Attention: no shape selected!")
        return

    Element.add_element(
        shape=selection["shape"][0],
        name="shape",
        features=selection["features"],
        axes=selection["axes"],
        radii=selection["radii"],
        thickness=selection["thickness"],
        insertion=selection["insertion"],
        joint_types=selection["joint_types"],
    )


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "shape": ([], list[Rhino.Geometry.Brep]),
        "name": (["beam", "plate"], list[str]),
        "features": ([], list[Rhino.Geometry.Brep]),
        "axes": ([], list[Rhino.Geometry.Polyline]),
        "radii": ([], list[float]),
        "thickness": ([], list[float]),
        "insertion": ([], list[Rhino.Geometry.Line]),
        "joint_types": ([], list[Rhino.Geometry.TextDot]),
    }

    process_input(selection_types, callback)
