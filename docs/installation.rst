********************************************************************************
Installation
********************************************************************************



.. _header1:

Clone Github Repository
========

.. Header 1.1
.. ----------

.. Header 1.1.1
.. ~~~~~~~~~~~~

.. Header 1.1.1.1
.. """"""""""""""

.. Documentation cheatsheet https://docs.typo3.org/m/typo3/docs-how-to-document/main/en-us/WritingReST/CheatSheet.html



Windows
""""""""""""""

.. code-block:: bash

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood
    cd compas_wood
    pip install -e .


Mac
""""""""""""""

.. code-block:: bash

    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https:/github.com/petrasvestartas/compas_wood
    cd compas_wood 
    pip install -e . 


Video 
""""""""""""""

https://user-images.githubusercontent.com/18013985/147587822-cfb43c96-4a81-4b89-987f-8d201ec2e5ba.mp4


Notes
""""""""""""""
* Be sure that you activate ```wood-dev``` after installation and you are in ```compas_wood``` folder before writing ```pip install -e .```

* For more detailed instructions please follow  `ReadMe <https://github.com/petrasvestartas/compas_wood>`_.