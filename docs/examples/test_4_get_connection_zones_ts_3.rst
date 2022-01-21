********************************************************************************
4_get_connection_zones ts_3
********************************************************************************
* **Description:** generates joints for elements connected side-to-side and top-to-side. Input and output images are shown below. The example show multiple different joint selection, insertio vector direction and three valence joint selection.
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** a list of polylines with added joints
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.




.. figure:: /_images/ts_3.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: test_4_get_connection_zones_ts_3.py
    :language: python