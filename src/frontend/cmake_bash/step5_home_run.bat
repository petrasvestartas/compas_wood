@echo off
cls
cd C:\legion_pro_7_16IRX8H\software\python\compas_wood\backend\build_win
cmake  --build C:\\legion_pro_7_16IRX8H\\software\\python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 8
C:\\legion_pro_7_16IRX8H\\software\\python\\compas_wood\\backend\\build_win\\Release\\wood.exe