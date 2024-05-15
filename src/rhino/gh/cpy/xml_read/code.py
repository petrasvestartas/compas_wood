from ghpythonlib.componentbase import executingcomponent as component  # noqa: E402
import Grasshopper as gh
import Rhino.Geometry as rg
import clr
import System

clr.AddReference("System.Xml")
import System.Xml  # noqa: E402


class write_component(component):
    def RunScript(self, _path):

        if _path is None:
            return False
        ################################################################################################################
        # Load document
        ################################################################################################################
        doc = System.Xml.XmlDocument()
        doc.Load(_path)

        ################################################################################################################
        # Start document
        ################################################################################################################
        polylines_tree = gh.DataTree[rg.Polyline]()
        types_tree = gh.DataTree[str]()

        # Iterate polyline groups
        group_count_a = 0
        group_count_b = 0
        for node in doc.DocumentElement.ChildNodes:

            if node.Name == "Polyline" or node.Name == "polyline":
                polyline = rg.Polyline()
                # Iterate polylines
                for node_0 in node.ChildNodes:
                    if node_0.Name == "point":

                        # Iterate points
                        # Convert coordinates to points
                        p = rg.Point3d(
                            float(node_0.ChildNodes[0].InnerText),
                            float(node_0.ChildNodes[1].InnerText),
                            float(node_0.ChildNodes[2].InnerText),
                        )

                    polyline.Add(p)

                if polyline.Count > 0:
                    path = gh.Kernel.Data.GH_Path(group_count_a)
                    polylines_tree.Add(polyline, path)

                group_count_a = group_count_a + 1

            elif node.Name == "type_group":

                for node_0 in node.ChildNodes:
                    types_tree.Add((node_0.ChildNodes[0].InnerText), gh.Kernel.Data.GH_Path(group_count_b))
                group_count_b = group_count_b + 1

        return polylines_tree, types_tree
