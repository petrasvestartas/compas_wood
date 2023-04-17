from ctypes import *


lib_object = cdll.LoadLibrary("C:/IBOIS57/_Code/Software/Python/compas_wood/src/frontend/build/Release/pinvoke_wood.dll")

result = lib_object.test_get_square(5)



print("______________________________________THANKS ANDERS!______________________________________")
print(result)
print("__________________________________________________________________________________________")
