********************************************************************************
All
********************************************************************************

.. |check| raw:: html

    <input checked=""  type="checkbox">

.. |uncheck| raw:: html

    <input type="checkbox">

################################################################################
Scope - Pair-wise Connections
################################################################################



.. figure:: /_images/Slide1.jpg
    :figclass: figure
    :class: figure-img img-fluid

################################################################################
Algorithm Structure
################################################################################

.. figure:: /_images/Slide8.jpg
    :figclass: figure
    :class: figure-img img-fluid


################################################################################
Joint Types
################################################################################



Joint parameters are given in a list following this order:
    * side-to-side parallel in-plane |  side-to-side parallel | side-to-side out-of-plane |  top-to-side | cross | top-to-top |  side-to-side non-parallel 
    * | division length | shift | type | 
    * { 1000, 0.5, 1,  1000, 0.5, 10 ,  1000, 0.5, 20 ,  1000, 0.5, 30 ,  1000, 0.5, 40 ,  1000, 0.5, 50 }

Code implementation:
    * how to a custom construct joint
        *   string, name of a joint e.g. "ss_e_ip_0"
        *   std::vector<CGAL_Polyline> m[2] - 2 plines lists, where 1st list are bottom and 2nd are top plines
        *   std::vector<CGAL_Polyline> f[2] - 2 plines lists, where 1st list are bottom and 2nd are top plines
        *   std::vector<char> - list of boolean types for the "m" array, length matches m list length
        *   std::vector<char> - list of boolean types for the "f" array, length matches f list length
    * |check| joint class that stores adjacency information
    * |check| change-basis transformation from unit tile to joint volume defined by two rectangles
    * |check| assignment of joints based and search categories and connection types
    * |uncheck| store each parameterized joint inside std::unordered_map<string, joint> that is not remapped yet



**Joints in the Joint Library:**

.. rst-class:: table table-bordered

.. list-table::
   :widths: auto
   :header-rows: 1

   * - Types_0 
     - Types_1-9
     - Types_10-19
     - Types_20-29
     - Types_30-39
     - Types_40-49
     - Types_50-59
   * - **skip**
     - **side-to-side**
     - **side-to-side**
     - **top-to-side**
     - **cross**
     - **top-to-top**
     - **side-to-side**
   * -  
     - **in-plane**
     - **out-of-plane**
     - 
     - 
     - 
     - **rotated**
   * -  
     - **default** ss_e_ip_1
     - **default** ss_e_op_1
     - **default** ts_e_p_3
     - **default** cr_c_ip_0
     - **default** 
     - **default** 
   * - 
     - 1 - (ss_e_ip_1)
     - 10 - (ss_e_op_1)
     - 20 - (ts_e_p_1)
     - 30 - (cr_c_ip_0)
     - 
     -
   * - 
     - 2 - (ss_e_ip_0)
     - 11 - (ss_e_op_2)
     - 21 - (ts_e_p_2) 
     - 31 - (cr_c_ip_1)
     - 
     - 
   * - 
     - 
     - 12 - (ss_e_op_0)
     - 22 - (ts_e_p_3)
     - 
     - 
     - 
   * - 
     - 
     - 
     - 23 - (ts_e_p_0)
     - 
     - 
     - 
   * - 
     - **custom** 9   ss_e_ip_9
     - **custom** 19   ss_e_op_9
     - **custom** 29   ts_e_p_9
     - **custom** 39   cr_c_ip_9
     - **custom** 49   tt_e_p_9 
     - **custom** 59   ss_e_r_9 

Joints have two properties m_boolean_type and f_boolean_type to guide the fabrication process:
    * **Default** nothing = '0'
    * **Plates** edge_insertion = '1'
    * **Plates** hole = '2',
    * **Plates** insert_between_multiple_edges = '3'
    * **Beams** slice = '4' //project and make rectangle
    * **Beams** mill = '5',
    * **Beams** mill_project = '6' //project
    * **Beams** cut = '7'
    * **Beams** cut_project = '8' //project
    * **Beams** binary_slice_mill = '9' //project and make rectangle



**Joint: ss_e_op_1 Nejiri Arigata**

.. figure:: /_images/joint_documentation_0.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: ss_e_op_2 Dovetail**

.. figure:: /_images/joint_documentation_1.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: ss_e_ip_0 Dovetail**

.. figure:: /_images/joint_documentation_2.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: tenon mortise ts_e_p_2 (ts_e_p_0 - original) (start and end point of the joint line are skipped)**

.. figure:: /_images/joint_documentation_3.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: Annen joint - ts_e_p_3 (ts_e_p_2 - original)**

.. figure:: /_images/joint_documentation_4.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: cr_c_ip_0 cross**

.. figure:: /_images/joint_documentation_5.jpg
    :figclass: figure
    :class: figure-img img-fluid

**Joint: cr_c_ip_1 conic cross**

.. figure:: /_images/joint_documentation_6.jpg
    :figclass: figure
    :class: figure-img img-fluid




To-do joints types:
    * |uncheck| snap-fit joint
    * |uncheck| keys
    * |uncheck| tenon-mortise beams
    * |uncheck| tenon-mortise half beam
    * |uncheck| scarf
    * |uncheck| screws


To-do others:
    * |uncheck| flipping case
    * |uncheck|  BLT



################################################################################
Element
################################################################################

Code Implementation:
    *  Element is specified as a pair of polylines, with planes for each side, in a beam case it is only a central polylines
    *  |check| j_mf property track joints (joint id, male/female, parameter on edge)    
    *  |uncheck| mesh boolean: a) reference shapes, b) joints polygon pairs

Merge:
    * plate geometry can be merged, for beams boolean difference must be performed
    * |check| Insert face joints inside edge
    * |check| Insert between multiple edges (open polylines + closed polygons)
    * |uncheck| Cut projection and insert in polygon (case when side-side joints are rotated e.g. butterfly)
    * |uncheck| Mesh boolean: a) reference shapes, b) joints polygon pairs

.. figure:: /_images/merge_1.jpg
    :figclass: figure
    :class: figure-img img-fluid

Grouping:
    * |uncheck| Introduce unordered_map to track grouping like sorted lists x;x;x 



################################################################################
Presentation
################################################################################


Element is a List of Joints
********************************************************************************

.. figure:: /_images/Slide2.jpg
    :figclass: figure
    :class: figure-img img-fluid


Element Group
********************************************************************************

.. figure:: /_images/Slide3.jpg
    :figclass: figure
    :class: figure-img img-fluid


Joint - Tiles + Undirected-graph. Tile = Female + Male Cuts
********************************************************************************

.. figure:: /_images/Slide4.jpg
    :figclass: figure
    :class: figure-img img-fluid


Search - Connection Detection
********************************************************************************

.. figure:: /_images/Slide5.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide32.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide34.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide33.jpg
    :figclass: figure
    :class: figure-img img-fluid


Tile - Change-of-basis Transformation
********************************************************************************

.. figure:: /_images/Slide6.jpg
    :figclass: figure
    :class: figure-img img-fluid


Boolean Methods for Digital Cuts
********************************************************************************

.. figure:: /_images/Slide7.jpg
    :figclass: figure
    :class: figure-img img-fluid




Side-to-side Topology
********************************************************************************

.. figure:: /_images/Slide35.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide36.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide44.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide45.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide9.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide10.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide11.jpg
    :figclass: figure
    :class: figure-img img-fluid




.. figure:: /_images/Slide24.jpg
    :figclass: figure
    :class: figure-img img-fluid


Side-to-top Topology
********************************************************************************


.. figure:: /_images/Slide41.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide42.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide12.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide43.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide39.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide37.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide38.jpg
    :figclass: figure
    :class: figure-img img-fluid





.. figure:: /_images/Slide13.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide14.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide15.jpg
    :figclass: figure
    :class: figure-img img-fluid






.. figure:: /_images/Slide21.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide22.jpg
    :figclass: figure
    :class: figure-img img-fluid




.. figure:: /_images/Slide27.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide19.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide20.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide28.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. figure:: /_images/Slide40.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide47.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide23.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide50.jpg
    :figclass: figure
    :class: figure-img img-fluid



Cross Topology
********************************************************************************

.. figure:: /_images/Slide16.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide48.jpg
    :figclass: figure
    :class: figure-img img-fluid





.. figure:: /_images/Slide17.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide18.jpg
    :figclass: figure
    :class: figure-img img-fluid



.. figure:: /_images/Slide29.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide49.jpg
    :figclass: figure
    :class: figure-img img-fluid



Mixed Topology
********************************************************************************

.. figure:: /_images/Slide26.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide30.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide31.jpg
    :figclass: figure
    :class: figure-img img-fluid


.. figure:: /_images/Slide25.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/Slide46.jpg
    :figclass: figure
    :class: figure-img img-fluid

