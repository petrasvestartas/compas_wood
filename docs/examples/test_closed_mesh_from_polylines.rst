********************************************************************************
closed_mesh_from_polylines
********************************************************************************
* **Description:** generates joints for elements connected side-to-side and top-to-side. Input and output images are shown below. The example show multiple different joint selection, insertio vector direction and three valence joint selection.
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** a list of polylines with added joints
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.



Data-set: ss_24
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_ss_24.jpg
    :figclass: figure
    :class: figure-img img-fluid


Data-set: ss_17
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_ss_17.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: ss_14
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_ss_14.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: ss_10
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_ss_10.jpg
    :figclass: figure
    :class: figure-img img-fluid


Data-set: ss_7
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_ss_7.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: c_0
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_0.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: c_10
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_10.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: c_13
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_13.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: c_14
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_14.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: c_15
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_15.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: c_16
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_c_16.jpg
    :figclass: figure
    :class: figure-img img-fluid 



.. literalinclude:: test_closed_mesh_from_polylines.py
    :language: python