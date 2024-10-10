import Rhino
import System
from typing import *
from System.Drawing import Color  # Import Color from System.Drawing
from Rhino import RhinoMath


def ensure_layer_exists(plugin_name: str, data_name: str, type_name: str, color: Color = None):
    """Ensure that the plugin_name layer, data_name sublayer, and type_name sublayer exist, and return the type layer index.

    If delete_existing is True, delete all objects from the specified layers if they exist.

    Parameters
    ----------
    data_name : str
        The name of the data sub-layer.
    type_name : str
        The name of the type sub-sub-layer.

    """
    
    # Check if the parent (plugin) layer exists
    plugin_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.FindByFullPath(plugin_name, True)
    if plugin_layer_index < 0:
        # Create the parent layer if it doesn't exist
        plugin_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.Add(
            plugin_name, System.Drawing.Color.Black
        )

    # Now create the full path for the case (second-level) layer
    case_layer_name = plugin_name + "::" + data_name
    case_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.FindByFullPath(case_layer_name, True)
    if case_layer_index < 0:
        # Create the case layer under the plugin layer
        case_layer = Rhino.DocObjects.Layer()
        case_layer.Name = data_name
        case_layer.ParentLayerId = Rhino.RhinoDoc.ActiveDoc.Layers[plugin_layer_index].Id
        case_layer.Color = System.Drawing.Color.Black
        case_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.Add(case_layer)

    # Now create the full path for the type (third-level) layer
    type_layer_name = case_layer_name + "::" + type_name
    type_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.FindByFullPath(type_layer_name, True)
    if type_layer_index < 0:
        # Create the type layer under the case layer
        type_layer = Rhino.DocObjects.Layer()
        type_layer.Name = type_name
        type_layer.ParentLayerId = Rhino.RhinoDoc.ActiveDoc.Layers[case_layer_index].Id
        type_layer.Color = color if color else System.Drawing.Color.Black
        type_layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.Add(type_layer)

    return type_layer_index


def delete_objects_in_layer(layer_index):
    """Delete all objects in the specified layer and its immediate child layers."""
    layer = Rhino.RhinoDoc.ActiveDoc.Layers[layer_index]
    objects = Rhino.RhinoDoc.ActiveDoc.Objects.FindByLayer(layer)
    for obj in objects:
        Rhino.RhinoDoc.ActiveDoc.Objects.Delete(obj, True)

    # Delete objects in immediate child layers
    child_layers = [l for l in Rhino.RhinoDoc.ActiveDoc.Layers if l.ParentLayerId == layer.Id]
    for child_layer in child_layers:
        objects = Rhino.RhinoDoc.ActiveDoc.Objects.FindByLayer(child_layer)
        for obj in objects:
            Rhino.RhinoDoc.ActiveDoc.Objects.Delete(obj, True)


def get_objects_by_layer(layer_name, debug=False):
    """ Get Rhino objects by layer name.
    
    Parameters
    ----------
    layer_name : str
        The name of the layer to search for objects.
    """
    
    # Find objects by the layer name
    layer_index = Rhino.RhinoDoc.ActiveDoc.Layers.FindByFullPath(layer_name, RhinoMath.UnsetIntIndex)
    layer = None
    if layer_index != RhinoMath.UnsetIntIndex: 
        layer = Rhino.RhinoDoc.ActiveDoc.Layers[layer_index]
    else:
        layer = Rhino.RhinoDoc.ActiveDoc.Layers.FindName(layer_name)
    if layer is None:
        print(f"Layer not found: {layer_name}")
        return
    
    print("get_objects_by_layer: ", layer)
    
    
    rhino_objects = Rhino.RhinoDoc.ActiveDoc.Objects.FindByLayer(layer)
    
    # Check if objects are found
    if rhino_objects is None:
        print(f"No objects found on layer: {layer_name}")
        return
    
    # Iterate through the found objects and print their details
    if debug:
        for obj in rhino_objects:
            if obj:
                # Example: print object ID and type
                print(f"Object ID: {obj.Id}, Object Type: {obj.ObjectType}")
    
    return rhino_objects