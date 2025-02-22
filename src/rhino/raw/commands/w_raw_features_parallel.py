#! python3
# venv: timber_connections

# TODO:

# Parallel Features
# Check Compas_wood project considering changes to compas_rui

# Publish raw to YAK
# Publish wood_nano, wood_rui to PyPi
# Publish compas_wood to YAK

# Custom Joints
# Check How to input varying axis


import Rhino
from wood_rui import process_input
from wood_rui import Element
from wood_rui import add_sub_layer
from System.Drawing import Color

#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element
from wood_rui import add_sub_layer
from System.Drawing import Color



def closest_curves(
    curves: list[Rhino.Geometry.NurbsCurve],
    radii : list[float],
    distance : float,
    display_closest_line_segments : bool
) -> None:


    ###############################################################################
    # Run the closest point search
    ###############################################################################
    bboxes = []
    for idx, c in enumerate(curves):
        bbox : Rhino.Geometry.BoundingBox = c.GetBoundingBox(True)
        bbox.Inflate(radii[idx] + 0.02)
        bboxes.append(bbox)


    # create a tree
    rtree = Rhino.Geometry.RTree()

    # fill the tree
    for idx, bbox in enumerate(bboxes):
        rtree.Insert(bbox, idx)

    # call_backs of rtree
    def search_callback(sender, rtree_event_args):
        data_by_reference.append(rtree_event_args.Id)

    neighbours : list[list[int, int]] = []

    # perform search
    for i in range(len(bboxes)):
        data_by_reference = []
        if rtree.Search(
            bboxes[i], search_callback, data_by_reference
        ):
            for j in data_by_reference:
                if j < i:
                    continue
                result, p0, p1 = curves[i].ClosestPoints(curves[j])

                if(distance < p0.DistanceTo(p1)):
                    neighbours.append([i, j])
                    if display_closest_line_segments:
                        line = Rhino.Geometry.Line(p0, p1)
                        Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line)
                    

    print(neighbours)

    ###############################################################################
    # Output
    ###############################################################################
    return



def callback(selection: dict[str, any], name: str):
    if not selection["elements"]:
        return
    
    # Extract axes and radii
    curves = []
    radii = []
    for idx, element in enumerate(selection["elements"]):
        curves.append(element.axes[0].ToNurbsCurve())
        radii.append(max(element.radii[0]))

    # Run closest curve search 
    closest_curves(curves, radii, 25)





        # add_sub_layer(
        #     element.geometry_plane[0],
        #     "axes",
        #     axes_curves,
        #     [Color.FromArgb(255, 30, 144, 255)],
        #     idx == 0,
        #     True,
        # )


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
        "features": ([], list[Rhino.Geometry.Brep]),
    }

    process_input(selection_types, callback)

