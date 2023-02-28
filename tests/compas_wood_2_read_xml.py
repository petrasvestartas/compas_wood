from compas_wood.joinery import filenames_of_datasets
from compas_wood.joinery import read_xml
from compas_wood.viewer_helpers import display


# read the xml file
foldername = (
    "C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/src/wood/dataset/"
)
filename_of_dataset = filenames_of_datasets[7]
polyline_output = read_xml(foldername, filename_of_dataset)

# print the output
print(len(polyline_output))

# display the putput
display(None, polyline_output, None, 0.001)
