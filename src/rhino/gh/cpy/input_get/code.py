"""unwrap element data to polylines, insertion vectors, joint types and etc"""
from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System

__author__ = "petras"
__version__ = "2023.04.02"

import ghpythonlib.treehelpers as th
from Rhino.Geometry import Polyline
import Rhino
import rhinoscriptsyntax as rs


class MyComponent(component):
    def RunScript(self, _data, _split: bool):

        if _data is None:
            return

        split = True if _split is None else _split

        _plines_0 = Grasshopper.DataTree[Rhino.Geometry.Curve]()
        _plines_1 = Grasshopper.DataTree[Rhino.Geometry.Curve]()
        _lines = Grasshopper.DataTree[Rhino.Geometry.Curve]()
        if split == False:
            for i in range(len(_data.plines_with_joints)):
                for j in range(len(_data.plines_with_joints[i])):
                    _plines_0.Add(_data.plines_with_joints[i][j].ToNurbsCurve(), Grasshopper.Kernel.Data.GH_Path(i))
        else:
            for i in range(len(_data.plines_with_joints)):
                for j in range(len(_data.plines_with_joints[i])):
                    if _data.plines_with_joints[i][j].Count == 2:
                        _lines.Add(_data.plines_with_joints[i][j].ToNurbsCurve(), Grasshopper.Kernel.Data.GH_Path(i))
                    else:
                        if j % 2 == 0:
                            print(_data.plines_with_joints[i][j])
                            _plines_0.Add(
                                _data.plines_with_joints[i][j].ToNurbsCurve(), Grasshopper.Kernel.Data.GH_Path(i)
                            )
                        else:
                            _plines_1.Add(
                                _data.plines_with_joints[i][j].ToNurbsCurve(), Grasshopper.Kernel.Data.GH_Path(i)
                            )

        _insertion_vectors = th.list_to_tree(_data.insertion_vectors)
        _joints_per_face = th.list_to_tree(_data.joints_per_face)
        _three_valence = th.list_to_tree(_data.three_valence)
        _adjacency = th.list_to_tree(_data.adjacency)
        _cut_types = th.list_to_tree(_data.cut_types)

        # return outputs if you have them; here I try it for you:
        return (
            _plines_0,
            _plines_1,
            _lines,
            _insertion_vectors,
            _joints_per_face,
            _three_valence,
            _adjacency,
            _cut_types,
        )
