import Rhino
from Rhino.DocObjects import ObjectType
from wood_rui import wood_rui_globals, ensure_layer_exists

def import_step_file(step_file_path, new_layer_name):
    # Step 1: Get the current object count before importing
    before_import_objects = [obj.Id for obj in Rhino.RhinoDoc.ActiveDoc.Objects]
    
    # Step 2: Use RhinoCommon to import the .STEP file via command
    import_command = '-_Import "{}" _Enter _Pause'.format(step_file_path)
    Rhino.RhinoApp.RunScript(import_command, False)
    
    # Step 3: Get the new object count after importing
    after_import_objects = [obj.Id for obj in Rhino.RhinoDoc.ActiveDoc.Objects]
    
    # Step 4: Identify newly imported objects by comparing the IDs
    new_object_ids = set(after_import_objects) - set(before_import_objects)
    if new_object_ids:
        last_imported_objects = [Rhino.RhinoDoc.ActiveDoc.Objects.FindId(obj_id) for obj_id in new_object_ids]
        
        # Step 5: Ensure the new layer exists
        layer_index = ensure_layer_exists("compas_wood", "annen", "surface")
        
        # Step 6: Change the layer of each imported object
        for obj in last_imported_objects:
            obj.Attributes.LayerIndex = layer_index
            obj.CommitChanges()
        
        return last_imported_objects
    else:
        return None



if __name__ == "__main__":

    dataset_name = "annen"
    wood_rui_globals.init_data(dataset_name)

# Example usage:
step_file_path = r"C:\brg\2_code\compas_wood\src\rhino\plugin\shared\datasets\annen.stp"
new_layer_name = "compas_wood::annen::surface"
imported_objects = import_step_file(step_file_path, new_layer_name)

if imported_objects:
    print(f"Successfully imported {len(imported_objects)} object(s) to layer '{new_layer_name}':")
    for obj in imported_objects:
        print(f"Object ID: {obj.Id}, Type: {obj.ObjectType}")
        geometry = obj.Geometry
        Rhino.RhinoDoc.ActiveDoc.Objects.Add(geometry)
else:
    print("No objects were imported.")