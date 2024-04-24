"""Provides a scripting component.
    Inputs:
        x: The x script variable
        y: The y script variable
    Output:
        a: The a output variable"""

__author__ = "petras"
__version__ = "2023.04.02"

import rhinoscriptsyntax as rs
import ghpythonlib.treehelpers as th
from Rhino.Geometry import Polyline
from Rhino.Geometry import Transform
from Rhino.Geometry import Vector3d
#import Grasshopper
from Rhino.Geometry import Plane

from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs

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
    
    def RunScript(self, _data, _planes, _ids):
        
        if(_data is None):
            return
        
        target_planes = _planes if len(_planes) > 0 else [Plane.WorldXY]
        _plines =  Grasshopper.DataTree[Polyline]()
        
        if(len(_ids)==0 or len(_planes) != len(_ids)):
            
            for i in range(len(_data[0])):
                xform = Transform.PlaneToPlane(self.get_plane(_data[0][i][-1],_data[0][i][-2][0]),target_planes[i%len(target_planes)])
                    
                for j in range(len(_data[0][i])):
                    polyline_copy = Polyline(_data[0][i][j])
                    polyline_copy.Transform(xform)
                    _plines.Add(polyline_copy,Grasshopper.Kernel.Data.GH_Path(i))
        else:
            total_element_count = len(_data[0])
            for i in range(len(_ids)):
                local_id = _ids[i]
                if(local_id >= total_element_count or local_id < 0 ):
                    continue
                xform = Transform.PlaneToPlane(self.get_plane(_data[0][local_id][0],_data[0][local_id][1][0]),target_planes[i%len(target_planes)])
                for j in range(len(_data[0][local_id])):
                    polyline_copy = Polyline(_data[0][local_id][j])
                    polyline_copy.Transform(xform)
                    _plines.Add(polyline_copy,Grasshopper.Kernel.Data.GH_Path(i))
        
        _plines_oriented = _plines
        
        """
        _data_xform = _plines
        _insertion_vectors =  th.list_to_tree(_data[1])
        _joints_per_face =  th.list_to_tree(_data[2])
        _three_valence = th.list_to_tree(_data[3])
        _adjacency =th.list_to_tree(_data[4])
        """
        
        # return outputs if you have them; here I try it for you:
        return _plines_oriented
