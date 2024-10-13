#! python3
import Rhino
import System
import System.IO
from compas_wood.binding import read_xml_polylines
import os
from wood_rui import add_polylines, wood_rui_globals, BooleanForm, generalized_input_method
from wood_nano import read_xml_polylines as wood_nano_read_xml_polylines
from wood_nano import double2
from typing import *


def read_xml_polylines(
    foldername="/home/petras/brg/2_code/wood_nano/src/wood/cmake/src/wood/dataset/",
    filename="type_plates_name_cross_vda_hexshell_reciprocal",
):
    """Read polylines from XML file.

    Parameters
    ----------
    foldername : str, optional
        Foldername.
    filename_of_dataset : str, optional
        Filename of dataset.

    Returns
    -------
    list[Rhino.Geometry.Polyline]
        List of polylines.
    """

    polylines_coordinates = double2()
    wood_nano_read_xml_polylines(
        foldername,
        filename,
        polylines_coordinates,
    )

    polylines = []
    for polyline in polylines_coordinates:
        points = []
        for i in range(0, len(polyline), 3):
            points.append(Rhino.Geometry.Point3d(polyline[i], polyline[i + 1], polyline[i + 2]))
        polyline = Rhino.Geometry.Polyline(points)
        polylines.append(polyline)

    return polylines


def load_data_set(): # path: str = "C://brg//2_code//compas_wood//src//rhino//plugin//shared//datasets"
    """Load datasets from xml files.

    Parameters
    ----------
    path : str, optional
        Foldername.

    """

    guid = System.Guid("D0647BA8-EEE5-4C18-AB3E-03A95F119654")
    plugin_file: str = Rhino.PlugIns.PlugIn.PathFromId(guid)
    plugin_path: str = System.IO.Path.GetDirectoryName(plugin_file)
    path: str = System.IO.Path.Combine(plugin_path, "shared")
    # load_data_set(data_sets)

    # Get filenames from the path:
    module_path: str = path  # os.path.dirname(compas_wood.__file__)
    print("Data-sets are located in the shared folder:\n" + module_path)
    foldername: str = module_path + os.sep
    files: List[str] = os.listdir(foldername)
    file_names_without_extensions: List[str] = [os.path.splitext(file)[0] for file in files]

    # Create a form with all file names and check which one to display:
    form: BooleanForm = BooleanForm(
        names=file_names_without_extensions, values=[False] * len(file_names_without_extensions), title="Datasets"
    )
    form.show()

    # Check which values are set.
    for value in form.attributes:
        if value[1]:
            wood_rui_globals.init_data(value[0])
            polylines = read_xml_polylines(foldername, value[0])
            wood_rui_globals[value[0]]["polylines"] = polylines
            add_polylines(polylines, value[0])
            Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms

    return file_names_without_extensions

def my_callback(name_value_type, dataset_name):

    if len(input_dict["polylines"][0])>0:
        dataset_name = "default"
        wood_rui_globals.init_data(dataset_name)
        add_polylines(input_dict["polylines"][0],dataset_name)




if __name__ == "__main__":



    # Define the input dictionary based on your initial dataset
    input_dict = {
        "library": (load_data_set, Callable),                       # Default value for weld radius (float)
        "polylines": ([], List[Rhino.Geometry.Polyline]),  # Default value for polylines (list of polylines)
    }

    # Call the generalized input method with the dataset name and input dictionary
    dataset_name = "default"
    generalized_input_method(dataset_name, input_dict, my_callback)

