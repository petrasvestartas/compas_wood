from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs
from Rhino.Geometry import Polyline, Point3d

class MyComponent(component):
    
    def RunScript(self, _cuts0, _cuts1, _type):
        
        
        _custom_joints = []
        _custom_types = []
        type = 39 if _type is None else _type
        
        for i in range(len(_cuts0)):
            _custom_joints.append(_cuts0[i])
            _custom_types.append(type)
        
        for i in range(len(_cuts1)):
            _custom_joints.append(_cuts1[i])
            _custom_types.append(-type)
        
        
        # return outputs if you have them; here I try it for you:
        return (_custom_joints, _custom_types)
