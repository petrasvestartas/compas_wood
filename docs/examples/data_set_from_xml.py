from compas.files import XML
from compas.geometry import Polyline
from compas.geometry import Point
import os


folder = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data_set.xml")
print("\n")
print(folder)
print("\n")
xml = XML.from_file(folder)
#xml.read()
