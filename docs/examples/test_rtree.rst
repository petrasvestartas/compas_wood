********************************************************************************
rtree
********************************************************************************
* **Description:** Find neighbors of polyline pairs using the R-Tree search
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** Tuple of 1) a nested list of neighbours 2) AABB box meshes, 3) OBB  box meshes
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.


Data-set: ss_0
""""""""""""""""""""""""""""

.. figure:: /_images/rtree.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: test_rtree.py
    :language: python

Data-set: annen_small_polylines
""""""""""""""""""""""""""""
.. figure:: /_images/rtree.gif
    :figclass: figure
    :class: figure-img img-fluid