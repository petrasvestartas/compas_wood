import Rhino
from .globals import wood_rui_globals  # Import the singleton instance
from .layer import ensure_layer_exists
from System.Drawing import Color
from typing import *


def delete_objects(guids):
    """Delete Rhino objects by their GUIDs, handling individual items, lists, and nested lists."""
    if isinstance(guids, list):
        # If the input is a list, iterate through it and call delete_objects recursively
        for item in guids:
            delete_objects(item)
    else:
        # If the input is not a list, assume it's a single GUID and delete it
        Rhino.RhinoDoc.ActiveDoc.Objects.Delete(guids, True)


def add_polylines(polylines: List[Rhino.Geometry.Polyline], data_name: str):
    """Add a list of polylines to the specified layer and return their GUIDs.

    Parameters
    ----------
    foldername : List[Rhino.Geometry.Polyline]
        List of polylines.
    filename_of_dataset : str
        Name of dataset.

    """

    layer_index = ensure_layer_exists(data_name, "polylines", Color.Red)

    polyline_guids = []
    for polyline in polylines:
        if polyline:
            obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddCurve(polyline.ToNurbsCurve())
            if obj_guid:
                obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
                if obj:
                    obj.Attributes.LayerIndex = layer_index
                    obj.CommitChanges()
                    polyline_guids.append(obj_guid)
                else:
                    print(obj_guid, obj)
            else:
                print(obj_guid)

    if "polylines_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["polylines_guid"])
    wood_rui_globals[data_name]["polylines_guid"] = polyline_guids
    wood_rui_globals[data_name]["polylines"] = polylines


def add_insertion_lines(lines, data_name):
    """Add a list of polylines to the specified layer and return their GUIDs."""

    layer_index = ensure_layer_exists(data_name, "insertion", Color.Blue)

    line_guids = []
    for line in lines:
        obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line)
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
        obj.Attributes.LayerIndex = layer_index
        obj.CommitChanges()
        line_guids.append(obj_guid)

    if "insertion_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["insertion_guid"])
    wood_rui_globals[data_name]["insertion_guid"] = line_guids
    wood_rui_globals[data_name]["insertion"] = lines


def add_mesh(mesh, data_name):
    """Add a mesh to the specified layer and return the mesh's GUID."""

    layer_index = ensure_layer_exists(data_name, "mesh")

    if wood_rui_globals[data_name]["mesh_guid"] is not None:
        # Replace the mesh
        wood_rui_globals[data_name]["mesh"] = mesh
        Rhino.RhinoDoc.ActiveDoc.Objects.Replace(wood_rui_globals[data_name]["mesh_guid"], mesh)
    else:
        # Add the mesh
        wood_rui_globals[data_name]["mesh"] = mesh
        mesh_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(mesh)
        wood_rui_globals[data_name]["mesh_guid"] = mesh_guid
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(mesh_guid)
        obj.Attributes.LayerIndex = layer_index
        obj.CommitChanges()

    return wood_rui_globals[data_name]["mesh_guid"]


def add_adjacency(four_indices_face_face_edge_edge, data_name):
    wood_rui_globals[data_name]["adjacency"] = four_indices_face_face_edge_edge


def add_flags(flags, data_name):
    wood_rui_globals[data_name]["flags"] = flags


def add_joinery(joinery: List[List[Rhino.Geometry.Polyline]], data_name: str) -> None:
    """
    Add a list of lists of polylines as groups to the specified layer in Rhino and return their GUIDs.

    :param joinery: A list of lists of polylines to add to Rhino.
    :param data_name: The name of the case to be used for layer and group management.
    """

    # Use wood_rui_globals.plugin_name as the top-level layer
    plugin_name = wood_rui_globals.plugin_name

    # Ensure the layer exists or create it under the plugin layer
    layer_index = ensure_layer_exists(data_name, "joinery", Color.Black)

    # Prepare to store all the GUIDs of added objects
    joinery_guids = []

    # Loop through the joinery list (list of lists of polylines)
    for polyline_group in joinery:
        # Store the GUIDs of the individual polylines in this group
        group_guids = []

        # Loop through individual polylines in the current group
        for polyline in polyline_group:
            # Add the polyline to the Rhino document
            obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(polyline)
            obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
            obj.Attributes.LayerIndex = layer_index  # Assign to the specified layer
            obj.CommitChanges()  # Commit the changes to the object's attributes
            group_guids.append(obj_guid)  # Collect the GUID for this polyline

        # If the group contains polylines, group them together in Rhino
        if group_guids:
            group_name = f"{data_name}_joinery_group"
            Rhino.RhinoDoc.ActiveDoc.Groups.Add(group_guids)  # Group the polyline GUIDs together
            joinery_guids.extend(group_guids)  # Add the group's GUIDs to the master list

    # If the case already has "joinery_guid", delete the previous objects
    if "joinery_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["joinery_guid"])

    # Store the new joinery GUIDs and the joinery data in wood_rui_globals
    wood_rui_globals[data_name]["joinery_guid"] = joinery_guids
    wood_rui_globals[data_name]["joinery"] = joinery

    # Hide all child layers except the 'joinery' layer
    # hide_non_joinery_layers(plugin_name, data_name, "joinery")
