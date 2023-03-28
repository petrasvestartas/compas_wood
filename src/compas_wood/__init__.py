"""

.. currentmodule:: compas_wood


.. toctree::
    :maxdepth: 1

joinery module
==============

.. autosummary::
    :toctree: generated/
    :nosignatures:

    joinery.test
    joinery.read_xml_polylines
    joinery.read_xml_polylines_and_properties
    joinery.get_connection_zones
    joinery.closed_mesh_from_polylines
    joinery.joints
    joinery.rtree


conversions module
==================

.. autosummary::
    :toctree: generated/
    :nosignatures:

    conversions.WoodVectorInt_to_list
    conversions.list_to_WoodVectorInt
    conversions.WoodNestedVectorInt_to_lists
    conversions.lists_to_WoodNestedVectorInt
    conversions.WoodVectorDouble_to_list
    conversions.list_to_WoodVectorDouble
    conversions.WoodNestedVectorDouble_to_lists
    conversions.lists_to_WoodNestedVectorDouble
    conversions.lists_of_vectors_to_WoodNestedVectorDouble
    conversions.WoodNestedVectorDouble_to_polylines
    conversions.WoodVectorFloat_and_WoodVectorInt_to_mesh
    conversions.polylines_to_WoodNestedVectorDouble
    conversions.WoodNestedVectorDouble_to_vectorslist
    conversions.WoodNestedNestedVectorDouble_to_polylineslists
    conversions.WoodNestedNestedVectorDouble_to_polylineslist
    conversions.WoodVectorDouble_to_box
    conversions.WoodNestedVectorDouble_to_boxeslist

"""


from __future__ import print_function

import os


__author__ = ["Petras Vestartas"]
__copyright__ = "Petras Vestartas"
__license__ = "GNU Lesser General Public License"
__email__ = "petrasvestartas@gmail.com"
__version__ = "1.0.0"


HERE = os.path.dirname(__file__)

HOME = os.path.abspath(os.path.join(HERE, "../../"))
DATA = os.path.abspath(os.path.join(HOME, "data"))
DOCS = os.path.abspath(os.path.join(HOME, "docs"))
TEMP = os.path.abspath(os.path.join(HOME, "temp"))

__all_plugins__ = [
    "compas_wood.joinery",
]


__all__ = ["HOME", "DATA", "DOCS", "TEMP"]
