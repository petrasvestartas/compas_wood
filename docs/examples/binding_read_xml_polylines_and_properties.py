from compas_wood.binding import read_xml_polylines_and_properties
from compas.geometry import Line
import compas_wood
import os

dataset_index = 0
module_path = os.path.dirname(compas_wood.__file__)
foldername = os.path.join(module_path, "datasets")
foldername = os.path.join(module_path, "datasets") + os.sep
files = os.listdir(foldername)

file_names_without_extensions = [os.path.splitext(file)[0] for file in files]
filename = file_names_without_extensions[dataset_index % len(file_names_without_extensions)]
filename = "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small"


(
    polylines,
    vectors_lists,
    input_joints_types,
    input_three_valence_element_indices_and_instruction,
    input_adjacency,
) = read_xml_polylines_and_properties(foldername, filename)


# print("polylines:", polylines)
print("input_three_valence_element_indices_and_instruction:", input_three_valence_element_indices_and_instruction)
print(filename)

try:
    
    from compas_viewer import Viewer

    viewer = Viewer()

    for i in range(len(vectors_lists)):
        viewer.scene.add(polylines[i * 2 + 0], show_points=False)
        viewer.scene.add(polylines[i * 2 + 1], show_points=False)
        for segment, vector in zip(polylines[i * 2 + 0].lines, vectors_lists[i]):
            midpoint = segment.midpoint
            line = Line(midpoint, midpoint + vector)
            if line.length > 0:
                viewer.scene.add(line, show_points=False, lineswidth=3)
            else:
                viewer.scene.add(line.start, show_points=True, points_size=0.02)

    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
