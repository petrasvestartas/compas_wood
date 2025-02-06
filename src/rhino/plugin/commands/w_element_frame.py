#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane
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
        line_x = Rhino.Geometry.Line(p0, p1)
        line_y = Rhino.Geometry.Line(p0, p2)
        line_z = Rhino.Geometry.Line(p0, p3)

        # Create attributes for colors
        attr_x = Rhino.DocObjects.ObjectAttributes()
        attr_x.ObjectColor = System.Drawing.Color.Red
        attr_x.ColorSource = Rhino.DocObjects.ObjectColorSource.ColorFromObject

        attr_y = Rhino.DocObjects.ObjectAttributes()
        attr_y.ObjectColor = System.Drawing.Color.Green
        attr_y.ColorSource = Rhino.DocObjects.ObjectColorSource.ColorFromObject

        attr_z = Rhino.DocObjects.ObjectAttributes()
        attr_z.ObjectColor = System.Drawing.Color.Blue
        attr_z.ColorSource = Rhino.DocObjects.ObjectColorSource.ColorFromObject

        # Add lines with attributes
        id0 = Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line_x, attr_x)
        id1 = Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line_y, attr_y)
        id2 = Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line_z, attr_z)
        ids = [id0, id1, id2]

        # Group all arrows
        if all(ids):
            group_index = Rhino.RhinoDoc.ActiveDoc.Groups.Add()
            for arrow_id in ids:
                Rhino.RhinoDoc.ActiveDoc.Groups.AddToGroup(group_index, arrow_id)

        # Redraw the document to reflect changes
        Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

        print(plane)

if __name__ == "__main__":
    geometry_planes = select_and_find_valid_groups("Elements")
    vizualize_group_plane(geometry_planes)


