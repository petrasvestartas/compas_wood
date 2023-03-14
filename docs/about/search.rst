********************************************************************************
Search Methods
********************************************************************************
.. |check| raw:: html

    <input checked=""  type="checkbox">

.. |uncheck| raw:: html

    <input type="checkbox">

User Inputs:
    * User does not give any input therefore, in this case "Global" + "Local" search methods are used.
    * User knows which elements (excluding faces) to search for, thus "Local" search method is used.

Search Types:
    * **0** - face-to-face search
    * **1** - plane-to-face search
    * **2** - both **0** and **1** Search
    * The reason to split the search types is to avoid having to cross connections and side connection in the same point. 

Special Case:
    * You want to cut one side of element without joints e.g. in foundations. In this case use adjacency input where v0=v1 and f0=f1.

    
Global:
    *  |check| Closest Object Query + OOB and ABB collision
    *  |check| no search by user given index (must have input in the beginning) 
    *  |check| Polyline Search 

Local:
    *  |check| face_to_face side-to-side parallel in-plane | **joinery_library 1-9** | type 12
    *  |check| face_to_face side-to-side parallel out-of-plane | **joinery_library 10-19** | type 11
    *  |check| face_to_face top-to-side | **joinery_library 20-29** | type 20
    *  |check| plane_to_face cross | **joinery_library 30-39** | type 30
    *  |check| face_to_face top_to_top | **joinery_library 40-49** |  type 40  currently only output joint_area with 0 local search
    *  |check| face_to_face side-to-side | **joinery_library non-parallel 50-59** |  type 50 
    *  |check| border_to_face border | **joinery_library border 60-69** |  type 60 



.. rst-class:: table table-bordered

.. list-table::
   :widths: auto
   :header-rows: 1

   * - **skip**
     - **side-to-side edge in-plane**
     - **side-to-side edge out-of-plane**
     - **top-to-side edge plane**
     - **cross cutting in-plane**
     - **top-to-top edge plane**
     - **side-to-side edge rotated**
     - **boundary**
   * -  
     -
     -
     - 
     - 
     - 
     -
     -
   * - 0 
     - 1-9
     - 10-19
     - 20-29
     - 30-39
     - 40-49
     - 50-59
     - 60-69


Local Search and Insertion Vector:
    *  A vector on an element edge that is equal to a plane normal on an edge.

.. figure:: /_images/about/insertion_vectors_0.jpg
    :figclass: figure
    :class: figure-img img-fluid

.. figure:: /_images/about/insertion_vectors_2.jpg
    :figclass: figure
    :class: figure-img img-fluid
