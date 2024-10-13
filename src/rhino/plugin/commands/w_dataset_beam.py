#! python3
from typing import *
from wood_rui import wood_rui_globals, NamedValuesForm  # Import the singleton instance
from compas_wood.binding import wood_globals

import Rhino
from Rhino.Geometry import (
    Brep,
    Polyline,
    Plane,
    Point3d,
    Rectangle3d,
    Interval,
    LoftType,
    BrepSolidOrientation,
)
import math


class Beam(component):

    def __init__(self,
            _crv: Rhino.Geometry.Curve,
            _align_vec: Rhino.Geometry.Vector3d,
            _radius: float,
            _circ_rect: bool):

        if _crv is None:
            return
        if _radius is None:
            self._radius = 0.5
        if _circ_rect is None:
            self._circ_rect = False


        if self._circ_rect:
            self.rects, self.brep = create_pipe_from_curve(_crv, _radius, _align_vec)
            self._brep = brep
            self._plines = rects
        else:
            self.rects, self.brep = create_rect_pipe_from_curve(_crv, _radius, _align_vec)
            self._brep = brep
            self._plines = rects

        # _brep = Rhino.Geometry.Brep.CreatePipe(_crv, [_radius], False, Rhino.Geometry.PipeCapMode.Flat, False, 0, 0.01)
        # print(_brep)

        # return outputs if you have them; here I try it for you:
        return (_brep, _plines)

    def polylines(self):
        return self._plines

    def shape(self):
        return self._brep

    def align_plane(plane, v):

        p = Plane(plane)

        result, parameterAlongXDirection, parameterAlongYDirection = p.ClosestParameter(p.Origin + v)
        angle = math.atan2(parameterAlongXDirection, parameterAlongYDirection)
        p.Rotate(-angle + 1.5707963267949, p.ZAxis, p.Origin)

        return p

    def create_rectangles(rail_curve, radius, vector, divisions=10):
        rects = []
        divisions_refined = 1 if _crv.IsLinear(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance) else divisions
        for i in range(divisions_refined + 1):
            t = (1.0 / divisions_refined) * i
            result, plane = rail_curve.PerpendicularFrameAt(rail_curve.Domain.ParameterAt(t))
            scale = 1
            if vector:
                plane = align_plane(plane, vector)
            rect = Rectangle3d(
                plane, Interval(-radius * scale, radius * scale), Interval(-radius * scale, radius * scale)
            )
            rects.append(rect.ToPolyline())

        if len(rects) == 2:
            pline0 = Polyline([rects[0][0], rects[0][1], rects[1][1], rects[1][0]])
            pline0.Add(pline0[0])
            pline1 = Polyline([rects[0][3], rects[0][2], rects[1][2], rects[1][3]])
            pline1.Add(pline1[0])
            rects = [pline0, pline1]
        return rects

    def create_pipe_from_curve(rail_curve, radius, vector, divisions=10):

        rects = create_rectangles(rail_curve, radius, vector, divisions)

        localblending = False
        cap = Rhino.Geometry.PipeCapMode.Flat
        fitrail = True
        abstol = 0.1
        angtol = math.radians(15)
        brep = list(
            Rhino.Geometry.Brep.CreatePipe(rail_curve, radius, localblending, cap, fitrail, abstol, angtol)
        )[0]

        return rects, brep

    def create_rect_pipe_from_curve(rail_curve, radius, vector, divisions=10):

        # if the curve is not linear then divide the line into segments
        rects = create_rectangles(rail_curve, radius, vector, divisions)

        curves = []
        for c in rects:
            curves.append(c.ToNurbsCurve())
        brep = Brep.CreateFromLoft(curves, Point3d.Unset, Point3d.Unset, LoftType.Normal, False)[0]

        if brep:
            brep.Faces.SplitKinkyFaces()
            brep = brep.CapPlanarHoles(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
            if brep:
                if brep.SolidOrientation == BrepSolidOrientation.Inward:
                    brep.Flip()

        return rects, brep


if __name__ == "__main__":
    pass