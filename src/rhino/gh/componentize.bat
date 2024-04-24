@echo off

REM Install pythonnet
pip install pythonnet

REM Run the desired command
@REM python src\rhino\gh\componentize_cpy.py src\rhino\gh\cpy src\rhino\gh\build
python src\rhino\gh\componentize_cpy.py src\rhino\gh\cpy %APPDATA%\Grasshopper\UserObjects
