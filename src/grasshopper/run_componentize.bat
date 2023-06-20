@echo off
cls
cd "C:\legion_pro_7_16IRX8H\software\python\compas_wood\"
"C:\Program Files\IronPython 2.7\ipy.exe" src/grasshopper/componentize.py src/grasshopper/components src/grasshopper/build

mac
mono '/Users/vestarta/Downloads/ipy/net45/ipy.exe' src/grasshopper/componentize.py src/grasshopper/components src/grasshopper/build