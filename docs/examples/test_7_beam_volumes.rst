********************************************************************************
7_beam_volumes
********************************************************************************
* **Description:** Draw rectangle volumes around polylines segments
* **Input:** polylines : list of polylines, segment_radii : radius per each polyline segment, used to construct boxes around segments, segment_vectors : radius per each polyline segment, used to construct boxes around segments, allowed_types : -1 - all possible types, 0 - end-to-end, 1 - cross or side-to-end, this option is needed because multiple joint can be made in one intersection, min_distance : double distance until which search is valid, volume_length : double length of beam volumes, cross_or_side_to_end : double type0_type1_parameter, flip_male : property for side-to-end volumes, 0 - no shift, -1 shift to right, 1 shift to left
* **Output:** neighbours_list : list of element indices of polyline0_id_segment0_id_polyline1_id_segment1_id, pair_points_list : a list of 2 points, pair_volumes_list : a list of 4 polyline, joint_areas_list : a list of closed polylines within local connection detection, joint_types_list : a list of integers of joint area types
* **Tests:** for testing purpose the polylines are stored in data_set_beams.py

Data-set: polylines_0 :
""""""""""""""""""""""""""""""""""""""""""""""""
.. figure:: /_images/beam_volumes_0c.jpg
    :figclass: figure
    :class: figure-img img-fluid 

Data-set: polylines_1 :
""""""""""""""""""""""""""""""""""""""""""""""""
.. figure:: /_images/beam_volumes_1c.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: polylines_3 :
""""""""""""""""""""""""""""""""""""""""""""""""
.. figure:: /_images/beam_volumes_3b.jpg
    :figclass: figure
    :class: figure-img img-fluid 


Data-set: polylines_4 :
""""""""""""""""""""""""""""""""""""""""""""""""
.. figure:: /_images/beam_volumes_4b.jpg
    :figclass: figure
    :class: figure-img img-fluid 

    

.. literalinclude:: test_7_beam_volumes.py
    :language: python