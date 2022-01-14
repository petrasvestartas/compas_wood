********************************************************************************
joints
********************************************************************************
* **Description:** Find joints using face_to_face or plane_to_face search
* **Input:** a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom))
* **Output:** Tuple of 1) a nested list of neighbours 2) polylines of connection areas, 3) types = 10 - SS (255/255.0,0/255.0,0/255.0),  Rotate 11 - SS OUT OF PLANE (255/255.0,0/255.0,100/255.0), 12 - SS IN Plane (206/255,0,88/255.0),  20 Top-Side (0/255.0,146/255.0,210/255.0), 30 - Cross (200/255.0, 174/255.0, 102/255.0)
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.

Data-set: ss_21
""""""""""""""""""""""""""""
.. figure:: /_images/joints_ss_21.jpg
    :figclass: figure
    :class: figure-img img-fluid

Data-set: c_0
""""""""""""""""""""""""""""
.. figure:: /_images/joints_c_0.jpg
    :figclass: figure
    :class: figure-img img-fluid

Data-set: c_1
""""""""""""""""""""""""""""
.. figure:: /_images/joints_c_1.jpg
    :figclass: figure
    :class: figure-img img-fluid

Data-set: c_15
""""""""""""""""""""""""""""
.. figure:: /_images/joints_c_15.jpg
    :figclass: figure
    :class: figure-img img-fluid

Data-set: ss_14
""""""""""""""""""""""""""""
.. figure:: /_images/joints_ss_14.jpg
    :figclass: figure
    :class: figure-img img-fluid


Data-set: annen_small_polylines
""""""""""""""""""""""""""""""""
.. figure:: /_images/joints_annen_small_polylines.jpg
    :figclass: figure
    :class: figure-img img-fluid




.. literalinclude:: test_joints.py
    :language: python