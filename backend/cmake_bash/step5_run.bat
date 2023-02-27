@echo off
cls
cd C:\IBOIS57\_Code\Software\Python\compas_wood\backend\build_win
cmake  --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 8
C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe