import ctypes

# Load the C++ shared library
proxy = ctypes.cdll.LoadLibrary("C:/legion_pro_7_16IRX8H/software/python/compas_wood/src/wood_pybind11.cp38-win_amd64.pyd")

# call the method
result = proxy.test_get_square(5)

# print result
print("__________________________________________________________________________________________")
print(result)
print("__________________________________________________________________________________________")
