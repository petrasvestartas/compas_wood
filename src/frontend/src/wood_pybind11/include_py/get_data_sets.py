import fnmatch
import os


# get all xml files in the dataset folder
def listdir(dirname, pattern="*"):
    return fnmatch.filter(os.listdir(dirname), pattern)


files = listdir(
    "C:/IBOIS57/_Code/Software/Python/compas_wood/frontend/src/wood/dataset/", "*.xml"
)

# remove .xml ending from the dataset names and skip the out dataset
input_data_sets = ["filenames_of_datasets = ["]
for i in range(0, len(files)):
    files[i] = files[i][:-4]
    if files[i] != "out":
        print(files[i])
        files[i] = files[i] + ","
        input_data_sets.append(files[i])
input_data_sets.append("]")

# output that is also printed in the console
a = input_data_sets
