#! python3
# venv: timber_connections
# r: compas, shapely, compas_model==0.6.1

import Rhino
import Rhino.Input
import Rhino.DocObjects
from Rhino.Geometry import Plane
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, add_beam
import System
from compas_rhino.conversions import mesh_to_compas


class StringDeserializer:
    def __init__(self):
        self.results = {}  # Dictionary to store outputs as lists

    def deserialize_transform(self, string):
        """Deserialize a Transform from a JSON string using Rhino.Geometry."""
        number_strings = string.split(',')
        transformation_numbers = [float(num) for num in number_strings if num]
        if len(transformation_numbers) == 16:
            T0 = Rhino.Geometry.Transform()
            T0.M00, T0.M01, T0.M02, T0.M03 = transformation_numbers[0:4]
            T0.M10, T0.M11, T0.M12, T0.M13 = transformation_numbers[4:8]
            T0.M20, T0.M21, T0.M22, T0.M23 = transformation_numbers[8:12]
            T0.M30, T0.M31, T0.M32, T0.M33 = transformation_numbers[12:16]
        self._store_result("xform", T0)  # Store result in dictionary
        

    def deserialize_mesh(self, string):
        """Deserialize a mesh from a JSON string using Rhino.Geometry."""
        mesh = Rhino.Geometry.Mesh.FromJSON(string)
        self._store_result("cut", mesh)  # Store result in dictionary
        return mesh

    def _store_result(self, key, value):
        """Helper method to store results in dictionary as lists."""
        if key not in self.results:
            self.results[key] = []  # Initialize as list if key is new
        self.results[key].append(value)  # Append new value

    def deserialize(self, key, string):
        """Call the appropriate deserialization method and store results in lists."""
        deserialization_functions = {
            "cut": self.deserialize_mesh,
            "xform": self.deserialize_transform
        }

        if key in deserialization_functions:
            return deserialization_functions[key](string)
        else:
            return None  # Handle unknown keys gracefully

def get_first_part(text):
    parts = text.split("_")
    return parts[0] if len(parts) > 1 else text

def get_mesh_attributes():
    option_name = "beams"
    go = Rhino.Input.Custom.GetObject()
    go.SetCommandPrompt(f"Select {option_name}")
    go.GeometryFilter = Rhino.DocObjects.ObjectType.Mesh  # Filter to curves
    go.EnablePreSelect(True, True)
    go.SubObjectSelect = False
    go.DeselectAllBeforePostSelect = False
    res = go.GetMultiple(1, 0)

    if go.CommandResult() == Rhino.Commands.Result.Success:
        selected_meshes = []

        for i in range(go.ObjectCount):
            obj = go.Object(i)
            rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(go.Object(i).ObjectId)
            user_strings : System.Collections.Specialized.NameValueCollection = rhino_object.Attributes.GetUserStrings()


            deserializer = StringDeserializer()
            for key in user_strings.AllKeys:
                value = user_strings[key]
                function_name = get_first_part(key)


                result = deserializer.deserialize(function_name, value)  # Replace json_string with actual JSON
                print(deserializer.results)  # Check stored results

                # xform



            mesh = obj.Mesh()
            if mesh:
                selected_meshes.append(mesh)
        return selected_meshes


if __name__ == "__main__":

    get_mesh_attributes()

#     dataset_name = "beamshape"
#     wood_rui_globals.init_data(dataset_name)

#     input_dict = {
#         "beam": ([], List[Rhino.Geometry.Mesh]),
#     }

#    # Call the generalized input method with the dataset name and input dictionary
#     generalized_input_method(dataset_name, input_dict, my_callback, False)


