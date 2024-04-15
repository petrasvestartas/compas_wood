********************************************************************************
type: plates name: side-to-side edge in-plane hilti
********************************************************************************

This tutorial focuses on Hilti CLT connectors. The objective is to demonstrate how to create a model consisting of pairs of polylines that represent timber plates, detect the interfaces between these plates, and create connectors. Subsequently, the geometry is meshed, and a mesh boolean difference is applied for visualization purposes.

Scope of the tutorial:

*    We begin with simple pairwise connections.
*    Then, we apply the process to a planar hexagonal shell.
*    Finally, we test the process on a planar quad shell.

The connector requires a 240x90x93 mm cutout. The LVL timber element measures 128x78.5x90 mm, and the inner thin part is 27.7x40x90 mm. The angles are set at 45 degrees, and the cuts are made with a router with a diameter of up to 40 mm. The minimum distance between two connectors is 300 mm, and the maximum is 3000 mm; the minimum distance to a corner is 300 mm, and the minimum CLT thickness is 120 mm. It can withstand tensile forces of up to 35 kN and shear forces of up to 40 kN.

.. figure:: ./type_plates_name_side_to_side_edge_in_plane_hilti0.png
    :figclass: figure
    :class: figure-img img-fluid


.. literalinclude:: ./type_plates_name_side_to_side_edge_in_plane_hilti_part1.py
    :language: python

