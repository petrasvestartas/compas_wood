import System
from ghpythonlib.componentbase import executingcomponent as component
import clr

clr.AddReference("System.Xml")
import System.Xml  # noqa: E402


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

    def RunScript(self, _plines, _path):
        if _path is None or _plines.AllData().Count == 0:
            return False
        ##########################################################################
        # Create document
        ##########################################################################
        xmlWriter = System.Xml.XmlWriter.Create(str(_path))
        xmlWriter.WriteStartDocument()
        xmlWriter.WriteStartElement("input_polylines")
        ##########################################################################
        # Start document
        ##########################################################################
        for pline in _plines.AllData():
            self.xml_polyline(xmlWriter, pline, "Polyline")

        ##########################################################################
        # Close document
        ##########################################################################
        xmlWriter.WriteEndDocument()
        xmlWriter.Close()

        return True
