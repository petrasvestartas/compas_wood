from ctypes import *
import ctypes

# Define custom structure for nested arrays
class Array_Floats(ctypes.Structure):
    _fields_ = [("data", ctypes.POINTER(ctypes.c_float)),
                ("size", ctypes.c_size_t)]

# Load library
lib = cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")
#lib = ctypes.cdll.LoadLibrary("C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")

# Declare function signature
lib.test_get_nested_array_floats.restype = None
lib.test_get_nested_array_floats.argtypes = (ctypes.POINTER(ctypes.POINTER(Array_Floats)), ctypes.POINTER(ctypes.c_size_t))

# Call function
array_out_ptr = ctypes.POINTER(Array_Floats)()
array_size_ptr = ctypes.c_size_t()
lib.test_get_nested_array_floats(ctypes.byref(array_out_ptr), ctypes.byref(array_size_ptr))

# Access the nested array data
for i in range(array_size_ptr.value):
    inner_array = array_out_ptr[i].data[:array_out_ptr[i].size]
    print(inner_array)
    
# Free memory
lib.release_nested_array.restype = None
lib.release_nested_array.argtypes =  (ctypes.POINTER(ctypes.POINTER(Array_Floats)), ctypes.POINTER(ctypes.c_size_t))
lib.release_nested_array(ctypes.byref(array_out_ptr), ctypes.byref(array_size_ptr))
