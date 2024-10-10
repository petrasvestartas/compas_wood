import Rhino
from .globals import wood_rui_globals
from .layer import ensure_layer_exists  # Import the singleton instance
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
    
    print("add_polylines", data_name)

    layer_index = ensure_layer_exists("compas_wood", data_name, "polylines", Color.Red)

    polyline_guids = []
    for idx, polyline in enumerate(polylines):
        if polyline:
            obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddCurve(polyline.ToNurbsCurve())
            if obj_guid:
                obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
                if obj:
                    obj.Attributes.LayerIndex = layer_index
                    
                    polyline_guids.append(obj_guid)
                    obj.Attributes.SetUserString("element_id", str(int(idx*0.5)))
                    obj.Attributes.SetUserString("dataset", data_name)
                    obj.Attributes.SetUserString("type", "polylines")
                    obj.CommitChanges()
                else:
                    print(obj_guid, obj)
            else:
                print(obj_guid)

    if "polylines_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["polylines_guid"])
    wood_rui_globals[data_name]["polylines_guid"] = polyline_guids
    wood_rui_globals[data_name]["polylines"] = polylines
    
    layer_index_dots = ensure_layer_exists("compas_wood", data_name, "joint_types", Color.Gray)
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_insertion_lines(lines, data_name):
    """Add a list of polylines to the specified layer and return their GUIDs."""
    
    print("insertion_lines", data_name)

    layer_index = ensure_layer_exists("compas_wood", data_name, "insertion", Color.Blue)

    line_guids = []
    for line in lines:
        obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line)
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
        obj.Attributes.LayerIndex = layer_index
        
        line_guids.append(obj_guid)
        obj.Attributes.SetUserString("dataset", data_name)
        obj.Attributes.SetUserString("type", "insertion")
        obj.CommitChanges()

    if "insertion_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["insertion_guid"])
    wood_rui_globals[data_name]["insertion_guid"] = line_guids
    wood_rui_globals[data_name]["insertion"] = lines
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_mesh(mesh, data_name):
    """Add a mesh to the specified layer and return the mesh's GUID."""

    print("add_mesh", data_name)
    
    layer_index = ensure_layer_exists("compas_wood", data_name, "mesh", Color.Black)

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
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_adjacency(four_indices_face_face_edge_edge, data_name):
    
    print("add_adjacency", data_name)
    wood_rui_globals[data_name]["adjacency"] = four_indices_face_face_edge_edge

    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms

def add_flags(flags, data_name):
    
    print("add_flags", data_name)
    wood_rui_globals[data_name]["flags"] = flags

def add_insertion_vectors(insertion_vectors, data_name):
    
    print("insertion_vectors", data_name)
    wood_rui_globals[data_name]["insertion_vectors"] = insertion_vectors
    
    polylines_guid = wood_rui_globals[data_name]["polylines_guid"]
    for idx, (obj_guid1, obj_guid2) in enumerate(zip(polylines_guid[0::2], polylines_guid[1::2])):
        for obj_guid in (obj_guid1, obj_guid2):
            obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
            if obj:
                for idy, joints_type in enumerate(insertion_vectors[idx]):
                    obj.Attributes.SetUserString("insertion_vectors"+str(idy), str(joints_type))
                obj.CommitChanges()
            else:
                print(obj_guid, obj)
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms

def add_three_valence(three_valence, data_name):
    
    print("three_valence", data_name)
    wood_rui_globals[data_name]["three_valence"] = three_valence


def add_joinery(joinery: List[List[Rhino.Geometry.Polyline]], data_name: str) -> None:
    """
    Add a list of lists of polylines as groups to the specified layer in Rhino and return their GUIDs.

    :param joinery: A list of lists of polylines to add to Rhino.
    :param data_name: The name of the case to be used for layer and group management.
    """
    
    print("add_joinery", data_name)

    # Use wood_rui_globals.plugin_name as the top-level layer
    plugin_name = wood_rui_globals.plugin_name

    # Ensure the layer exists or create it under the plugin layer
    layer_index = ensure_layer_exists("compas_wood", data_name, "joinery", Color.Black)

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
            
            group_guids.append(obj_guid)  # Collect the GUID for this polyline
            obj.Attributes.SetUserString("dataset", data_name)
            obj.Attributes.SetUserString("type", "joinery")
            obj.CommitChanges()  # Commit the changes to the object's attributes

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
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_joint_type(joints_per_face: List[List[int]], data_name: str) -> None:
    
    wood_rui_globals[data_name]["joints_per_face"] = joints_per_face
      
 
    polylines_guid = wood_rui_globals[data_name]["polylines_guid"]
    for idx, (obj_guid1, obj_guid2) in enumerate(zip(polylines_guid[0::2], polylines_guid[1::2])):
        for obj_guid in (obj_guid1, obj_guid2):
            obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
            if obj:
                for idy, joints_type in enumerate(joints_per_face[idx]):
                    obj.Attributes.SetUserString("joints_per_face_"+str(idy), str(joints_type))
                obj.CommitChanges()
            else:
                print(obj_guid, obj)
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_loft_brep(breps, data_name):
    """Add a list of lofted polylines with holes as breps to the specified layer and return their GUIDs."""
    
    print("loft", data_name)

    layer_index = ensure_layer_exists("compas_wood", data_name, "loft", Color.Black)

    brep_guids = []
    for brep in breps:
        obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(brep)
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
        obj.Attributes.LayerIndex = layer_index
        brep_guids.append(obj_guid)
        obj.Attributes.SetUserString("dataset", data_name)
        obj.CommitChanges()

    if "loft_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["loft_guid"])
    wood_rui_globals[data_name]["loft_guid"] = brep_guids
    wood_rui_globals[data_name]["loft"] = breps
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def add_loft_mesh(meshes, data_name):
    """Add a list of lofted polylines with holes as meshes to the specified layer and return their GUIDs."""
    
    print("loft", data_name)

    layer_index = ensure_layer_exists("compas_wood", data_name, "loft", Color.Black)

    mesh_guids = []
    for mesh in meshes:
        obj_guid = Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(mesh)
        obj = Rhino.RhinoDoc.ActiveDoc.Objects.Find(obj_guid)
        obj.Attributes.LayerIndex = layer_index
        mesh_guids.append(obj_guid)
        obj.Attributes.SetUserString("dataset", data_name)
        obj.CommitChanges()

    if "loft_guid" in wood_rui_globals[data_name]:
        delete_objects(wood_rui_globals[data_name]["loft_guid"])
    wood_rui_globals[data_name]["loft_guid"] = mesh_guids
    wood_rui_globals[data_name]["loft"] = meshes
    Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms