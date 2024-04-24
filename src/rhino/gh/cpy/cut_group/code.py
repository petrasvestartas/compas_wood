from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs

class MyComponent(component):
    
    def RunScript(self, _geometry, _group_id):
        
        
        
        tree_temp = Grasshopper.DataTree[Rhino.Geometry.GeometryBase]()
        
        # flatten to the first index
        for i in range(_geometry.BranchCount):
            path = _geometry.Paths[i]
            tree_temp.AddRange(_geometry.Branch(i),Grasshopper.Kernel.Data.GH_Path(path.Indices[0]))
        
        
        
        # group
        tree = Grasshopper.DataTree[Rhino.Geometry.GeometryBase]()
        for i in range(_group_id.BranchCount):
            for j in _group_id.Branch(i):
                path = Grasshopper.Kernel.Data.GH_Path(j)
                if tree_temp.PathExists(path ):
                    tree.AddRange(tree_temp.Branch(path),_group_id.Paths[i])
        # output
        _result = tree
        
        # return outputs if you have them; here I try it for you:
        return _result
