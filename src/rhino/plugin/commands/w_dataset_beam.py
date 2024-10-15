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
    Line,
    Vector3d,
    Curve,
)
import math

import Rhino
from wood_rui import (
    wood_rui_globals,
    ensure_layer_exists,
    add_polylines,
    add_mesh,
    add_insertion_lines,
    add_adjacency,
    add_insertion_vectors,
    add_three_valence,
    add_joint_type,
    add_loft_brep,
)


class Beam:

    def __init__(
        self, _crv: Rhino.Geometry.Curve, _align_vec: Rhino.Geometry.Vector3d, _radius: float, _circ_rect: bool
    ):

        if _crv is None:
            return
        else:
            self._crv = _crv
        if _radius is None:
            self._radius = 0.5
        if _circ_rect is None:
            self._circ_rect = False

        if _circ_rect:
            rects, brep = self.create_pipe_from_curve(_crv, _radius, _align_vec)
            self._brep = brep
            self._plines = rects
        else:
            rects, brep = self.create_rect_pipe_from_curve(_crv, _radius, _align_vec)
            self._brep = brep
            self._plines = rects

        # _brep = Rhino.Geometry.Brep.CreatePipe(_crv, [_radius], False, Rhino.Geometry.PipeCapMode.Flat, False, 0, 0.01)
        # print(_brep)

        # return outputs if you have them; here I try it for you:
        # return (self._brep, self._plines)

    @property
    def polylines(self):
        return self._plines

    @property
    def shape(self):
        return self._brep

    @classmethod
    def from_dataset_cross(
        cls, dataset_name="beams_cross", radius0=2, radius1=2, circle_or_square=True, direction0=None, direction1=None
    ):
        """Polylines and brep reprsenting a round beam in a cross configuration.

        Attributes
        ----------
        dataset_name : str
            Name of the dataset. Geometry is added to this layer childs: polylines and loft.

        """

        # Create rectangles around lines directed to y-axis.
        line0 = Line(Point3d(0, 4.513429, -3.081924), Point3d(28.80821, 4.513429, 3.995666)).ToNurbsCurve()
        line1 = Line(Point3d(3.646118, 4.081924, 11.380378), Point3d(27.534322, 4.513429, -10.948873)).ToNurbsCurve()

        # Set the alignment vector, if user does not specify any, use the corss product
        alignment_vector0: Vector3d = direction0
        alignment_vector1: Vector3d = direction1
        if direction0 is None or direction1 is None:
            vector0: Vector3d = line0.PointAtEnd - line0.PointAtStart
            vector1: Vector3d = line1.PointAtEnd - line1.PointAtStart
            cross_product = Vector3d.CrossProduct(vector0, vector1)
            alignment_vector0 = cross_product
            alignment_vector1 = cross_product

        beam0 = cls(line0, alignment_vector0, radius0, circle_or_square)
        beam1 = cls(line1, alignment_vector1, radius1, circle_or_square)

        # Extract rectangles.
        wood_rui_globals.init_data(dataset_name)
        polylines = []
        polylines.extend(beam0.polylines)
        polylines.extend(beam1.polylines)

        # Add the polylines and shape to the global dataset and rhino.
        add_polylines(polylines, dataset_name)
        add_loft_brep([beam0.shape, beam1.shape], dataset_name)

    @classmethod
    def from_dataset_side_to_side(
        cls,
        dataset_name="beams_side_to_side",
        radius0=5,
        radius1=5,
        circle_or_square=True,
        direction0=None,
        direction1=None,
    ):
        """Polylines and brep reprsenting a round beam in a side-to-side configuration.

        Attributes
        ----------
        dataset_name : str
            Name of the dataset. Geometry is added to this layer childs: polylines and loft.

        """

        # Create rectangles around lines directed to y-axis.
        line0 = Line(Point3d(50, 50, 0), Point3d(0, 50, 0)).ToNurbsCurve()
        line1 = Line(Point3d(0, 50, 0), Point3d(-50, 50, 10)).ToNurbsCurve()

        # Set the alignment vector, if user does not specify any, use the corss product
        alignment_vector0: Vector3d = direction0
        alignment_vector1: Vector3d = direction1
        if direction0 is None or direction1 is None:
            vector0: Vector3d = line0.PointAtEnd - line0.PointAtStart
            vector1: Vector3d = line1.PointAtEnd - line1.PointAtStart
            cross_product = Vector3d.CrossProduct(vector0, vector1)
            alignment_vector0 = cross_product
            alignment_vector1 = cross_product

        beam0 = cls(line0, alignment_vector0, radius0, circle_or_square)
        beam1 = cls(line1, alignment_vector1, radius1, circle_or_square)

        # Cut rectangles so that male element would meet at the face of the female element.
        beam0.polylines[0], beam0.polylines[1], beam1.polylines[0], beam1.polylines[1] = (
            Beam.cut_pair_of_rectangles_side_to_side(
                line0, line1, beam0.polylines[0], beam0.polylines[1], beam1.polylines[0], beam1.polylines[1]
            )
        )
        polylines = []
        polylines.extend(beam0.polylines)
        polylines.extend(beam1.polylines)

        # Add the polylines and shape to the global dataset and rhino.
        add_polylines(polylines, dataset_name)
        add_loft_brep([beam0.shape, beam1.shape], dataset_name)

    @classmethod
    def from_dataset_side_to_top(
        cls,
        dataset_name="beams_side_to_top",
        radius0=5,
        radius1=5,
        circle_or_square=True,
        direction0=None,
        direction1=None,
    ):
        """Polylines and brep reprsenting a round beam in a side-to-top configuration.

        Attributes
        ----------
        dataset_name : str
            Name of the dataset. Geometry is added to this layer childs: polylines and loft.

        """
        # Create rectangles around lines directed to y-axis.
        line0 = Line(Point3d(0, 100, 0), Point3d(-50, 100, 0)).ToNurbsCurve()
        line1 = Line(Point3d(-53.536687, 100, 47.754182), Point3d(-19.440882, 100, -15.660927)).ToNurbsCurve()

        # Set the alignment vector, if user does not specify any, use the corss product
        alignment_vector0: Vector3d = direction0
        alignment_vector1: Vector3d = direction1
        if direction0 is None or direction1 is None:
            vector0: Vector3d = line0.PointAtEnd - line0.PointAtStart
            vector1: Vector3d = line1.PointAtEnd - line1.PointAtStart
            cross_product = Vector3d.CrossProduct(vector0, vector1)
            alignment_vector0 = cross_product
            alignment_vector1 = cross_product

        beam0 = cls(line0, alignment_vector0, radius0, circle_or_square)
        beam1 = cls(line1, alignment_vector0, radius1, circle_or_square)

        # Extract rectangles.
        wood_rui_globals.init_data(dataset_name)

        # Cut rectangles so that male element would meet at the face of the female element.
        beam0.polylines[0], beam0.polylines[1], beam1.polylines[0], beam1.polylines[1] = (
            Beam.cut_pair_of_rectangles_side_to_top(
                line0, line1, beam0.polylines[0], beam0.polylines[1], beam1.polylines[0], beam1.polylines[1]
            )
        )
        polylines = []
        polylines.extend(beam0.polylines)
        polylines.extend(beam1.polylines)

        # Add the polylines and shape to the global dataset and rhino.
        add_polylines(polylines, dataset_name)
        add_loft_brep([beam0.shape, beam1.shape], dataset_name)

    @staticmethod
    def cut_pair_of_rectangles_side_to_top(line0, line1, r00, r01, r10, r11, tolerance=0.01):

        # Find which element is male and female based on curve intersection.
        line0.Domain = Interval(0, 1)
        line1.Domain = Interval(0, 1)
        curve_intersections: Rhino.Geometry.Intersect.CurveIntersections = (
            Rhino.Geometry.Intersect.Intersection.CurveCurve(line0, line1, tolerance, tolerance)
        )

        if curve_intersections.Count == 0:
            return False

        curve_intersection: Rhino.Geometry.Intersect.IntersectionEvent = curve_intersections[0]
        p0: Point3d = curve_intersection.PointA
        p1: Point3d = curve_intersection.PointB
        t0: float = curve_intersection.ParameterA
        t1: float = curve_intersection.ParameterB

        # male is the one that has intersection point farest from the middle
        distance_to_center0 = abs(0.5 - t0)
        distance_to_center1 = abs(0.5 - t1)

        male = [line0, r00, r01, t0, p0]
        female = [line1, r10, r11, t1, p1]

        swap: bool = False
        if distance_to_center0 < distance_to_center1:
            male, female = female, male
            swap = True

        # Find which rectangle is closest to the point, by creating a plane from rectangle and measuring the distance from the place to the point
        farest_male_point_from_intersection: Point3d = (
            male[0].PointAtStart
            if male[-1].DistanceTo(male[0].PointAtStart) > male[-1].DistanceTo(male[0].PointAtEnd)
            else male[0].PointAtEnd
        )
        result0, female_plane0 = Plane.FitPlaneToPoints(female[1])
        result1, female_plane1 = Plane.FitPlaneToPoints(female[2])
        distance_from_plane0_to_intersection_point: float = female_plane0.DistanceTo(
            farest_male_point_from_intersection
        )
        distance_from_plane1_to_intersection_point: float = female_plane1.DistanceTo(
            farest_male_point_from_intersection
        )
        cut_plane: Plane = (
            female_plane0
            if distance_from_plane0_to_intersection_point < distance_from_plane1_to_intersection_point
            else female_plane1
        )

        # Cut male polylines 2nd and 4th segments  with the nearest male plane.
        cut_plane: Plane = (
            cut_plane
            if cut_plane.DistanceTo(farest_male_point_from_intersection) > 0
            else Plane(cut_plane.Origin, cut_plane.XAxis, -cut_plane.YAxis)
        )

        def cut_4_point_polyline(rectangle, cut_plane):
            l00: Line = rectangle.SegmentAt(1)
            l01: Line = rectangle.SegmentAt(3)
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l00, cut_plane)
            line_side: bool = cut_plane.DistanceTo(l00.To) > 0
            l00 = Line(l00.PointAt(t), l00.To) if line_side else Line(l00.From, l00.PointAt(t))
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l01, cut_plane)
            l01 = Line(l01.From, l01.PointAt(t)) if line_side else Line(l01.PointAt(t), l01.To)
            rectangle_cut: Polyline = Rhino.Geometry.Polyline([l00.From, l00.To, l01.From, l01.To, l00.From])
            return rectangle_cut

        male_cut_r00 = cut_4_point_polyline(male[1], cut_plane)
        male_cut_r01 = cut_4_point_polyline(male[2], cut_plane)

        if swap:
            return r00, r01, male_cut_r00, male_cut_r01
        else:
            return male_cut_r00, male_cut_r01, r00, r01

    @staticmethod
    def cut_pair_of_rectangles_side_to_side(line0, line1, r00, r01, r10, r11, tolerance=0.01):

        # Find which element is male and female based on curve intersection.
        line0.Domain = Interval(0, 1)
        line1.Domain = Interval(0, 1)
        curve_intersections: Rhino.Geometry.Intersect.CurveIntersections = (
            Rhino.Geometry.Intersect.Intersection.CurveCurve(line0, line1, tolerance, tolerance)
        )

        if curve_intersections.Count == 0:
            return False

        curve_intersection: Rhino.Geometry.Intersect.IntersectionEvent = curve_intersections[0]
        p0: Point3d = curve_intersection.PointA
        p1: Point3d = curve_intersection.PointB
        t0: float = curve_intersection.ParameterA
        t1: float = curve_intersection.ParameterB

        # male is the one that has intersection point farest from the middle
        distance_to_center0 = abs(0.5 - t0)
        distance_to_center1 = abs(0.5 - t1)

        male = [line0, r00, r01, t0, p0]
        female = [line1, r10, r11, t1, p1]

        swap: bool = False
        if distance_to_center0 < distance_to_center1:
            male, female = female, male
            swap = True

        # Find which rectangle is closest to the point, by creating a plane from rectangle and measuring the distance from the place to the point
        farest_male_point_from_intersection: Point3d = (
            male[0].PointAtStart
            if male[-1].DistanceTo(male[0].PointAtStart) > male[-1].DistanceTo(male[0].PointAtEnd)
            else male[0].PointAtEnd
        )
        result0, female_plane0 = Plane.FitPlaneToPoints(female[1])
        result1, female_plane1 = Plane.FitPlaneToPoints(female[2])
        distance_from_plane0_to_intersection_point: float = female_plane0.DistanceTo(
            farest_male_point_from_intersection
        )
        distance_from_plane1_to_intersection_point: float = female_plane1.DistanceTo(
            farest_male_point_from_intersection
        )
        cut_plane: Plane = (
            female_plane0
            if distance_from_plane0_to_intersection_point < distance_from_plane1_to_intersection_point
            else female_plane1
        )

        # Cut male polylines 2nd and 4th segments  with the nearest male plane.
        cut_plane: Plane = (
            cut_plane
            if cut_plane.DistanceTo(farest_male_point_from_intersection) > 0
            else Plane(cut_plane.Origin, cut_plane.XAxis, -cut_plane.YAxis)
        )

        def cut_4_point_polyline(rectangle, cut_plane):
            l00: Line = rectangle.SegmentAt(1)
            l01: Line = rectangle.SegmentAt(3)
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l00, cut_plane)
            line_side: bool = cut_plane.DistanceTo(l00.To) > 0
            l00 = Line(l00.PointAt(t), l00.To) if line_side else Line(l00.From, l00.PointAt(t))
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l01, cut_plane)
            l01 = Line(l01.From, l01.PointAt(t)) if line_side else Line(l01.PointAt(t), l01.To)
            rectangle_cut: Polyline = Rhino.Geometry.Polyline([l00.From, l00.To, l01.From, l01.To, l00.From])
            return rectangle_cut

        male_cut_r00 = cut_4_point_polyline(male[1], cut_plane)
        male_cut_r01 = cut_4_point_polyline(male[2], cut_plane)

        if swap:
            return r00, r01, male_cut_r00, male_cut_r01
        else:
            return male_cut_r00, male_cut_r01, r00, r01

    def align_plane(self, plane, v):

        p = Plane(plane)

        result, parameterAlongXDirection, parameterAlongYDirection = p.ClosestParameter(p.Origin + v)
        angle = math.atan2(parameterAlongXDirection, parameterAlongYDirection)
        p.Rotate(-angle + 1.5707963267949, p.ZAxis, p.Origin)

        return p

    def create_rectangles(self, rail_curve, radius, vector, divisions=10):
        rects = []
        divisions_refined = 1 if self._crv.IsLinear(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance) else divisions
        for i in range(divisions_refined + 1):
            t = (1.0 / divisions_refined) * i
            result, plane = rail_curve.PerpendicularFrameAt(rail_curve.Domain.ParameterAt(t))
            scale = 1
            if vector:
                plane = self.align_plane(plane, vector)
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

    def create_pipe_from_curve(self, rail_curve, radius, vector, divisions=10):

        rects = self.create_rectangles(rail_curve, radius, vector, divisions)

        localblending = False
        cap = Rhino.Geometry.PipeCapMode.Flat
        fitrail = True
        abstol = 0.1
        angtol = math.radians(15)
        brep = list(Rhino.Geometry.Brep.CreatePipe(rail_curve, radius, localblending, cap, fitrail, abstol, angtol))[0]

        return rects, brep

    def create_rect_pipe_from_curve(self, rail_curve, radius, vector, divisions=10):

        # if the curve is not linear then divide the line into segments
        rects = self.create_rectangles(rail_curve, radius, vector, divisions)

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
    # Beam.from_dataset_cross()
    Beam.from_dataset_side_to_side()
    # Beam.from_dataset_side_to_top()
