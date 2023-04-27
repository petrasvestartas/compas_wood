import ctypes

# Load library
lib = ctypes.cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")
#lib = ctypes.cdll.LoadLibrary("C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")

# Define function prototype
test_get_array_floats = lib.test_get_array_floats
test_get_array_floats.argtypes = [
    ctypes.POINTER(ctypes.POINTER(ctypes.c_float)),
    ctypes.POINTER(ctypes.c_size_t),
]
test_get_array_floats.restype = None

# Call function
coord_out = ctypes.POINTER(ctypes.c_float)()
coord_size_out = ctypes.c_size_t()
test_get_array_floats(ctypes.byref(coord_out), ctypes.byref(coord_size_out))

# Convert output to Python list
coord_list = [coord_out[i] for i in range(coord_size_out.value)]

# Print output
print(coord_list)

# Release memory allocated in C++ code
lib.release_float(coord_out)
