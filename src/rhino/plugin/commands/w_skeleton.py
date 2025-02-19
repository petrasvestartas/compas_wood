#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_skeleton
from compas_wood.binding_rhino import beam_skeleton, mesh_skeleton
import System

def my_callback(string_to_tuplesofitemsandtypes, dataset_name):



    polylines = []
    distances = []
    meshes = []
    transforms = []

    for mesh in string_to_tuplesofitemsandtypes["meshes"]:
        
        # polyline = mesh_skeleton(mesh)

        polyline, distance = beam_skeleton(
            mesh,
            string_to_tuplesofitemsandtypes["divisions"],
            string_to_tuplesofitemsandtypes["nearest_neighbors"],
            string_to_tuplesofitemsandtypes["extend_end"])
        polylines.append(polyline)
        distances.append(distance)
        meshes.append(mesh)


        # Add a small segement at the end of the polyline to provide orientation
        z_axis = polyline[polyline.Count-1] - polyline[0]
        plane = Plane(polyline[polyline.Count-1], z_axis)
        x_axis = plane.XAxis
        polyline.Add(polyline[polyline.Count-1] + x_axis*0.1)
        plane_xy = Plane.WorldXY
        plane = Plane(polyline[polyline.Count-1], x_axis, Rhino.Geometry.Vector3d.CrossProduct(x_axis, z_axis))
        xform = Rhino.Geometry.Transform.PlaneToPlane(plane, plane_xy)
        transforms.append(xform)

    # Add Object to Rhino
    add_skeleton(polylines, "mesh_skeleton", distances, meshes, transforms)




if __name__ == "__main__":

    dataset_name = "mesh_skeleton"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "meshes": ([], List[Rhino.Geometry.Mesh]),
        "divisions": (10, int),
        "nearest_neighbors": (10, int),
        "extend_end" : (True, bool)
    }


   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback)


