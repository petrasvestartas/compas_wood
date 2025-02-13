#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane, generalized_input_method, add_sub_layer
)

def string_to_xyz(s):
    nums = list(map(float, s.split(',')))
    points = [Rhino.Geometry.Point3d(nums[i], nums[i+1], nums[i+2]) for i in range(0, len(nums), 3)]
    return points

def get_features(geometry_planes):

    for geometry_plane in geometry_planes:

        # Collect information from user strings
        string_dictionary = geometry_plane[0].Attributes.GetUserStrings()
        T = Rhino.Geometry.Transform.PlaneToPlane(Rhino.Geometry.Plane.WorldXY, polyline_obj_to_plane(geometry_plane[1]))

        for key in string_dictionary:

            if "axis" in key:
                # Get axis and plane
                value = string_dictionary[key]  # GetValues returns a list of values for the key
                points = string_to_xyz(value)
                axis = Rhino.Geometry.Polyline(points).ToNurbsCurve()
                axis.Transform(T)
                add_sub_layer(geometry_plane[0], "axis", [axis], [System.Drawing.Color.FromArgb(0,0,255)], False)


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


