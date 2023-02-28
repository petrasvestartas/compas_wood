********************************************************************************
Installation
********************************************************************************

Stable
======

Stable releases of :mod:`compas_cgal` can be installed via ``conda-forge``.

.. code-block:: bash

    conda create -n cgal -c conda-forge compas compas_cgal --yes
    conda activate cgal


Optional
========

Several examples use COMPAS View2 for visualisation outside of CAD environments.
To install :mod:`compas_view2` in the same environment

.. code-block:: bash

    conda install compas_view2 --yes

Or everything in one go

.. code-block:: bash

    conda create -n cgal -c conda-forge compas compas_cgal compas_view2 --yes


Dev Install
===========

A local development version can be set up using a combination of ``conda`` and ``pip``.
First, clone the :mod:`compas_cgal` repo.

.. code-block:: bash

    git clone https://github.com/compas-dev/compas_cgal


Create a dev environment.

.. code-block:: bash

    conda create -n cgal-dev python=3.8 compas compas_view2 --yes

Install common dev requirements.

.. code-block:: bash

    conda install -n cgal-dev mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 --yes

Some are OS dependent.

.. tabs::

    .. tab-item:: Windows
        :active:

        .. code-block:: bash

            conda install -n cgal-dev mpir  --yes

    .. tab-item:: OSX / Linux

        .. code-block:: bash

            conda install -n cgal-dev gmp --yes


Activate the environment.

.. code-block:: bash

    conda activate cgal-dev


Change to the root folder of the :mod:`compas_cgal` repo.

.. code-block:: bash

    cd compas_cgal


And compile the code while creating an editable install of the python wrapper using ``setuptools``.

.. code-block:: bash

    pip install -e .


To add your c++ module to the wrapper, and potentially exclude some of the existing modules during its development,
modify the list of extension modules in the ``setup.py`` file.

.. code-block:: python

    ext_modules = [
        Extension(
            'compas_cgal._cgal',
            sorted([
                'src/compas_cgal.cpp',
                'src/compas.cpp',
                'src/meshing.cpp',
                'src/booleans.cpp',
                'src/slicer.cpp',
                'src/intersections.cpp',
                'src/measure.cpp',
                'src/triangulations.cpp',
                'src/subdivision.cpp',
            ]),
            include_dirs=[
                './include',
                get_eigen_include(),
                get_pybind_include()
            ],
            library_dirs=[
                get_library_dirs(),
            ],
            libraries=['mpfr', 'gmp'],
            language='c++'
        ),
    ]
