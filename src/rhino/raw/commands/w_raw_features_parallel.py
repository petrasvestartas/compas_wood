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

    # Run closest curve search
    neighbours, planes, lines = Element.closest_axis(
        selection["elements"],
        selection["distance"],
        selection["display_plane"],
        selection["display_line"],
    )

    ####################################################################################################
    # Collect Neighbour Information
    ####################################################################################################
    neighbours_for_user_data = {}
    features = {}

    # Explicit loop for initialization
    for idx in range(len(selection["elements"])):
        neighbours_for_user_data[idx] = []
        features[idx] = []

    # Populate neighbour relationships
    for pair in neighbours:
        a, b = pair  # Explicit unpacking for clarity
        neighbours_for_user_data[a].append(b)
        neighbours_for_user_data[b].append(a)

    # Assign indices and sorted neighbours
    for idx, element in enumerate(selection["elements"]):
        element.index = idx
        element.neighbours = sorted(neighbours_for_user_data[idx])

    ####################################################################################################
    # Collect breps into the features dictionary
    ####################################################################################################
    for pair, target_plane in zip(neighbours, planes):
        element_pair = [selection["elements"][pair[0]], selection["elements"][pair[1]]]
        breps_pair = [selection["features_positive"], selection["features_negative"]]

        T = Rhino.Geometry.Transform.PlaneToPlane(
            Rhino.Geometry.Plane.WorldXY, target_plane
        )

        for element, breps in zip(element_pair, breps_pair):
            breps_copies = []
            for brep in breps:
                breps_copies.append(brep.DuplicateBrep())
                breps_copies[-1].Transform(T)
            features[element.index].extend(breps_copies)

    ####################################################################################################
    # Assign features to the elements
    ####################################################################################################
    for idx, element in enumerate(selection["elements"]):
        new_features = (
            features[element.index]
            if selection["replace_or_extend_features"]
            else features[element.index] + element.features
        )

        # Display must be in this position, because setter of features transforms BReps
        if selection["display_features"]:
            add_sub_layer(
                element.geometry_plane[0],
                "features",
                new_features,
                [Color.FromArgb(0, 0, 255)],
                idx == 0,
                True,
            )

        element.features = new_features


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
        "features_positive": ([], list[Rhino.Geometry.Brep]),
        "features_negative": ([], list[Rhino.Geometry.Brep]),
        "distance": (150, float),
        "display_plane": (False, bool),
        "display_line": (False, bool),
        "display_features": (False, bool),
        "replace_or_extend_features": (True, bool),
    }

    process_input(selection_types, callback, hide_input=False)
