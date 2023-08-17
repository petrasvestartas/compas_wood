@echo off
cls
cd C:\brg\2_code\compas_wood\backend\build_win
cmake  --build C:\\brg\\2_code\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 8
C:\\brg\\2_code\\compas_wood\\backend\\build_win\\Release\\wood.exe