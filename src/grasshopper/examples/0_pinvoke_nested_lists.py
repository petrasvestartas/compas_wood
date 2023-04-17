import ctypes


# Load the C++ shared library
proxy = ctypes.cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/wood_pybind11.cp38-win_amd64.pyd")


# Prepare the nested list in Python
nested_list = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]

# Convert the nested list to a flat list
flat_list = [item for sublist in nested_list for item in sublist]

# Convert the flat list to a ctypes array
c_array = (ctypes.c_int * len(flat_list))(*flat_list)

# Call the C++ function with the ctypes array as argument and pass in buffers for the flattened list and its size
flattened_list = (ctypes.c_int * len(flat_list))()
flattened_list_size = ctypes.c_int()
proxy.process_and_return_nested_list(c_array, flattened_list, ctypes.byref(flattened_list_size))

# Convert the flattened list back to a Python list
result_list = list(flattened_list[:flattened_list_size.value])

# Convert the flattened list back to a nested list
nested_list_size = len(nested_list[0])
nested_list = [result_list[i:i + nested_list_size] for i in range(0, flattened_list_size.value, nested_list_size)]

# Print the result
print("__________________________________________________________________________________________")
print("nested_list = ", nested_list)
print("__________________________________________________________________________________________")

