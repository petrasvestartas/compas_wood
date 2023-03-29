__author__ = "petras vestartas"
__version__ = "2023.03.22"

"""
Move curves to to the origin
"""

from ghpythonlib.componentbase import executingcomponent as component

import Rhino.Geometry as RhinoGeometry
import Grasshopper
class xml_recenter_component(component):
    def RunScript(self, _geo, _scale):
        
        scale = _scale if _scale != None else 1
        
        if(_geo.AllData().Count == 0):
            return
        
        # get bounding box
        bbox = _geo.AllData()[0].GetBoundingBox(True)
        for g in _geo.AllData():
            bbox.Union(g.GetBoundingBox(True))
        
        # Transformation matrix
        translation = -RhinoGeometry.Vector3d(bbox.Center)
        move_to_center = RhinoGeometry.Transform.Translation(translation)
        scale_xform = RhinoGeometry.Transform.Scale(RhinoGeometry.Point3d.Origin, scale)
        combined = scale_xform * move_to_center
        
        # Apply transformation
        geo_transformed = Grasshopper.DataTree[RhinoGeometry.GeometryBase]()
        for i in range(_geo.BranchCount):
            for j in range(_geo.Branch(i).Count):
                geo_copy = _geo.Branch(i)[j].Duplicate()
                geo_copy.Transform(combined)
                geo_transformed.Add(geo_copy, _geo.Paths[i])
        
        # Output
        return geo_transformed