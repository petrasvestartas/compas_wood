"""compas_wood: COMPAS-friendly interface to the wood_nano timber joinery kernel.

The compiled kernel lives in the ``wood_nano`` package (nanobind bindings of
the wood C++ library). This package only converts between COMPAS types and the
kernel's raw containers - all computation happens in C++.
"""

__author__ = ["Petras Vestartas"]
__copyright__ = "Petras Vestartas"
__license__ = "MIT License"
__email__ = "petrasvestartas@gmail.com"
__version__ = "3.0.0"

from compas_wood.translation_shell import translation_shell_elements
from compas_wood.reflex_fold import reflex_fold_elements
from compas_wood.chevron import (
    chevron_elements,
    chevron_elements_annen,
    chevron_elements_nurbs,
)
from compas_wood.reciprocal_move import (
    reciprocal_move_elements,
    reciprocal_move_elements_from_mesh,
    reciprocal_move_elements_from_surface,
)
from compas_wood.reciprocal_rotation import (
    reciprocal_rotation_elements,
    reciprocal_rotation_elements_from_mesh,
    reciprocal_rotation_elements_from_surface,
)
from compas_wood.diamond_mesh import (
    diamond_mesh_elements,
    diamond_mesh_elements_annen,
    diamond_mesh_elements_from_surface,
)
from compas_wood.connectors import connectors_elements
from compas_wood.joinery_solver import (
    joinery_solver_elements,
    SEARCH_FACE_TO_FACE,
    SEARCH_CROSS_JOINT,
    SEARCH_BOTH,
)
from compas_wood.wood_element import WoodElement, JoineryElement, JointResult

# Reserved for a future compas_viewer scene-object registration module
# (compas_wood.scene). Empty on purpose: nothing is registered in 3.0.
__all_plugins__ = []

__all__ = [
    "translation_shell_elements",
    "reflex_fold_elements",
    "chevron_elements",
    "chevron_elements_annen",
    "chevron_elements_nurbs",
    "reciprocal_move_elements",
    "reciprocal_move_elements_from_mesh",
    "reciprocal_move_elements_from_surface",
    "reciprocal_rotation_elements",
    "reciprocal_rotation_elements_from_mesh",
    "reciprocal_rotation_elements_from_surface",
    "diamond_mesh_elements",
    "diamond_mesh_elements_annen",
    "diamond_mesh_elements_from_surface",
    "connectors_elements",
    "joinery_solver_elements",
    "SEARCH_FACE_TO_FACE",
    "SEARCH_CROSS_JOINT",
    "SEARCH_BOTH",
    "WoodElement",
    "JoineryElement",
    "JointResult",
]
