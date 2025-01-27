#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_skeleton

def my_callback(string_to_tuplesofitemsandtypes, dataset_name):


    polylines = []
    distances = []
    meshes = []
    transforms = []

    if len(string_to_tuplesofitemsandtypes["meshes"][0]) > 0:
        if len(string_to_tuplesofitemsandtypes["meshes"][0]) != len(string_to_tuplesofitemsandtypes["polylines"][0]):
            raise Exception("Number of polylines and meshes must be equal")

    for id, mesh in enumerate(string_to_tuplesofitemsandtypes["meshes"][0]):

        mesh.Faces.ConvertQuadsToTriangles()

        polyline = string_to_tuplesofitemsandtypes["polylines"][0][id]
        distance = string_to_tuplesofitemsandtypes["radius"][0]

        polylines.append(polyline)
        axis_distances = []
        for i in range(polyline.Count):
            axis_distances.append(distance)
        distances.append(axis_distances)
        meshes.append(mesh)

        # Add a small segment at the end of the polyline to provide orientation
        z_axis = polyline[polyline.Count-1] - polyline[0]
        plane = Plane(polyline[polyline.Count-1], z_axis)
        x_axis = plane.XAxis
        polyline.Add(polyline[polyline.Count-1] + x_axis)
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
        "polylines": ([], List[Rhino.Geometry.Polyline]),
        "meshes": ([], List[Rhino.Geometry.Mesh]),
        "radius": (100, float)
    }


   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback)


