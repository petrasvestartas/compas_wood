********************************************************************************
get_connection_zones ss_0
********************************************************************************
* **Description:** generates joints for elements connected side-to-side and top-to-side. Input and output images are shown below. The example show multiple different joint selection, insertio vector direction and three valence joint selection.
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** a list of polylines with added joints
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.




.. figure:: /_images/ss_0.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: test_get_connection_zones_ss_0.py
    :language: python