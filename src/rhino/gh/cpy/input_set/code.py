"""combine different data sets into one list for a faster data transder between grasshopper wires"""

from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython

__author__ = "petras"
__version__ = "2023.04.02"


import System
import Rhino
import rhinoscriptsyntax as rs
import ghpythonlib.treehelpers as th
from Rhino.Geometry import Polyline
from Rhino.Geometry import Vector3d
from Rhino.Geometry import Plane
import compas_wood
from compas_wood.binding import test
from compas_wood.datastructures import WoodData

# r: compas


class MyComponent(component):
    def get_plane(self, _polyline, _orientation_point):

        len = _polyline.Count - 1
        normal = Vector3d(0, 0, 0)

        for i in range((_polyline.Count - 1)):
            num = ((i - 1) + len) % len
            item1 = ((i + 1) + len) % len
            point3d = _polyline[num]
            point3d1 = _polyline[item1]
            item2 = _polyline[i]
            normal = normal + Vector3d.CrossProduct(Vector3d(item2 - point3d), Vector3d(point3d1 - item2))

        normal.Unitize()
        plane = Plane(_polyline.CenterPoint(), normal)
        if plane.DistanceTo(_orientation_point) < 0:
            plane.Flip()

        return plane

    def RunScript(
        self,
        _polylines: Grasshopper.DataTree[Rhino.Geometry.Polyline],
        _vectors: Grasshopper.DataTree[Rhino.Geometry.Vector3d],
        _joint_types: Grasshopper.DataTree[int],
        _three_valence: Grasshopper.DataTree[int],
        _adjacency: Grasshopper.DataTree[int],
    ):

        plines = []
        planes = []
        if _polylines.DataCount > 0:
            for i in range(_polylines.BranchCount):
                plines.extend(list(_polylines.Branch(i)))
                planes.append(self.get_plane(_polylines.Branch(i)[0], _polylines.Branch(i)[1][0]))

        insertion_vectors = th.tree_to_list(_vectors) if _vectors.DataCount > 0 else []
        joints_per_face = th.tree_to_list(_joint_types) if _joint_types.DataCount > 0 else []
        three_valence = th.tree_to_list(_three_valence) if _three_valence.DataCount > 0 else []
        adjacency = th.tree_to_list(_adjacency) if _adjacency.DataCount > 0 else []

        # output
        _data = WoodData(plines, insertion_vectors, joints_per_face, three_valence, adjacency, planes)

        # return outputs if you have them; here I try it for you:
        return _data
