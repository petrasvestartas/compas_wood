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
    
    def RunScript(self, _data):
        
        if(_data is None):
            return
        
        _plines =  Grasshopper.DataTree[Polyline]()
        for i in range(len(_data[0])):
            _plines.AddRange(_data[0][i],Grasshopper.Kernel.Data.GH_Path(i))
        
        
        _insertion_vectors =  th.list_to_tree(_data[1])
        _joints_per_face =  th.list_to_tree(_data[2])
        _three_valence = th.list_to_tree(_data[3])
        _adjacency =th.list_to_tree(_data[4])
        
        # return outputs if you have them; here I try it for you:
        return (_plines, _insertion_vectors, _joints_per_face, _three_valence, _adjacency)
