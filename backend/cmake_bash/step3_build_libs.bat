@echo off
cls
cd C:\IBOIS57\_Code\Software\Python\compas_wood\backend\build_win
cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. 
cmake --build . --config Release --parallel 8