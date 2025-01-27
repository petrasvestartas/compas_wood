#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_skeleton
from compas_wood.binding_rhino import beam_skeleton, mesh_skeleton
from System.Drawing import Color
import System

def my_callback(string_to_tuplesofitemsandtypes, dataset_name):

    wood_rui_globals["mesh_skeleton"]

    for id, polyline_guid in enumerate(wood_rui_globals["mesh_skeleton"]["polylines_guid"]):

       

        # Update the polyline location
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(polyline_guid)
        if not obj:
            print("NOTE: Object was deleted or invalidated.")
            continue
        wood_rui_globals["mesh_skeleton"]["polylines"][id] = obj.CurveGeometry.TryGetPolyline()[1]

        string_vertices = obj.Attributes.GetUserString("vertices")
        string_faces = obj.Attributes.GetUserString("faces")

        if string_vertices:
            vertex_numbers = [float(num) for num in string_vertices.split(',') if num]
            vertices = []
            for i in range(0, len(vertex_numbers), 3):
                vertices.append(Rhino.Geometry.Point3d(vertex_numbers[i], vertex_numbers[i+1], vertex_numbers[i+2]))

        if string_faces:
            face_numbers = [int(num) for num in string_faces.split(',') if num]
            faces = []
            for i in range(0, len(face_numbers), 3):
                faces.append(Rhino.Geometry.MeshFace(face_numbers[i], face_numbers[i+1], face_numbers[i+2]))

        # Now you can use the 'vertices' and 'faces' lists to reconstruct the mesh
        mesh = Rhino.Geometry.Mesh()
        for vertex in vertices:
            mesh.Vertices.Add(vertex)
        for face in faces:
            mesh.Faces.AddFace(face)

        # Reverse process: Convert string back to Rhino transform
        string_transformation = obj.Attributes.GetUserString("transform")
        if string_transformation:
            number_strings = string_transformation.split(',')
            transformation_numbers = [float(num) for num in number_strings if num]
            if len(transformation_numbers) == 16:
                T0 = Rhino.Geometry.Transform()
                T0.M00, T0.M01, T0.M02, T0.M03 = transformation_numbers[0:4]
                T0.M10, T0.M11, T0.M12, T0.M13 = transformation_numbers[4:8]
                T0.M20, T0.M21, T0.M22, T0.M23 = transformation_numbers[8:12]
                T0.M30, T0.M31, T0.M32, T0.M33 = transformation_numbers[12:16]
   

        # Transformation from XY to Polyline
        polyline = wood_rui_globals["mesh_skeleton"]["polylines"][id]
        z_axis = polyline[polyline.Count-2] - polyline[0]
        x_axis = polyline[polyline.Count-1] - polyline[polyline.Count-2]
        plane_xy = Plane.WorldXY
        plane = Plane(polyline[polyline.Count-1], x_axis, Rhino.Geometry.Vector3d.CrossProduct(x_axis, z_axis))
        T1 = Rhino.Geometry.Transform.PlaneToPlane(plane_xy, plane)

        
        mesh.Transform(T1*T0)

        Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(mesh)
        # Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(polyline)





if __name__ == "__main__":

    dataset_name = "mesh_skeleton"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {

    }


   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback)


