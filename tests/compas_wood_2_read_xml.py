from compas_wood.joinery import filenames_of_datasets
from compas_wood.joinery import read_xml_polylines
from compas_wood.viewer_helpers import display


# read the xml file and get a data-set
foldername = (
    "../compas_wood/src/frontend/src/wood/dataset/"
)

filename_of_dataset = filenames_of_datasets[7]
output_polylines = read_xml_polylines(foldername, filename_of_dataset)


# display the putput
display(None, output_polylines, None, 0.001)