class Globals(object):
    """Singleton class with dictionary-like access for global variables per plugin."""

    # Dictionary to store singleton instances per plugin
    _instances = {}
    scale = [1.0, 1.0, 1.0] # temporary parameter that must go from compas_wood.binding import wood_globals
    search_type = 0 # temporary parameter that must go from compas_wood.binding import wood_globals
    extension = [0.0, 0.0, 0.0] # temporary parameter that must go from compas_wood.binding import wood_globals

    def __new__(cls, plugin_name="compas_wood"):
        """Ensure that only one instance per plugin exists."""
        if plugin_name not in cls._instances:
            print("Creating new instance for plugin: {}".format(plugin_name))
            instance = super(Globals, cls).__new__(cls)
            # Initialize the plugin-specific globals storage
            instance.plugin_name = plugin_name
            instance.cases = {}
            instance.init_case("default")  # Call init_case on the instance
            
            cls._instances[plugin_name] = instance
            
        return cls._instances[plugin_name]

    def init_case(self, name):
        
        self.cases[name] = {
            "mesh": None, # INPUT meshes that are good inputs for generating polylines
            "mesh_guid": None, 
            "polylines": [], # INPUT top and bottom outlines of a plate
            "polylines_guid": [],
            "insertion_lines": [], # USER lines for direction of a joint
            "insertion_lines_guid": [],
            "joints_dots": [], # USER text dot that define joint type
            "joints_dots_guid": [],
            "insertion_vectors": [], # INPUT generated insertion vector per face
            "insertion_vectors_guid": [],
            "joints_per_face": [], # INPUT text dot that defines the joint type
            "joints_per_face_guid": [],
            "three_valence": [], # INPUT special cases for annen and vidy
            "three_valence_guid": [],
            "adjacency": [], # INPUT only filled for special cases like annen, where computation time is important
            "planes" : [],
            "planes_guid" : [],
            "flags": [],
            "interfaces": [], # OUTPUT polygons where two faces of a plate have a common area
            "interfaces_guid": [],
            "joinery": [], # OUTPUT geometry related to joints
            "joinery_guid": [],
            "loft" : [], # OUTPUT lofted geometry
            "loft_guid" : [],
        }

    def __getitem__(self, key):
        return self.cases[key]

    def __setitem__(self, key, value):
        self.cases[key] = value

    def __delitem__(self, key):
        if key in self.cases:
            del self.cases[key]

    def __contains__(self, key):
        return key in self.cases

    def reset(self):
        """Reset the Globals instance to its initial phase."""
        self.cases.clear()
        self.plugin_name = "compas_wood"
        self.init_case("default")
        self.scale = [1.0, 1.0, 1.0] # temporary parameter that must go from compas_wood.binding import wood_globals
        self.search_type = 0 # temporary parameter that must go from compas_wood.binding import wood_globals
        self.extension = [0.0, 0.0, 0.0] # temporary parameter that must go from compas_wood.binding import wood_globals
        print("Globals instance has been reset.")



    def __repr__(self):
        """Return a string representation of the Globals instance."""
        return f"Globals(plugin_name={self.plugin_name}, cases={self._format_cases(self.cases)})"

    def _format_cases(self, cases, indent=0):
        """Recursively format the cases dictionary."""
        formatted = ""
        for key, value in cases.items():
            formatted += " " * indent + f"{key}: "
            if isinstance(value, dict):
                formatted += "{\n" + self._format_cases(value, indent + 2) + " " * indent + "}\n"
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
                formatted += " " * indent + "{\n" + self._format_cases(item, indent + 2) + " " * indent + "},\n"
            elif isinstance(item, list):
                formatted += " " * indent + "[\n" + self._format_list(item, indent + 2) + " " * indent + "],\n"
            else:
                formatted += " " * indent + f"{item},\n"
        return formatted


wood_rui_globals = Globals()