********************************************************************************
Installation
********************************************************************************

Conda
=====

.. code-block:: bash

    conda create -n compas_wood_3_9_10 python=3.9.10 compas
    conda activate compas_wood_3_9_10
    pip install -r requirements.txt
    pip install -e .


Geometry is displayed using compas_viewer. Please clone the viewer to have the latest version install in the current environment:¨


.. code-block:: bash

    git clone https://github.com/compas-dev/compas_viewer.git
    cd compas_viewer
    conda activate compas_wood_3_9_10
    pip install -e .


Pip
===
.. code-block:: bash

    pip install compas_wood
