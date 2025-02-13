#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, handle_solid_input, polyline_obj_to_plane
)

def get_features(geometry_planes, replacement_geometries):

    if len(geometry_planes) != len(replacement_geometries):
        print("Number of elements and replacement geometries are not the same.")
        return

    for idx, geometry_plane in enumerate(geometry_planes):
        T = Rhino.Geometry.Transform.PlaneToPlane(Rhino.Geometry.Plane.WorldXY, polyline_obj_to_plane(geometry_plane[1]))
        replacement_geometries[idx].Transform(T)
        Rhino.RhinoDoc.ActiveDoc.Objects.Replace(geometry_plane[0].Id, replacement_geometries[idx])
    Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

if __name__ == "__main__":
    
    geometry_planes = select_and_find_valid_groups("Elements")
    breps = handle_solid_input("Solids")
    get_features(geometry_planes, breps)


