********************************************************************************
Conda
********************************************************************************

Create environment
------------------

.. code-block:: bash

    conda create -n compas_wood_3_9_10 -c conda-forge python=3.9.10 compas
    conda activate compas_wood_3_9_10


Clone and install compas_wood
-----------------------------

.. code-block:: bash

    git clone https://github.com/petrasvestartas/compas_wood.git
    cd compas_wood
    pip install -r requirements.txt
    pip install -e .


Optional
--------

.. code-block:: bash

    pip install compas_viewer==1.1.2


Visual Studio Code
------------------

- Launch ``VSCode`` and select the ``compas_wood_3_9_10`` Python environment using ``CTRL+SHIFT+P``.
- Open a new terminal via ``Terminal -> New Terminal``.
- Navigate to the ``docs/examples`` folder and execute any ``.py`` example file by right-clicking and selecting ``Run Python File in Terminal``.

.. figure:: /_images/vscode_environment.gif
     :figclass: figure
     :class: figure-img img-fluid

Notes
=====

If you are new to Anaconda World, install these tools first:

- `Anaconda <https://www.anaconda.com/download>`_

- `Visual Studio Code <https://code.visualstudio.com/download>`_

- `Git <https://git-scm.com/downloads>`_