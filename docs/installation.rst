********************************************************************************
Installation
********************************************************************************

################################################################################
Windows
################################################################################

.. code-block:: 

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
    conda activate wood-dev 
    git clone https://github.com/petrasvestartas/compas_wood.git
    cd compas_wood
    pip install -e .

################################################################################
Mac
################################################################################

.. code-block:: 

    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood.git
    cd compas_wood 
    pip install -e .

################################################################################
Rhino Grasshopper 
################################################################################

.. code-block:: 

    Download the zipped files and place them in libraries folder.
    Current suppoorted Grasshopper is version is Windows only.
    You can find this folder when you open Grasshopper. 
    File->Special Folder->Component Folder.
    https://github.com/petrasvestartas/compas_wood/releases/tag/compas_wood_GH_1.0.0

################################################################################
C++
################################################################################

Step 1: Create a build directory and go into it

.. code-block:: cmake

    cd C:\IBOIS57\_Code\Software\Python\compas_wood\backend\
    mkdir build_win
    cd build_win

Step 2: Download libraries via CMake

.. code-block:: cmake
    
    cmake --fresh -DGET_LIBS=ON -DCOMPILE_LIBS=OFF -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 

Step 3: Build 3rd-party libraries (this part compiles 3rd party libraries to static libraries that reduces compilation time while working with C++)

.. code-block:: cmake    

    cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 

Step 4: Build the code (precompiled header is compiled to reduce the compilation time)

.. code-block:: cmake
 
    cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 

Step 5 - Run the code - you can use this as a default VScode task:

.. code-block:: cmake 

    cmake  --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 8 &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe