********************************************************************************
4_get_connection_zones vda_2
********************************************************************************
* **Description:** generates joints for elements connected side-to-side and top-to-side. Input and output images are shown below. The example show multiple different joint selection, insertio vector direction and three valence joint selection.
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** a list of polylines with added joints
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.

The data-set is taken from  `Lina Vestarte project <https://www.linavestarte.com/Corrugated-Cardboard-Shell-1-1/>`_.


.. figure:: /_images/vda_0.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/vda_2.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: test_4_get_connection_zones_vda_2.py
    :language: python