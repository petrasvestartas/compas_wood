@echo off

REM Install pythonnet
pip install pythonnet

REM Run the desired command
python src\rhino\gh\componentize_cpy.py src\rhino\gh\cpy src\rhino\gh\build
python src\rhino\gh\componentize_cpy.py src\rhino\gh\cpy %APPDATA%\McNeel\Rhinoceros\packages\8.0\compas_wood\2.1.0
