from ghpythonlib.componentbase import executingcomponent as component
import System
import Rhino
from Rhino.Geometry import BoundingBox
from Rhino.Geometry import Point3d
from Rhino.Geometry import Plane
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
    polylines = []

    def DrawViewportWires(self, args):

        plane = Plane.WorldXY
        for line in self.lines:
            args.Display.DrawLineArrow(line, Color.FromArgb(207, 0, 90), 2, 100)
        for polyline in self.polylines:
            args.Display.DrawPolyline(polyline, Color.FromArgb(207, 0, 90), 2)
        for insertionVectorsCurrent in self.insertion_vectors_current:
            if insertionVectorsCurrent.Length < 0.05:
                continue
            args.Display.DrawLine(insertionVectorsCurrent, Color.FromArgb(0, 0, 0), 5)
        for jointPerFaceCurrent in self.joint_per_face_current_text_entity:
            result, plane = Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.ActiveViewport.GetCameraFrame()
            plane.Origin = jointPerFaceCurrent.Plane.Origin
            args.Display.Draw3dText(
                jointPerFaceCurrent.PlainText,
                jointPerFaceCurrent.MaskColor,
                plane,
                jointPerFaceCurrent.TextHeight,
                "Arial",
                False,
                False,
                Rhino.DocObjects.TextHorizontalAlignment.Center,
                Rhino.DocObjects.TextVerticalAlignment.BottomOfTop,
            )

    def get_ClippingBox(self):
        return self.bbox

    def display_insertion_sequence(self, _data, _lines, _n, _t):

        # ==============================================================================
        # clear input
        # ==============================================================================
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        self.lines = []
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.polylines = []

        text_size = 50
        line_scale = 300

        ###############################################################################
        # input
        ###############################################################################
        self.bbox = BoundingBox.Unset
        self.lines = _lines if len(_lines) > 0 else [Line(Point3d(0, 0, 0), Point3d(0, 0, 1))]
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.polylines = []

        group_count = _n if _n is not None else 4
        t = _t if _t is not None else 0.0
        item = self.lines[0]
        self.bbox = self.lines[0].BoundingBox

        ###############################################################################
        # process
        ###############################################################################
        for i in range(1, len(self.lines)):
            self.bbox.Union(self.lines[i].BoundingBox)

        for i in range(0, int(math.floor(t))):
            for j in range(group_count):
                self.polylines.append(_data.plines[(i * group_count + j) * 2])
                self.polylines.append(_data.plines[(i * group_count + j) * 2 + 1])

        if t % 1 > 0:

            id2 = int(math.floor(t))
            polylines_current = []

            for i in range(group_count):
                polylines_current.append(_data.plines[(id2 * group_count + i) * 2])
                polylines_current.append(_data.plines[(id2 * group_count + i) * 2 + 1])

            for m in range(len(polylines_current)):

                item = self.lines[id2]
                line_from = item.From
                item = self.lines[id2]
                to = line_from - item.To

                polyline = polylines_current[m].Duplicate()
                polyline.Transform(Transform.Translation(to * (1 - t % 1)))
                polylines_current[m] = polyline

            for i in polylines_current:
                self.polylines.append(i)

            for n in range(0, len(polylines_current), 2):

                point3ds = [polylines_current[n].CenterPoint(), polylines_current[n + 1].CenterPoint()]

                for o in range(0, polylines_current[n].Count - 1):
                    p0 = (polylines_current[n][o] + polylines_current[n][o + 1]) * 0.5
                    p1 = (polylines_current[n + 1][o] + polylines_current[n + 1][o + 1]) * 0.5
                    pmid = (p0 + p1) * 0.5
                    point3ds.append(pmid)

                for p in range(len(point3ds)):

                    face_vectors = _data.insertion_vectors[id2 * 4 + int(n * 0.5)][p]

                    if face_vectors == Vector3d.Zero:
                        face_vectors = Vector3d(0, 0, 0.0001)

                    self.insertion_vectors_current.append(
                        Line(
                            point3ds[p] + (face_vectors * line_scale * 0.25), point3ds[p] + (face_vectors * line_scale)
                        )
                    )
                    text = str(_data.joints_per_face[int(n * 0.5)][p])
                    textEntity = TextEntity()
                    textEntity.Plane = Plane(point3ds[p], Vector3d.ZAxis)
                    textEntity.PlainText = text

                    if _data.joints_per_face[int(n * 0.5)][p] > 0:
                        textEntity.MaskColor = Color.FromArgb(0, 0, 0)
                        textEntity.TextHeight = (float)(text_size)
                    else:
                        textEntity.MaskColor = Color.FromArgb(100, 100, 100)
                        textEntity.TextHeight = (float)(text_size * 0.5)
                    self.joint_per_face_current_text_entity.append(textEntity)

        # return outputs if you have them here I try it for you:
        return

    def RunScript(self, _data, _dir: System.Collections.Generic.List[Rhino.Geometry.Line], _n: int, _t: float):

        if not _data:
            return

        directions = []
        if not _dir:
            number_of_directions = len(_data.plines_with_joints)
            directions = [Rhino.Geometry.Line(Point3d(0, 0, 0), Point3d(0, 0, 1000))] * number_of_directions
        else:
            directions = _dir

        if not _t:
            return

        self.display_insertion_sequence(_data, directions, _n, _t)
