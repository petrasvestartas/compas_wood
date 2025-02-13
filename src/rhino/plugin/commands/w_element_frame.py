#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane, add_sub_layer
)

def vizualize_group_plane(geometry_planes):

    for geometry_plane in geometry_planes:

        # Define points
        plane = polyline_obj_to_plane(geometry_plane[1])
        p0 = plane.Origin
        p1 = plane.Origin + plane.XAxis * 5
        p2 = plane.Origin + plane.YAxis * 5
        p3 = plane.Origin + plane.ZAxis * 5

        # Create lines
        line_x = Rhino.Geometry.Line(p0, p1).ToNurbsCurve()
        line_y = Rhino.Geometry.Line(p0, p2).ToNurbsCurve()
        line_z = Rhino.Geometry.Line(p0, p3).ToNurbsCurve()

        add_sub_layer(geometry_plane[0], "frame", [line_x, line_y, line_z], [System.Drawing.Color.Red, System.Drawing.Color.Green, System.Drawing.Color.Blue], False)

        print(plane)

if __name__ == "__main__":
    geometry_planes = select_and_find_valid_groups("Elements")
    vizualize_group_plane(geometry_planes)


