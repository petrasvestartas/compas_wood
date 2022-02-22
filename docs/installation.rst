********************************************************************************
Installation
********************************************************************************

################################################################################
Windows
################################################################################

.. code-block:: 

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood
    cd compas_wood
    pip install -e .

################################################################################
Mac
################################################################################

.. code-block:: 

    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https:/github.com/petrasvestartas/compas_wood
    cd compas_wood 
    pip install -e 

################################################################################
Rhino Grasshopper 
################################################################################

.. code-block:: 

    Download the (Windows) or (Mac) zipped files and place them in libraries folder.
    You can find this folder when you open Grasshopper. 
    File->Special Folder->Component Folder.
    https://github.com/ibois-epfl/compas_wood/releases/tag/compas_wood_GH_1.0.0

