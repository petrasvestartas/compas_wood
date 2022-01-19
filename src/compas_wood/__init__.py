"""
********************************************************************************
compas_wood
********************************************************************************

.. currentmodule:: compas_wood


.. toctree::
    :maxdepth: 1

Functions
=========

joinery_solver
----------------
.. autosummary::
    :toctree: generated/
    :nosignatures:

    joinery.test
    joinery.rtree
    joinery.joints
    joinery.get_connection_zones
    joinery.closed_mesh_from_polylines
    joinery.closest_polylines
    joinery.beam_volumes

"""

from __future__ import print_function

import os


__author__ = ["Petras Vestartas"]
__copyright__ = "Petras Vestartas"
__license__ = "MIT License"
__email__ = "petrasvestartas@gmail.com"
__version__ = "0.1.1"


HERE = os.path.dirname(__file__)

HOME = os.path.abspath(os.path.join(HERE, "../../"))
DATA = os.path.abspath(os.path.join(HOME, "data"))
DOCS = os.path.abspath(os.path.join(HOME, "docs"))
TEMP = os.path.abspath(os.path.join(HOME, "temp"))

__all_plugins__ = [
    "compas_cgal.joinery",
]


__all__ = ["HOME", "DATA", "DOCS", "TEMP"]
