********************************************************************************
closed_mesh_from_polylines
********************************************************************************
* **Description:** Create a mesh from a top and bottom outlines
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** closed valid mesh
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.

Be sure to input the correctly order outlines as image below, if not the side mesh face winding will be inconsistent. And you would need to clean the mesh afterwards (Rhino only, COMPAS does not have good clean up methods).

Input: List of Polylines Order and Winding:
""""""""""""""""""""""""""""
.. figure:: /_images/closed_mesh_from_polylines_input.jpg
    :figclass: figure
    :class: figure-img img-fluid 


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