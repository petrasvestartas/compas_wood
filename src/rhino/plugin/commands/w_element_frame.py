#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_skeleton
from System.Drawing import Color
import System


def retrieve_plane_from_object():

    object_type = Rhino.DocObjects.ObjectType.Mesh
    rc, objrefs = Rhino.Input.RhinoGet.GetMultipleObjects("Select meshes", False, object_type)
    if rc != Rhino.Commands.Result.Success:
        return

    for objref in objrefs:
        obj = objref.Object()
        if obj:
            
            transform_str = obj.Attributes.GetUserString("ObjectFrame")
            print(transform_str)
            if not transform_str:
                return None

                # Convert back to transform matrix
                transform_values = list(map(float, transform_str.split(";")))
                transform = Rhino.Geometry.Transform(4, 4)
                transform.SetFromFloatArray(transform_values, True)

                # Reconstruct the plane
                plane = Rhino.Geometry.Plane.WorldXY
                plane.Transform(transform)

                # Define points
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
                
                return plane

def TestGetObjectFrame():
    object_type = Rhino.DocObjects.ObjectType.Mesh
    rc, objrefs = Rhino.Input.RhinoGet.GetMultipleObjects("Select meshes", False, object_type)
    if rc != Rhino.Commands.Result.Success:
        return

    for objref in objrefs:
        obj = objref.Object()
        if obj:
            # Define points
            p0 = obj.Attributes.ObjectFrame().Origin
            p1 = obj.Attributes.ObjectFrame().Origin + obj.Attributes.ObjectFrame().XAxis * 5
            p2 = obj.Attributes.ObjectFrame().Origin + obj.Attributes.ObjectFrame().YAxis * 5
            p3 = obj.Attributes.ObjectFrame().Origin + obj.Attributes.ObjectFrame().ZAxis * 5

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
            print(obj.Attributes.ObjectFrame())

if __name__ == "__main__":

    retrieve_plane_from_object()

