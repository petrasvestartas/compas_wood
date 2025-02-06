#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, handle_solid_input, polyline_obj_to_plane, handle_polylines_input, polyline_to_plane
)

def orient(geometry_planes, plane=Rhino.Geometry.Plane.WorldXY):

    # replace the geometry and the plane

    # if len(geometry_planes) != len(replacement_geometries):
    #     print("Number of elements and replacement geometries are not the same.")
    #     return

    for idx, geometry_plane in enumerate(geometry_planes):
       
        T = Rhino.Geometry.Transform.PlaneToPlane(polyline_obj_to_plane(geometry_plane[1]), plane)
        geometry = geometry_plane[0].Geometry
        polyline = geometry_plane[1].Geometry
        geometry.Transform(T)
        polyline.Transform(T)
        Rhino.RhinoDoc.ActiveDoc.Objects.Replace(geometry_plane[0].Id, geometry)
        Rhino.RhinoDoc.ActiveDoc.Objects.Replace(geometry_plane[1].Id, polyline)
    Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

if __name__ == "__main__":
    
    geometry_planes = select_and_find_valid_groups("Elements")
    rectangles = handle_polylines_input("Rectangle, if you want to orient to WorldXY, press Enter.")

    plane = Rhino.Geometry.Plane.WorldXY
    for rect in rectangles:
        plane=polyline_to_plane(rect)
        break
    print(plane)

    orient(geometry_planes, plane)


