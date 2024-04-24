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

class MyComponent(component):
    
    def get_plane(self, _polyline, _orientation_point):
        
        len = _polyline.Count - 1
        normal = Vector3d(0,0,0)
        
        for i in range((_polyline.Count - 1)):
            num = ((i - 1) + len) % len
            item1 = ((i + 1) + len) % len
            point3d =_polyline[num]
            point3d1 =_polyline[item1]
            item2 = _polyline[i]
            normal = normal + Vector3d.CrossProduct(Vector3d(item2 - point3d), Vector3d(point3d1 - item2))
        
        normal.Unitize()
        plane = Plane(_polyline.CenterPoint(), normal)
        if(plane.DistanceTo(_orientation_point)<0):
            plane.Flip()
        
        return plane
    
    
    
    def RunScript(self, _plines, _insertion_vectors, _joints_per_face, _three_valence, _adjacency):
        
        
        
        plines = []
        planes = []
        if( _plines.DataCount > 0):
            for i in range(_plines.BranchCount):
                plines.append(_plines.Branch(i))
                # create planes for orientation
                
                planes.append(self.get_plane(_plines.Branch(i)[0],_plines.Branch(i)[1][0]))
        
        insertion_vectors =  th.tree_to_list(_insertion_vectors) if _insertion_vectors.DataCount > 0 else []
        joints_per_face =  th.tree_to_list(_joints_per_face) if _joints_per_face.DataCount > 0 else []
        three_valence =  th.tree_to_list(_three_valence) if _three_valence.DataCount > 0 else []
        adjacency = th.tree_to_list(_adjacency) if _adjacency.DataCount > 0 else []
        
        # output
        _data = [[plines,insertion_vectors,joints_per_face,three_valence,adjacency, planes]]
        
        
        # return outputs if you have them; here I try it for you:
        return _data
