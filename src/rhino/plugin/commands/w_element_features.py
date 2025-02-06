#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane
)

def get_features(geometry_planes):

    for geometry_plane in geometry_planes:

        # Collect information from user strings
        string_dictionary = geometry_plane[0].Attributes.GetUserStrings()
        T = Rhino.Geometry.Transform.PlaneToPlane(Rhino.Geometry.Plane.WorldXY, geometry_plane[1])
        for key in string_dictionary:
            if "feature" in key:
                value = string_dictionary[key]  # GetValues returns a list of values for the key
                brep = Rhino.Geometry.Brep.FromJSON(value)
                brep.Transform(T)
                Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(brep)

        # Redraw the document to reflect changes
        Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

if __name__ == "__main__":
    geometry_planes = select_and_find_valid_groups()
    get_features(geometry_planes)


