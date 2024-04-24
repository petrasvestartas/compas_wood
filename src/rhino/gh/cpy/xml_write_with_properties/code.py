__author__ = "petras vestartas"
__version__ = "2023.03.28"

"""
Write polylines with properties to xml file
"""

from ghpythonlib.componentbase import executingcomponent as component
import clr
clr.AddReference("System.Xml")
import System.Xml

class write_component(component):

    def xml_polyline(self, xmlWriter, pline, name):
        xmlWriter.WriteStartElement(name) 
        for p in pline:
            xmlWriter.WriteStartElement("point") 
            
            xmlWriter.WriteStartElement("x") 
            xmlWriter.WriteString(p.X.ToString()) 
            xmlWriter.WriteEndElement() 
            
            xmlWriter.WriteStartElement("y") 
            xmlWriter.WriteString(p.Y.ToString()) 
            xmlWriter.WriteEndElement() 
            
            xmlWriter.WriteStartElement("z") 
            xmlWriter.WriteString(p.Z.ToString()) 
            xmlWriter.WriteEndElement() 
            
            xmlWriter.WriteEndElement() 
        
        xmlWriter.WriteEndElement() 

    def xml_polyline_vectors(self, xmlWriter, vectors, name):
        
        xmlWriter.WriteStartElement(name)
        for v in vectors:
            xmlWriter.WriteStartElement("vector")
            
            xmlWriter.WriteStartElement("x")
            xmlWriter.WriteString(v.X.ToString())
            xmlWriter.WriteEndElement()
            
            xmlWriter.WriteStartElement("y")
            xmlWriter.WriteString(v.Y.ToString())
            xmlWriter.WriteEndElement()
            
            xmlWriter.WriteStartElement("z")
            xmlWriter.WriteString(v.Z.ToString())
            xmlWriter.WriteEndElement()
            
            xmlWriter.WriteEndElement()
        xmlWriter.WriteEndElement()

    def xml_polyline_ids(self, xmlWriter, ids, name):
        xmlWriter.WriteStartElement(name)
        for id in ids:
            xmlWriter.WriteStartElement("id")
            xmlWriter.WriteString(id.ToString())
            xmlWriter.WriteEndElement()
        xmlWriter.WriteEndElement()
    
    def RunScript(
        self,
        _plines,
        _insertion_vectors,
        _joints_types,
        _type_of_three_valence,
        _three_valence,
        _adjacency,
        _path
    ):
        
        if(_path == None or _plines.AllData().Count == 0):
            return False
        
        
        ##########################################################################
        # Create document
        ##########################################################################
        xmlWriter = System.Xml.XmlWriter.Create(_path) 
    
        ##########################################################################
        # Start document
        ##########################################################################
    
        xmlWriter.WriteStartDocument() 
        xmlWriter.WriteStartElement("input_polylines") 
        ##########################################################################
        # _plines
        ##########################################################################
    
        for pline in _plines.AllData():
            self.xml_polyline(xmlWriter, pline, "polyline") 
    
    
        ##########################################################################
        # _insertion_vectors
        ##########################################################################
        if(_insertion_vectors.AllData().Count != 0):
            for i in range(_insertion_vectors.BranchCount):
                self.xml_polyline_vectors(xmlWriter, _insertion_vectors.Branch(i), "insertion_vectors") 
    
    
        ##########################################################################
        # _joints_per_face
        ##########################################################################
        if(_joints_types.AllData().Count != 0):
            for i in range(_joints_types.BranchCount):
                self.xml_polyline_ids(xmlWriter, _joints_types.Branch(i), "joints_types") 
    
    
        ##########################################################################
        # _three_valence | we indicate which 3 valence it is -> 0 - Annen, 1 - Vidy
        ##########################################################################
        if(_three_valence.AllData().Count != 0):
            self.xml_polyline_ids(xmlWriter, [_type_of_three_valence], "three_valence")
            for i in range(_three_valence.BranchCount):
                self.xml_polyline_ids(xmlWriter, _three_valence.Branch(i), "three_valence") 
    
    
        ##########################################################################
        # _adjacency, sides are not given so set them to -1
        ##########################################################################
        if(_adjacency.AllData().Count != 0):
            adjacency_no_sides = _adjacency.AllData() 
            adjacency = []
    
            for i in range(0, adjacency_no_sides.Count, 2):
                adjacency.append(adjacency_no_sides[i + 0]) 
                adjacency.append(adjacency_no_sides[i + 1]) 
                adjacency.append(-1) 
                adjacency.append(-1) 
    
            self.xml_polyline_ids(xmlWriter, adjacency, "adjacency") 
    
    
    
        ##########################################################################
        # Close document
        ##########################################################################
        xmlWriter.WriteEndDocument() 
        xmlWriter.Close() 
        
        return True