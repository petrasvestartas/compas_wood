from compas_wood.joinery import filenames_of_datasets
from compas_wood.joinery import read_xml_polylines
from compas_wood.joinery import read_xml_polylines_and_properties
from compas_wood.joinery import get_connection_zones
from compas_wood.viewer_helpers import display


# read the xml file
foldername = (
    "C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/src/wood/dataset/"
)
filename_of_dataset = filenames_of_datasets[7]
output_polylines = read_xml_polylines(foldername, filename_of_dataset)

# print the output
# print(output_polylines)

# display the putput
# display(None, output_polylines, None, 0.001)

# read the xml file and properties
filename_of_dataset_with_properties = filenames_of_datasets[33]
(
    output_polylines_with_properties,
    output_vectors,
    output_joints_types,
    output_three_valence_element_indices_and_instruction,
    output_adjacency,
) = read_xml_polylines_and_properties(foldername, filename_of_dataset_with_properties)

# print the output
# print(output_polylines_with_properties)
# print(output_vectors)
# print(output_joints_types)
# print(output_three_valence_element_indices_and_instruction)
# print(output_adjacency)

lists_of_polylines = get_connection_zones(
    output_polylines_with_properties,
    output_vectors,
    output_joints_types,
    output_three_valence_element_indices_and_instruction,
    output_adjacency,
    None,
    0,
    [1, 1, 1],
    4,
    True,
)


# display the putput
display(None, lists_of_polylines, None, 0.001)
