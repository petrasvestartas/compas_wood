from compas_wood.binding import read_xml_polylines
import compas_wood
import os

dataset_index = -1
module_path = os.path.dirname(compas_wood.__file__)
foldername = os.path.join(module_path, "datasets")
foldername = os.path.join(module_path, "datasets") + os.sep
files = os.listdir(foldername)

file_names_without_extensions = [os.path.splitext(file)[0] for file in files]
filename = file_names_without_extensions[dataset_index % len(file_names_without_extensions)]

polylines = read_xml_polylines(foldername, filename)


try:

    from compas_viewer import Viewer
    from compas.geometry import Scale

    scale = 1e-2
    xform = Scale.from_factors([scale, scale, scale])

    viewer = Viewer()

    for polyline in polylines:
        polyline.transform(xform)
        viewer.scene.add(polyline, show_points=False)

    viewer.show()


except ImportError:
    print("compas_viewer is not installed.")
