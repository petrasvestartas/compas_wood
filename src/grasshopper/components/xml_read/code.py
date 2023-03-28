__author__ = "petras vestartas"
__version__ = "2023.03.28"

"""
Read polylines and cutting types from xml file
"""
from ghpythonlib.componentbase import executingcomponent as component
import clr
clr.AddReference("System.Xml")
import System.Xml
import Grasshopper as gh
import Rhino.Geometry as rg


class write_component(component):
    def RunScript(
        self,
        _path
    ):
        
        if(_path == None):
            return False
        #############################################################################################################################################
        ##Load document
        #############################################################################################################################################
        doc = System.Xml.XmlDocument() 
        doc.Load(_path) 

        #############################################################################################################################################
        ##Start document
        #############################################################################################################################################
        polylines_tree = gh.DataTree[rg.Polyline]() 
        types_tree = gh.DataTree[str]()

        # Iterate polyline groups
        group_count_a = 0 
        group_count_b = 0 
        for node in doc.DocumentElement.ChildNodes:
            print(node)
            if(node.Name == "polyline_group"):
                
                # Iterate polylines
                for node_0 in node.ChildNodes:
                    
                    polyline = rg.Polyline() 
                    
                    # Iterate points
                    for node_1 in node_0.ChildNodes:
                        
                        # Convert coordinates to points
                        if(node_1.Name == "point"):
                            p = rg.Point3d(
                            float(node_1.ChildNodes[0].InnerText),
                            float(node_1.ChildNodes[1].InnerText),
                            float(node_1.ChildNodes[2].InnerText)
                            )
                            
                            polyline.Add(p) 
                    polylines_tree.Add(polyline, gh.Kernel.Data.GH_Path(group_count_a))
                    
                group_count_a = group_count_a + 1
                
            elif(node.Name == "type_group"):
                for node_0 in node.ChildNodes:
                    types_tree.Add((node_0.ChildNodes[0].InnerText), gh.Kernel.Data.GH_Path(group_count_b)) 
                group_count_b= group_count_b+1


        #############################################################################################################################################
        # Close document
        #############################################################################################################################################
        __plines = polylines_tree 
        __types = types_tree 

        return polylines_tree, types_tree