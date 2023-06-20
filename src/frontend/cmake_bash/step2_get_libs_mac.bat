@echo off
cls
cd C:\legion_pro_7_16IRX8H\software\python\compas_wood\src\frontend\build
cmake --fresh -DGET_LIBS=ON -DCOMPILE_LIBS=OFF -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release" -DCMAKE_OSX_ARCHITECTURES=x86_64   -G "Xcode"  .. 
cmake --build . --config Release --parallel 8