********************************************************************************
closest_polylines
********************************************************************************
* **Description:** Find closest polylines within given distance
* **Input:** polylines : list of polylines, segment_radii : radius per each polyline segment, used to construct boxes around segments, min_distance : distance until which search is valid
* **Output:** neighbours_list, point_pairs
* **Tests:** for testing purpose the polylines are stored in data_set_beams.py


Data-set: polylines_0:
""""""""""""""""""""""""""""
.. figure:: /_images/closest_polylines_1.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: polylines_1 and polylines_2 :
""""""""""""""""""""""""""""""""""""""""""""""""
.. figure:: /_images/closest_polylines_3.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: polylines_3 :
""""""""""""""""""""""""""""
.. figure:: /_images/closest_polylines_4.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: polylines_4 :
""""""""""""""""""""""""""""
.. figure:: /_images/closest_polylines_5.jpg
    :figclass: figure
    :class: figure-img img-fluid 



.. literalinclude:: test_closest_polylines.py
    :language: python