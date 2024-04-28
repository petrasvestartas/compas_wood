"""display assembly sequence"""
__author__ = "petras"
__version__ = "2023.04.13"

from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs


import rhinoscriptsyntax as rs
import Rhino.Geometry as rg
from Rhino.Geometry import BoundingBox
from Rhino.Geometry import Point3d
from Rhino.Geometry import Plane
from Rhino.Geometry import Polyline
from Rhino.Geometry import Vector3d
from Rhino.Geometry import Line
from Rhino.Geometry import TextEntity
from Rhino.Geometry import Transform
from System.Drawing import Color
import math


class MyComponent(component):

    bbox = BoundingBox.Unset
    lines = []
    insertion_vectors_current = []
    joint_per_face_current_text_entity = []
    meshes = []
    breps = []
    material = Rhino.Display.DisplayMaterial(Color.FromArgb(200, 200, 200))

    def DrawViewportWires(self, args):

        for o in self.breps:
            for i in o.Curves3D:
                args.Display.DrawCurve(i, Color.FromArgb(0, 0, 0))
            args.Display.DrawBrepShaded(o, self.material)
        for o in self.meshes:
            args.Display.DrawMeshShaded(o, self.material)
            args.Display.DrawMeshWires(o, Color.Black, 1)

    def get_ClippingBox(self):
        return self.bbox

    def display_insersion_sequence(self, _geos, _lines, _t):

        text_size = 50
        line_scale = 300

        ###############################################################################
        # input
        ###############################################################################
        self.bbox = BoundingBox.Unset
        self.lines = _lines if len(_lines) > 0 else [Line(Point3d(0, 0, 0), Point3d(0, 0, 1))]
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.breps = []
        self.meshes = []

        if len(_geos) == 0:
            return

        _n = 1
        group_count = _n if _n is not None else 1
        t = _t if _t is not None else 0.0
        item = self.lines[0]
        self.bbox = _geos[0].GetBoundingBox(False)

        ###############################################################################
        # process
        ###############################################################################
        for i in range(1, len(self.lines)):
            self.bbox.Union(_geos[i].GetBoundingBox(False))

        for i in range(0, int(math.floor(t))):
            if isinstance(_geos[i], Rhino.Geometry.Mesh):
                self.meshes.append(_geos[i])
            elif isinstance(_geos[i], Rhino.Geometry.Brep):
                self.breps.append(_geos[i])

        if t % 1 > 0:

            id2 = int(math.floor(t))
            geos_current = []

            for i in range(group_count):
                geos_current.append(_geos[id2])

            for m in range(len(geos_current)):
                item = self.lines[id2]
                line_from = item.From
                item = self.lines[id2]
                to = line_from - item.To

                geo = geos_current[m].Duplicate()
                geo.Transform(Transform.Translation(to * (1 - t % 1)))
                geos_current[m] = geo

            for i in geos_current:
                if isinstance(i, Rhino.Geometry.Mesh):
                    self.meshes.append(i)
                elif isinstance(i, Rhino.Geometry.Brep):
                    self.breps.append(i)

        return

    def RunScript(
        self,
        _geos: System.Collections.Generic.List[Rhino.Geometry.GeometryBase],
        _dir: System.Collections.Generic.List[Rhino.Geometry.Line],
        _t: float,
    ):

        if not _geos:
            return

        lines = _dir
        if not _dir:
            first_geo_bbox = _geos[0].GetBoundingBox(False)
            pmin = Rhino.Geometry.Point3d(0, 0, first_geo_bbox.Min.Z)
            pmax = Rhino.Geometry.Point3d(0, 0, first_geo_bbox.Max.Z)
            line = Rhino.Geometry.Line(pmin, pmax)
            lines = [line] * len(_geos)

        self.display_insersion_sequence(_geos, lines, _t)
