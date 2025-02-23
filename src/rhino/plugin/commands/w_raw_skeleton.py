#! python3
# venv: timber_connections
import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, process_input, add_skeleton
from compas_wood.binding_rhino import beam_skeleton, mesh_skeleton
import System


def callback(selection: dict[str, any], name: str):
    # Just axis without radii
    if not selection["simplify"]:
        for mesh in selection["meshes"]:
            polylines = mesh_skeleton(mesh)
            for polyline in polylines:
                Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(polyline)
        return

    # Try to simplify an axis

    polylines = []
    distances = []
    meshes = []
    transforms = []

    for mesh in selection["meshes"]:
        polyline = None
        polylines = []

        polyline, distance = beam_skeleton(
            mesh,
            selection["divisions"],
            selection["nearest_neighbors"],
            selection["extend_end"],
        )
        polylines.append(polyline)
        distances.append(distance)
        meshes.append(mesh)

        # Add a small segement at the end of the polyline to provide orientation
        z_axis = polyline[polyline.Count - 1] - polyline[0]
        plane = Plane(polyline[polyline.Count - 1], z_axis)
        x_axis = plane.XAxis
        polyline.Add(polyline[polyline.Count - 1] + x_axis * 0.1)
        plane_xy = Plane.WorldXY
        plane = Plane(
            polyline[polyline.Count - 1],
            x_axis,
            Rhino.Geometry.Vector3d.CrossProduct(x_axis, z_axis),
        )
        xform = Rhino.Geometry.Transform.PlaneToPlane(plane, plane_xy)
        transforms.append(xform)

    add_skeleton(polylines, "mesh_skeleton", distances, meshes, transforms)


if __name__ == "__main__":
    input_dict = {
        "meshes": ([], list[Rhino.Geometry.Mesh]),
        "simplify": (False, bool),
        "divisions": (10, int),
        "nearest_neighbors": (10, int),
        "extend_end": (True, bool),
    }

    # Call the generalized input method with the dataset name and input dictionary
    process_input(input_dict, callback, hide_input=False)
