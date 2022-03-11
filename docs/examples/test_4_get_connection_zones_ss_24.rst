********************************************************************************
4_get_connection_zones ss_24
********************************************************************************

* **Description:** generates joints for elements connected side-to-side. Input and output images are shown below.
* **Input:**  **annen_small_polylines** - a list of polylines (the list of top and bottom outlines are stored sequentially (0-top, 0-bottom, 1-top, 1-bottom ... N-top, N-bottom)), **annen_small_edge_directions** - insertion vectors, **annen_small_edge_joints** - joint types, **annen_small_three_valance_element_indices_and_instruction()** - alignment of joing for special 3 valence tenon-mortise joints
* **Output:** a list of polylines with added joints
* **Tests:** for testing purpose the polylines are stored in compas_wood.data files. Input and output geomety is visualized below.


.. figure:: /_images/ss_24.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: test_4_get_connection_zones_ss_24.py
    :language: python


**Grasshopper Version**

.. figure:: /_images/annen_1.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_0.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. figure:: /_images/annen_2.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_3.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_4.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_5.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_6.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_7.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_8.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_9.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/annen_10.jpg
    :figclass: figure
    :class: figure-img img-fluid