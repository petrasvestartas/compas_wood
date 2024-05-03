"""match data tree of beam-nodes component with the output of the solver"""
from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino


class MyComponent(component):
    def RunScript(self,
            _geometry: Grasshopper.DataTree[Rhino.Geometry.GeometryBase],
            _group_id: Grasshopper.DataTree[int]):

        tree_temp = Grasshopper.DataTree[Rhino.Geometry.GeometryBase]()

        # flatten to the first index to bring lofted cuts to a single joint
        for i in range(_geometry.BranchCount):
            path = _geometry.Paths[i]
            tree_temp.AddRange(_geometry.Branch(i), Grasshopper.Kernel.Data.GH_Path(path.Indices[0]))

        # group
        tree = Grasshopper.DataTree[Rhino.Geometry.GeometryBase]()
        all_indices = _group_id.AllData()

        for i, element_idx in enumerate(all_indices):
            path = Grasshopper.Kernel.Data.GH_Path(element_idx)
            tree.AddRange(tree_temp.Branch(i), path)

        return tree
