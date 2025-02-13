#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane, add_sub_layer
)

def get_features(geometry_planes):

    for geometry_plane in geometry_planes:

        # Collect information from user strings
        string_dictionary = geometry_plane[0].Attributes.GetUserStrings()
        T = Rhino.Geometry.Transform.PlaneToPlane(Rhino.Geometry.Plane.WorldXY, polyline_obj_to_plane(geometry_plane[1]))

        print(string_dictionary)
        geometries = []
        for key in string_dictionary:
            if "feature" in key:
                value = string_dictionary[key]  # GetValues returns a list of values for the key
                geometry = Rhino.Geometry.GeometryBase.FromJSON(value)
                geometry.Transform(T)
                geometries.append(geometry)
        add_sub_layer(geometry_plane[0], "features", geometries, [System.Drawing.Color.FromArgb(0,0,255)], False)

        # Redraw the document to reflect changes
        Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

def remove_features():
    pass

def add_features():
    pass

def update_features():
    pass

if __name__ == "__main__":
    geometry_planes = select_and_find_valid_groups("Elements")




    get_features(geometry_planes)


