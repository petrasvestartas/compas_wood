@echo off
cls
cd C:\legion_pro_7_16IRX8H\software\python\compas_wood\src\frontend\build
cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=ON -DRELEASE_DEBUG=ON -DCMAKE_OSX_ARCHITECTURES=x86_64     .. 
cmake --build . --config Release --parallel 8