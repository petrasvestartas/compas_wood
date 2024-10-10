

class Globals(object):
    """Singleton class with dictionary-like access for global variables per plugin."""

    # Dictionary to store singleton instances per plugin
    _instances = {}
    scale = [1.0, 1.0, 1.0]  # temporary parameter that must go from compas_wood.binding import wood_globals
    search_type = 0  # temporary parameter that must go from compas_wood.binding import wood_globals
    joint_volume_extension = [0.0, 0.0, 0.0]  # temporary parameter that must go from compas_wood.binding import wood_globals

    def __new__(cls, plugin_name="compas_wood"):
        """Ensure that only one instance per plugin exists."""
        if plugin_name not in cls._instances:
            print("Creating new instance for plugin: {}".format(plugin_name))
            cls._instances[plugin_name] = super(Globals, cls).__new__(cls)
        return cls._instances[plugin_name]

    def __init__(self, plugin_name="compas_wood"):
        """Initialize the plugin-specific globals storage."""
        if not hasattr(self, 'initialized'):  # Ensure initialization happens only once
            self.plugin_name = plugin_name
            self.dataset = {}
            self.initialized = True
            self.reset()

    def init_data(self, name):

        self.dataset[name] = {
            "mesh": None,  # INPUT meshes that are good inputs for generating polylines
            "mesh_guid": None,
            "surface": None,  # INPUT surface that is good for generating meshes
            "surface_guid": None,
            "axes": None,  # INPUT axis that is good for generating polylines
            "axes_guid": None,
            "polylines": [],  # INPUT top and bottom outlines of a plate
            "polylines_guid": [],
            "insertion_lines": [],  # USER lines for direction of a joint
            "insertion_lines_guid": [],
            "joints_dots": [],  # USER text dot that define joint type
            "joints_dots_guid": [],
            "insertion_vectors": [],  # INPUT generated insertion vector per face
            "insertion_vectors_guid": [],
            "joints_per_face": [],  # INPUT text dot that defines the joint type
            "joints_per_face_guid": [],
            "three_valence": [],  # INPUT special dataset for annen and vidy
            "three_valence_guid": [],
            "adjacency": [],  # INPUT only filled for special dataset like annen, where computation time is important
            "three_valence_element_indices_and_instruction": [],  # INPUT special dataset for annen and vidy
            "planes": [],
            "planes_guid": [],
            "flags": [],
            "joinery": [],  # OUTPUT geometry related to joints
            "joinery_guid": [],
            "loft": [],  # OUTPUT lofted geometry
            "loft_guid": [],
        }
        
        from System.Drawing import Color
        from .layer import ensure_layer_exists as create_layer
        create_layer(self.plugin_name, name, "mesh", Color.Black)
        create_layer(self.plugin_name, name, "surface", Color.Black)
        create_layer(self.plugin_name, name, "axes", Color.Black)
        create_layer(self.plugin_name, name, "polylines", Color.Red)
        create_layer(self.plugin_name, name, "insertion", Color.Blue)
        create_layer(self.plugin_name, name, "joint_types", Color.Gray)
        create_layer(self.plugin_name, name, "loft", Color.Black)

    def __getitem__(self, key):
        return self.dataset[key]

    def __setitem__(self, key, value):
        self.dataset[key] = value

    def __delitem__(self, key):
        if key in self.dataset:
            del self.dataset[key]

    def __contains__(self, key):
        return key in self.dataset

    def reset(self):
        """Reset the Globals instance to its initial phase."""
        self.dataset.clear()
        self.plugin_name = "compas_wood"
        self.init_data("default")
        self.scale = [1.0, 1.0, 1.0]  # temporary parameter that must go from compas_wood.binding import wood_globals
        self.search_type = 0  # temporary parameter that must go from compas_wood.binding import wood_globals
        self.joint_volume_extension = [
            0.0,
            0.0,
            0.0,
        ]  # temporary parameter that must go from compas_wood.binding import wood_globals
        print("Globals instance has been reset.")

    def __repr__(self):
        """Return a string representation of the Globals instance."""
        return f"Globals(plugin_name={self.plugin_name}, dataset={self._format_dataset(self.dataset)})"

    def _format_dataset(self, dataset, indent=0):
        """Recursively format the dataset dictionary."""
        formatted = ""
        for key, value in dataset.items():
            formatted += " " * indent + f"{key}: "
            if isinstance(value, dict):
                formatted += "{\n" + self._format_dataset(value, indent + 2) + " " * indent + "}\n"
            elif isinstance(value, list):
                formatted += "[\n" + self._format_list(value, indent + 2) + " " * indent + "]\n"
            else:
                formatted += f"{value}\n"
        return formatted

    def _format_list(self, lst, indent=0):
        """Recursively format a list."""
        formatted = ""
        for item in lst:
            if isinstance(item, dict):
                formatted += " " * indent + "{\n" + self._format_dataset(item, indent + 2) + " " * indent + "},\n"
            elif isinstance(item, list):
                formatted += " " * indent + "[\n" + self._format_list(item, indent + 2) + " " * indent + "],\n"
            else:
                formatted += " " * indent + f"{item},\n"
        return formatted


wood_rui_globals = Globals()
