from ghpythonlib.componentbase import executingcomponent as component
import Rhino
from System.Drawing import Color
import System


class connections_zones(component):
    def project_curves_to_plane(self, curves):
        points = []
        for curve in curves:
            for point in curve.ToNurbsCurve().Points:
                points.append(point.Location)

        result, plane = Rhino.Geometry.Plane.FitPlaneToPoints(points)

        xform = Rhino.Geometry.Transform.PlanarProjection(plane)
        curves_projected = []
        for curve in curves:
            curve_projected = curve.DuplicateCurve()
            curve_projected.Transform(xform)
            curves_projected.append(curve_projected)
        return curves_projected

    def loft_polylines_with_holes(self, curves0, curves1, color):
        """Loft polylines with holes

        Parameters
        ----------
        curves0 : list[rg.Polyline]
        curves1 : list[rg.Polyline]

        Returns
        -------
        rg.Brep
            brep as lofted polylines and cap them
        """

        ###############################################################################
        # user input
        ###############################################################################

        if curves0 == [None]:
            return
        if curves1 == [None]:
            return

        if len(curves0) == 0:
            return
        if len(curves1) == 0:
            return

        flag = len(curves0) is not 0 if True else len(curves1) != 0
        if flag:
            curves = []
            curves2 = []

            flag0 = len(curves1) == 0
            flag1 = len(curves0) == 0 and len(curves1) != 0
            flag2 = len(curves0) and len(curves1)

            if flag0:
                for i in range(len(curves0)):
                    if float(i) >= float(len(curves0)) * 0.5:
                        curves2.Add(curves0[i])
                    else:
                        curves.Add(curves0[i])
            elif flag1:
                for i in range(0, len(curves1), 2):
                    curves.Add(curves1[i])
                    curves2.Add(curves1[i + 1])
            elif flag2:
                curves = curves0
                curves2 = curves1

            curves0 = curves
            curves1 = curves2

        ###############################################################################
        # Create mesh of the bottom face
        ###############################################################################

        curves0 = self.project_curves_to_plane(curves0)
        brep_0 = Rhino.Geometry.Brep.CreatePlanarBreps(curves0, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

        curves1 = self.project_curves_to_plane(curves1)
        brep_1 = Rhino.Geometry.Brep.CreatePlanarBreps(curves1, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

        ###############################################################################
        # Create lofts
        ###############################################################################
        breps_to_join = [brep_0, brep_1]
        for i in range(len(curves0)):
            wall = Rhino.Geometry.Brep.CreateFromLoft(
                [curves0[i], curves1[i]],
                Rhino.Geometry.Point3d.Unset,
                Rhino.Geometry.Point3d.Unset,
                Rhino.Geometry.LoftType.Normal,
                False,
            )[0]
            breps_to_join.append(wall)

        ###############################################################################
        # Join Brep
        ###############################################################################

        solid = Rhino.Geometry.Brep.JoinBreps(breps_to_join, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

        faces = []
        for i in range(solid.Faces.Count):
            faces.append(solid.Faces[i].DuplicateFace(False))

        solid = Rhino.Geometry.Brep.JoinBreps(faces, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)[0]

        if solid:
            solid.Faces.SplitKinkyFaces(Rhino.RhinoMath.DefaultAngleTolerance, True)

            if Rhino.Geometry.BrepSolidOrientation.Inward is solid.SolidOrientation:
                solid.Flip()

        if color is not None:
            for i in range(solid.Faces.Count):
                solid.Faces[i].PerFaceColor = color
        else:
            for i in range(solid.Faces.Count):
                if i < 2:
                    solid.Faces[i].PerFaceColor = Color.LightGray  # Color.LightGray
                else:
                    solid.Faces[i].PerFaceColor = Color.DeepPink

        return solid

    def RunScript(
        self,
        _p0: System.Collections.Generic.List[Rhino.Geometry.Curve],
        _p1: System.Collections.Generic.List[Rhino.Geometry.Curve],
        _c: System.Drawing.Color,
    ):
        if _p0 and _p1:
            return self.loft_polylines_with_holes(_p0, _p1, _c)
