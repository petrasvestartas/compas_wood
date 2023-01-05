using System;
using System.Collections.Generic;
using System.Linq;
using Rhino;
using Rhino.Geometry;
using Rhino.Geometry.Intersect;

namespace rhino_util
{
    public static class PolylineUtil
    {
        public static Polyline CreateSpiral(Polyline x, Polyline y, int divisions)
        {
            //Spiral will always have %2 elements to have the same vertical start and end point

            Polyline[] interpolatedPlines = PointUtil.InterpolatePolylines(y, x, (((int)(divisions)) + ((int)(divisions)) % 2), true);

            Polyline spiral = new Polyline();

            for (int i = 0; i < interpolatedPlines.Length - 1; i++)
            {//i
                for (int j = 0; j < interpolatedPlines[i].Count - 1; j++)
                {
                    Line l = new Line(interpolatedPlines[i][j], interpolatedPlines[i + 1][j]);
                    double t = MathUtil.RemapNumbers(j, 0, interpolatedPlines[i].Count - 1, 0, 1);
                    //double modT = i % 2 == 0 && !x.IsClosed ? t : 1 - t;
                    spiral.Add(l.PointAt(t));
                }

                if (!y.IsClosed)
                {
                    for (int j = interpolatedPlines[i].Count - 1; j >= 0; j--)
                    {
                        spiral.Add(interpolatedPlines[i + 1][j]);
                    }
                }
            }

            if (y.IsClosed)
            {
                spiral.AddRange(interpolatedPlines[interpolatedPlines.Length - 1]);
            }

            spiral.Add(spiral[0]);

            return spiral;
        }

        public static Polyline InsertPointsToPolyline(Polyline P, List<Point3d> L)
        {
            Polyline pline = new Polyline(P);

            for (int i = 0; i < L.Count; i++)
            {
                Point3d[] p = new Point3d[] { L[i] };
                for (int j = 0; j < p.Length; j++)
                {
                    double t = pline.ClosestParameter(p[j]);
                    if (t % 1 == 0) continue;

                    if (P.ClosestPoint(p[j]).DistanceToSquared(p[j]) < 0.0001)
                    {
                        pline.Insert((int)Math.Ceiling(t), p[j]);
                        pline = new Polyline(pline);
                    }
                }
            }

            return pline;
        }

        public static Polyline InsertLineEndPointsToPolyline(Polyline P, List<Line> L)
        {
            Polyline pline = new Polyline(P);

            for (int i = 0; i < L.Count; i++)
            {
                Point3d[] p = new Point3d[] { L[i].From, L[i].To };
                for (int j = 0; j < p.Length; j++)
                {
                    double t = pline.ClosestParameter(p[j]);
                    if (t % 1 == 0) continue;

                    if (P.ClosestPoint(p[j]).DistanceToSquared(p[j]) < 0.0001)
                    {
                        pline.Insert((int)Math.Ceiling(t), p[j]);
                        pline = new Polyline(pline);
                    }
                }
            }

            return pline;
        }

        public static void ExtendTwoRectangles(ref Polyline p0_0, ref Polyline p0_1, double[] extend)
        {
            //Polyline p0_0 = new Polyline(pline0);
            //Polyline p0_1 = new Polyline(pline1);
            p0_0 = p0_0.Extend(0, extend[0], extend[0]);
            p0_0 = p0_0.Extend(2, extend[0], extend[0]);
            p0_0 = p0_0.Extend(1, extend[1], extend[1]);
            p0_0 = p0_0.Extend(3, extend[1], extend[1]);
            p0_1 = p0_1.Extend(0, extend[0], extend[0]);
            p0_1 = p0_1.Extend(2, extend[0], extend[0]);
            p0_1 = p0_1.Extend(1, extend[1], extend[1]);
            p0_1 = p0_1.Extend(3, extend[1], extend[1]);

            Vector3d v = new Vector3d(p0_0[0] - p0_1[0]).Unit();
            p0_0 = p0_0.Translate(v * extend[2]);
            p0_1 = p0_1.Translate(v * -extend[2]);

            //pline0 = p0_0;
            //pline1 = p0_1;
        }

        public static Polyline[] OffsetPolyline(Polyline pline, double d = 1)
        {
            Plane plane = pline.plane();
            Vector3d v = plane.ZAxis;

            if (d == 0)
            {
                return new Polyline[] { pline, pline };
            }
            else if (d > 0)
            {
                return new Polyline[] { pline.Translate(v * d * 0.5), pline.Translate(-v * d * 0.5) };
            }
            else
            {
                return new Polyline[] { pline, pline.Translate(v * d) };
            }
        }

        public static Polyline[] ProjectPolylinesBBox(Polyline P0, Polyline P1)
        {
            //Basis line
            Line l = new Line(P0[0], P1[0]);
            l.Transform(Rhino.Geometry.Transform.Scale(l.PointAt(0.5), 10));

            //Collect points
            List<Point3d> points = new List<Point3d>(10);
            points.AddRange(P0);
            points.AddRange(P1);

            //Find ends of the line by closest point parameters
            double[] t = new double[points.Count];
            for (int i = 0; i < points.Count; i++)
            {
                t[i] = l.ClosestParameter(points[i]);
            }
            Array.Sort(t);

            //Get bounding box planes
            Plane plane0 = new Plane(l.PointAt(t[0]), l.Direction);
            Plane plane1 = new Plane(l.PointAt(t[t.Length - 1]), l.Direction);

            //Project polyline points to planes
            Polyline P0_ = new Polyline();
            Polyline P1_ = new Polyline();

            for (int i = 0; i < P0.Count; i++)
            {
                Line ray = new Line(P0[i], P0[i] + l.Direction);
                double tLine;
                Rhino.Geometry.Intersect.Intersection.LinePlane(ray, plane0, out tLine);
                P0_.Add(ray.PointAt(tLine));
            }

            for (int i = 0; i < P1.Count; i++)
            {
                Line ray = new Line(P1[i], P1[i] + l.Direction);
                double tLine;
                Rhino.Geometry.Intersect.Intersection.LinePlane(ray, plane1, out tLine);
                P1_.Add(ray.PointAt(tLine));
            }

            return new Polyline[] { P0_, P1_ };
        }

        public static Polyline XForm(this Polyline polyline, Transform xform)
        {
            Polyline pline = new Polyline(polyline);
            pline.Transform(xform);
            return pline;
        }

        public static Polyline ToPolyline(this Point3d[] pts, bool close = true)
        {
            Polyline polyline = new Polyline(pts);
            polyline.Close();
            return polyline;
        }

        public static Polyline[] InterpolatePolylines(this Polyline path, Polyline normal, int n = 0)
        {
            Polyline[] interpolatedPolylines = new Polyline[2 + n];

            for (int j = 0; j < 2 + n; j++)
                interpolatedPolylines[j] = new Polyline();

            for (int j = 0; j < path.Count; j++)
            {
                Point3d[] pts = PointUtil.InterpolatePoints(path[j], normal[j], n, true);
                for (int k = 0; k < 2 + n; k++)
                {
                    interpolatedPolylines[k].Add(pts[k]);
                }
            }

            return interpolatedPolylines;
        }

        public static Polyline Extend(this Polyline polyline, int sID, double dist0, double dist1, double proportion0 = 0, double proportion1 = 0)
        {
            Polyline pline = polyline.Duplicate();
            Line l = polyline.SegmentAt(sID);

            //Either scale or extend polyline segments
            if (proportion0 != 0 || proportion1 != 0)
            {
                double len = l.Length;
                l = l.ExtendLine(len * proportion0, len * proportion1);
            }
            else
            {
                l = l.ExtendLine(dist0, dist1);
            }

            pline[sID] = l.From;
            pline[sID + 1] = l.To;

            if (sID == 0)
                pline[pline.Count - 1] = pline[0];
            else if (sID == polyline.SegmentCount - 1)
                pline[0] = pline[pline.Count - 1];

            return pline;
        }

        public static bool ConvertCurveToPolyline(Curve c, out Polyline pl)
        {
            if (c.TryGetPolyline(out pl))
            {
                return pl.IsValid && !(pl.Length < RhinoMath.ZeroTolerance);
            }
            // default options.. should perhaps do something with the document tolerance.
            PolylineCurve polylineCurve = c.ToPolyline(0, 0, 0.1, 0, 0, 0, 0, 0, true);
            if (polylineCurve == null)
            {
                //Component.AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "Unable to convert brep edge to polyline");
                return false;
            }
            if (!polylineCurve.TryGetPolyline(out pl))
            {
                //Component.AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "Unable to convert brep edge to polyline - weird shizzle");
                return false;
            }
            return pl.IsValid && !(pl.Length < RhinoMath.ZeroTolerance);
        }

        public static Polyline FitPline(this List<Point3d> pts)
        {
            Line.TryFitLineToPoints(pts, out Line fit);
            double[] t = new double[pts.Count];
            int[] id = new int[pts.Count];
            for (int i = 0; i < pts.Count; i++)
            {
                t[i] = fit.ClosestParameter(pts[i]);
                id[i] = i;
            }
            Array.Sort(t, id);
            Polyline fitPline = new Polyline();
            for (int i = 0; i < pts.Count; i++)
            {
                fitPline.Add(pts[id[i]]);
            }

            return fitPline;
        }

        public static bool LinePolyline(Line la, Polyline pl, out double t)
        {
            t = 0;
            int c = 0;
            foreach (Line lb in pl.GetSegments())
            {
                if (Rhino.Geometry.Intersect.Intersection.LineLine(la, lb, out double a, out double b, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance, true))
                {//If there is an intersection{
                    t = b + c;
                    return true;
                }
                c++;
            }

            return false;
        }

        public static List<Point3d> LinePolylineInfinite(Line la, Polyline pl)
        {
            List<Point3d> faceFemaleIntersectionPoints = new List<Point3d>(2);

            //Sensitive to tolerance
            foreach (Line lb in pl.GetSegments())
            {
                if (Rhino.Geometry.Intersect.Intersection.LineLine(la, lb, out double t0, out double t1, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 10, false))
                {
                    Point3d p = lb.PointAt(t1);
                    if (pl.ClosestPoint(p).DistanceToSquared(p) < 0.001)
                        faceFemaleIntersectionPoints.Add(lb.PointAt(t1));
                }
            }
            return faceFemaleIntersectionPoints;
        }

        public static Line LinePolylineInfiniteToLine(Line la, Polyline pl)
        {
            List<Point3d> faceFemaleIntersectionPoints = new List<Point3d>(2);

            //Sensitive to tolerance
            foreach (Line lb in pl.GetSegments())
            {
                if (Rhino.Geometry.Intersect.Intersection.LineLine(la, lb, out double t0, out double t1, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 10, false))
                {
                    Point3d p = lb.PointAt(t1);
                    if (pl.ClosestPoint(p).DistanceToSquared(p) < 0.001)
                        faceFemaleIntersectionPoints.Add(lb.PointAt(t1));
                }
            }

            if (faceFemaleIntersectionPoints.Count > 1)
                return new Line(faceFemaleIntersectionPoints.First(), faceFemaleIntersectionPoints.Last());
            return Line.Unset;
        }

        public static Tuple<SortedDictionary<int, Polyline>, SortedDictionary<int, Polyline>> SplitCurvesByPlane(this Polyline pline, Plane plane)
        {
            Polyline pline_ = new Polyline(pline);
            Curve c = pline_.ToNurbsCurve();

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Intersect each segment of curve to get cut parameters
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            List<double> T = new List<double>();
            for (int i = 0; i < pline_.SegmentCount; i++)
            {
                bool flag = Rhino.Geometry.Intersect.Intersection.LinePlane(pline_.SegmentAt(i), plane, out double t);

                if (flag && t > -0.999999999 && t < 1.0000001)
                    T.Add(i + t);
            }

            var TArray = T.ToArray();
            Array.Sort(TArray);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Split curve by parameters
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            c = pline_.ToNurbsCurve();
            Curve[] splitCurves;

            if (TArray.Length == 2)
            {
                splitCurves = new Curve[] { c.Trim(TArray[0], TArray[1]), c.Trim(TArray[1], TArray[0]) };
            }
            else
            {
                splitCurves = c.Split(TArray);
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Check if curve on which side
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            SortedDictionary<int, Polyline> splitPlines0 = new SortedDictionary<int, Polyline>();
            SortedDictionary<int, Polyline> splitPlines1 = new SortedDictionary<int, Polyline>();
            Plane planeOffset = new Plane(plane);
            planeOffset.Translate(plane.ZAxis * 0.01);
            int key = 0;

            foreach (Curve sp in splitCurves)
            {
                if (sp.TryGetPolyline(out Polyline cutPline))
                {
                    if (plane.IsAbovePlane(sp.PointAtNormalizedLength(0.5)) > -1)
                    { //if (cp0.DistanceToSquared(pt) < cp1.DistanceToSquared(pt)){ //Distance is positive or negative based on plane normal
                        splitPlines0.Add(key, cutPline);
                    }
                    else
                    {
                        splitPlines1.Add(key, cutPline);
                    }
                }

                key++;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Output
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            return new Tuple<SortedDictionary<int, Polyline>, SortedDictionary<int, Polyline>>(splitPlines0, splitPlines1);
        }

        public static Line ToLine(this Polyline pline)
        {
            return new Line(pline[0], pline[1]);
        }

        /// <summary>
        /// Scale and Shear Polyline by Vector
        /// </summary>
        /// <param name="pline - tile to transform"></param>
        /// <param name="scale - scale tile after stretching to maintain original thickness"></param>
        /// <param name="scaleX - Scale tile before shear"></param>
        /// <param name="scaleY - Scale tile before shear"></param>
        /// <param name="v - joint direction"></param>
        /// <returns></returns>
        public static Polyline ScaleShearByVector(this Polyline pline, Vector3d vX, bool scale = false, double scaleX = 1, double scaleY = 1, double scaleZ = 1, int translation = 0, double rotateDegrees = 0, bool split = false, Vector3d vY = default(Vector3d), Vector3d vZ = default(Vector3d))
        {
            Polyline p = pline.Duplicate();

            var xforms = new List<Transform>();

            //Rotate tile
            if (rotateDegrees != 0)
            {
                if (rotateDegrees > 0)
                {
                    var t0 = Rhino.Geometry.Transform.Rotation(Rhino.RhinoMath.ToRadians(rotateDegrees), Point3d.Origin);
                    p.Transform(t0);
                    xforms.Add(t0);
                }
                else
                {
                    var t0 = Rhino.Geometry.Transform.Rotation(Rhino.RhinoMath.ToRadians(rotateDegrees), Vector3d.XAxis, Point3d.Origin);
                    p.Transform(t0);
                    xforms.Add(t0);
                }
            }

            //Translate to change origin point
            var t1 = Rhino.Geometry.Transform.Translation(new Vector3d(0.5 * translation, 0, 0));
            xforms.Add(t1);
            p.Transform(t1);

            //Scale based on initial angle
            var t2 = Rhino.Geometry.Transform.Scale(Plane.WorldXY, scaleX, scaleY, scaleZ);
            xforms.Add(t2);
            p.Transform(t2);

            //Scale based on sin angle
            if (scale)
            {
                double angleScaleX = Math.Sin(Vector3d.VectorAngle(vX, Vector3d.YAxis));
                double angleScaleZ = Math.Sin(Vector3d.VectorAngle(vZ, Vector3d.YAxis));
                if (angleScaleX > 0.01)
                {
                    var t3 = Rhino.Geometry.Transform.Scale(Plane.WorldXY, 1.00, 1 / angleScaleX, 1);
                    p.Transform(t3);
                    //xforms.Add(t3);
                }
                if (angleScaleZ > 0.01)
                {
                    var t4 = Rhino.Geometry.Transform.Scale(Plane.WorldXY, 1.00, 1 / angleScaleZ, 1);
                    p.Transform(t4);
                    //xforms.Add(t3);
                }
            }

            Vector3d vX_ = new Vector3d(vX);
            Vector3d vY_ = new Vector3d(vY);
            Vector3d vZ_ = new Vector3d(vZ);
            foreach (var t in xforms)
            {
                vZ_.Transform(t);
            }
            //vZ_ = default(Vector3d);
            //Shear based on tan angle
            double angleShearX = (vX_ == Vector3d.Zero) ? 0 : Vector3d.VectorAngle(vX_, Vector3d.XAxis, Plane.WorldXY);
            double angleShearY = (vY_ == Vector3d.Zero) ? 0 : Vector3d.VectorAngle(vY_, Vector3d.YAxis, Plane.WorldXY);
            //double angleShearZ = (vZ == Vector3d.Zero) ? 0 : Vector3d.VectorAngle(vZ, Vector3d.XAxis, Plane.WorldXY);
            double angleShearZ = (vZ_ == Vector3d.Zero) ? 0 : Vector3d.VectorAngle(vZ_, Vector3d.ZAxis, Plane.WorldYZ);

            //Rhino.RhinoApp.WriteLine(angleShearZ.ToString());

            //(new Line(Point3d.Origin, vX * 200)).Bake();
            //(new Line(Point3d.Origin, vZ*200)).Bake();
            //double angleShearZ = 0;
            double xShear = Math.Tan(angleShearX) * -1;
            double yShear = Math.Tan(angleShearY) * -1;
            double zShear = Math.Tan(angleShearZ) * -1;
            //Rhino.RhinoApp.WriteLine(zShear.ToString());

            p.Transform(Rhino.Geometry.Transform.Shear(Plane.WorldXY, new Vector3d(1, xShear, 0), new Vector3d(yShear, 1, 0.0), new Vector3d(0, zShear * 1, 1)));

            //Split if needed
            if (split)
            {
                Tuple<SortedDictionary<int, Polyline>, SortedDictionary<int, Polyline>> splitPlines = SplitCurvesByPlane(p, Plane.WorldYZ);

                //Shear the polyline negatively
                foreach (var pl in splitPlines.Item1)
                {
                    pl.Value.Transform(Rhino.Geometry.Transform.Shear(Plane.WorldXY, new Vector3d(1, 2 * -xShear, 0), new Vector3d(2 * -yShear, 1, 0), new Vector3d(0, 0, 1)));
                    splitPlines.Item2.Add(pl.Key, pl.Value);
                }

                //Combine the polyline back
                Polyline joinedPline = new Polyline();
                foreach (var pl in splitPlines.Item2)
                    joinedPline.AddRange(pl.Value);
                joinedPline.CollapseShortSegments(0.01);

                return joinedPline;
            }

            return p;
        }

        public static List<List<Polyline>> FindPairsPlines(List<Polyline> crvs, bool pairing = true)
        {
            List<List<Polyline>> pairs = new List<List<Polyline>>();

            //if pairing is turned of the list is just split into pairs without any ordering
            if (!pairing)
            {
                for (int i = 0; i < crvs.Count; i += 2)
                    pairs.Add(new List<Polyline> { crvs[i], crvs[i + 1] });
                return pairs;
            }

            List<Polyline> ccrvs = new List<Polyline>();

            foreach (var crv in crvs)
            {
                if (crv != null)
                {
                    if (crv.IsValid)
                    {
                        if (crv.IsClosed)
                        {
                            ccrvs.Add(crv);
                        }
                    }
                }
            }

            List<Polyline> uppercrvs = new List<Polyline>();
            List<Polyline> lowercrvs = new List<Polyline>();

            if (ccrvs != null)                                                  // now sort into 2 lists of upper and lower crvs
            {
                foreach (var crv in ccrvs)
                {
                    Point3d testpt = crv[0];
                    if (testpt.Z >= -0.1 && testpt.Z <= 0.1)
                    {
                        lowercrvs.Add(crv);
                    }
                    else
                    {
                        uppercrvs.Add(crv);
                    }
                }

                foreach (var lcrv in lowercrvs)
                {
                    Point3d pt0 = lcrv[0];
                    List<double> ldst = new List<double>();

                    foreach (var ucrv in uppercrvs)
                    {
                        Point3d pt1 = ucrv[0];
                        double dst = pt1.DistanceTo(pt0);
                        ldst.Add(dst);
                    }

                    double var1 = 1000;
                    int var2 = 0;
                    for (int i = 0; i != ldst.Count; i++)
                    {
                        if (ldst[i] > 0 && ldst[i] < var1)
                        {
                            var1 = ldst[i];
                            var2 = i;
                        }
                    }

                    List<Polyline> pair = new List<Polyline>();
                    pair.Add(lcrv);
                    pair.Add(uppercrvs[var2]);
                    pairs.Add(pair);
                }
            }

            return pairs;
        }

        public static List<List<Curve>> FindPairs(List<Curve> crvs, bool pairing = true)
        {
            List<List<Curve>> pairs = new List<List<Curve>>();

            //if pairing is turned of the list is just split into pairs without any ordering
            if (!pairing)
            {
                for (int i = 0; i < crvs.Count; i += 2)
                    pairs.Add(new List<Curve> { crvs[i], crvs[i + 1] });
                return pairs;
            }

            List<Curve> ccrvs = new List<Curve>();

            foreach (Curve crv in crvs)
            {
                if (crv != null)
                {
                    if (crv.IsValid)
                    {
                        if (crv.IsClosed)
                        {
                            ccrvs.Add(crv);
                        }
                    }
                }
            }

            List<Curve> uppercrvs = new List<Curve>();
            List<Curve> lowercrvs = new List<Curve>();

            if (ccrvs != null)                                                  // now sort into 2 lists of upper and lower crvs
            {
                foreach (Curve crv in ccrvs)
                {
                    Point3d testpt = crv.PointAtStart;
                    if (testpt.Z >= -0.1 && testpt.Z <= 0.1)
                    {
                        lowercrvs.Add(crv);
                    }
                    else
                    {
                        uppercrvs.Add(crv);
                    }
                }

                foreach (Curve lcrv in lowercrvs)
                {
                    Point3d pt0 = lcrv.PointAtStart;
                    List<double> ldst = new List<double>();

                    foreach (Curve ucrv in uppercrvs)
                    {
                        Point3d pt1 = ucrv.PointAtStart;
                        double dst = pt1.DistanceTo(pt0);
                        ldst.Add(dst);
                    }

                    double var1 = 1000;
                    int var2 = 0;
                    for (int i = 0; i != ldst.Count; i++)
                    {
                        if (ldst[i] > 0 && ldst[i] < var1)
                        {
                            var1 = ldst[i];
                            var2 = i;
                        }
                    }

                    List<Curve> pair = new List<Curve>();
                    pair.Add(lcrv);
                    pair.Add(uppercrvs[var2]);
                    pairs.Add(pair);
                }
            }

            return pairs;
        }

        public static Polyline ProjectToPlaneByVector(this Polyline polyline, Plane plane, Line dir)
        {
            Polyline projectedPolyline = new Polyline();
            foreach (var p in polyline)
            {
                Line ray = new Line(p, p + dir.Direction);
                projectedPolyline.Add(PlaneUtil.LinePlane(ray, plane));
            }
            return projectedPolyline;
        }

        public static Polyline ToPolyline(this Line l)
        {
            return new Polyline(new Point3d[] { l.From, l.To });
        }

        public static List<Polyline> Duplicate(this List<Polyline> p)
        {
            List<Polyline> polylines = new List<Polyline>();
            foreach (Polyline p_ in p)
                polylines.Add(new Polyline(p_));
            return polylines;
        }

        /// <summary>
        /// Takes 4 point rectangle 1st - 3rd or 2nd - 4th lines and creates zigzag
        ///returns good result only when rectangle edges are equal length
        /// </summary>
        /// <param name="rectangle"></param>
        /// <param name="flip"></param>
        /// <param name="dist"></param>
        /// <returns></returns>
        public static Polyline ZigZag(this Polyline rectangle, bool flip, double dist, int divisions = -1)
        {
            Line l0 = (flip) ? new Line(rectangle[0], rectangle[1]) : new Line(rectangle[1], rectangle[2]);
            Line l1 = (flip) ? new Line(rectangle[3], rectangle[2]) : new Line(rectangle[0], rectangle[3]);

            int n = divisions == -1 ? (int)Math.Ceiling(l0.Length / dist) : divisions;

            n += (n % 2);

            //rectangle.Bake();

            double dist_ = l0.Length / n;

            Polyline zigzag = new Polyline();
            Vector3d v0 = l0.Direction.UnitVector() * dist_;

            for (int i = 0; i < n; i++)
            {
                if (i % 2 == 0)
                {
                    zigzag.Add(l0.From + (v0 * i));
                    zigzag.Add(l1.From + (v0 * (i)));
                }
                else
                {
                    zigzag.Add(l1.From + (v0 * (i)));
                    zigzag.Add(l0.From + (v0 * (i)));
                }//if
            }//for

            //Finish the zigzag end
            if (n % 2 == 0)
            {
                zigzag.Add(l0.To);
                zigzag.Add(l1.To);
            }
            else
            {
                zigzag.Add(l1.To);
                zigzag.Add(l0.To);
            }//if

            zigzag[0] = (zigzag[0] + zigzag[1]) * 0.5;
            zigzag[zigzag.Count - 1] = (zigzag[zigzag.Count - 1] + zigzag[zigzag.Count - 2]) * 0.5;

            return zigzag;
        }

        public static Polyline Flip(this Polyline polyline)
        {
            Polyline p = polyline.Duplicate();
            p.Reverse();
            return p;
        }

        public static void Orient(this Polyline polyline, Plane source, Plane target)
        {
            Transform transform = Rhino.Geometry.Transform.plane_to_plane(source, target);
            polyline.Transform(transform);
        }

        public static List<Line> Transform(this List<Line> lines, Transform t)
        {
            List<Line> linesTransform = new List<Line>();

            for (int i = 0; i < lines.Count; i++)
            {
                Line l = lines[i];
                l.Transform(t);
                linesTransform.Add(l);
            }

            return linesTransform;
        }

        public static Line[] Transform(this Line[] lines, Transform t)
        {
            Line[] linesTransform = new Line[lines.Length];

            for (int i = 0; i < lines.Length; i++)
            {
                Line l = lines[i];
                l.Transform(t);
                linesTransform[i] = l;
            }

            return linesTransform;
        }

        public static List<Line[]> Transform(this List<Line[]> lines, Transform t)
        {
            List<Line[]> linesTransformed = new List<Line[]>(lines.Count);

            foreach (Line[] ln in lines)
            {
                linesTransformed.Add(ln.Transform(t));
            }
            return linesTransformed;
        }

        public static void Transform(this IEnumerable<Polyline> polylines, Transform t)
        {
            foreach (Polyline p in polylines)
                p.Transform(t);
        }

        public static Polyline[] XForm(this IEnumerable<Polyline> polylines, Transform t)
        {
            Polyline[] pp = new Polyline[polylines.Count()];

            int count = 0;
            foreach (Polyline p in polylines)
            {
                Polyline pline = new Polyline(p);
                pline.Transform(t);
                pp[count++] = pline;
            }
            return pp;
        }

        public static Polyline Translate(this Polyline p, Vector3d v)
        {
            Polyline polyline = new Polyline(p);
            polyline.Transform(Rhino.Geometry.Transform.Translation(v));
            return polyline;
        }

        public static List<Polyline> Polygons(this IEnumerable<Circle> curves, int n, double rotation = Math.PI * 0.25, bool sqrt = true)
        {
            List<Polyline> polygons = new List<Polyline>();
            foreach (var c in curves)
                polygons.Add(Polygon(n, c, rotation, sqrt));
            return polygons;
        }

        public static List<Polyline> Polygons(this IEnumerable<Curve> curves, int n, double rotation = Math.PI * 0.25, bool sqrt = true)
        {
            List<Polyline> polygons = new List<Polyline>();
            foreach (var c in curves)
                polygons.Add(Polygon(n, c, rotation, sqrt));
            return polygons;
        }

        public static Polyline Polygon(int n, Circle circle, double rotation = Math.PI * 0.25, bool sqrt = true)
        {
            return Polygon(n, circle.Radius, circle.Plane, rotation, sqrt);
        }

        public static Polyline Polygon(int n, Curve c, double rotation = Math.PI * 0.25, bool sqrt = true)
        {
            if (c.TryGetCircle(out Circle circle))
            {
                return Polygon(n, circle.Radius, circle.Plane, rotation, sqrt);
            }

            return new Polyline();
        }

        public static Polyline Polygon(int n, double radius, Plane plane, double rotation = Math.PI * 0.25, bool sqrt = true)
        {
            Polyline polyline = new Polyline();
            double sector = Math.PI * 2 / n;
            double r = sqrt ? 1 / Math.Sqrt(2) * radius : radius;

            for (int i = 0; i < n; i++)
            {
                Point3d p = new Point3d(Math.Sin((sector * i) + rotation) * r, Math.Cos((sector * i) + rotation) * r, 0);
                polyline.Add(p);
            }

            polyline.Add(polyline[0]);

            polyline.Transform(Rhino.Geometry.Transform.plane_to_plane(Plane.WorldXY, plane));

            return polyline;
        }

        public static Point3d CenterPoint(Polyline polyline)
        {
            int Count = polyline.Count;

            if (Count == 0) { return Point3d.Unset; }

            if (Count == 1) { return polyline[0]; }

            Point3d center = Point3d.Origin;

            double weight = 0.0;

            int stop = (Count - 1);
            if (polyline[0].DistanceToSquared(polyline[polyline.Count - 1]) > 0.001)
            {
                //Rhino.RhinoApp.WriteLine(polyline[0].DistanceToSquared(polyline[polyline.Length - 1]).ToString());
                stop++;
            }
            for (int i = 0; i < stop; i++)
            {
                Point3d A = polyline[i];

                Point3d B = polyline[(i + 1) % Count];

                double d = A.DistanceTo(B);

                center += d * 0.5 * (A + B);

                weight += d;
            }

            center /= weight;

            return center;
        }

        public static Polyline[] IntersectTwoPlates(Polyline plateA0, Polyline plateA1, Polyline plateB0, Polyline plateB1)
        {
            double extend = 0;

            var pA0B0 = PolylinePlane(plateA0, plateB0);
            var pA0B1 = PolylinePlane(plateA0, plateB1);
            var pA1B0 = PolylinePlane(plateA1, plateB0);
            var pA1B1 = PolylinePlane(plateA1, plateB1);

            if (pA0B0[0] != Point3d.Unset && pA0B0[1] != Point3d.Unset && pA0B1[0] != Point3d.Unset && pA0B1[1] != Point3d.Unset &&
                pA1B0[0] != Point3d.Unset && pA1B0[1] != Point3d.Unset && pA1B1[0] != Point3d.Unset && pA1B1[1] != Point3d.Unset
                )
            {
                Point3d pA0B0Mid = PointUtil.AveragePoint(pA0B0);
                Point3d pA0B1Mid = PointUtil.AveragePoint(pA0B1);
                Point3d pA1B0Mid = PointUtil.AveragePoint(pA1B0);
                Point3d pA1B1Mid = PointUtil.AveragePoint(pA1B1);

                Vector3d v0 = (pA0B0[0] - pA0B0[1]).UnitVector() * extend;
                Vector3d v1 = (pA0B1[0] - pA0B1[1]).UnitVector() * extend;
                Vector3d v2 = (pA1B0[0] - pA1B0[1]).UnitVector() * extend;
                Vector3d v3 = (pA1B1[0] - pA1B1[1]).UnitVector() * extend;

                Polyline u0 = new Polyline(new Point3d[] { pA0B0[0], pA0B0Mid - v0, pA0B1Mid - v1, pA0B1[0] });
                Polyline u1 = new Polyline(new Point3d[] { pA1B0[0], pA1B0Mid - v2, pA1B1Mid - v3, pA1B1[0] });

                Polyline u2 = new Polyline(new Point3d[] { pA0B0[1], pA0B0Mid + v0, pA1B0Mid + v2, pA1B0[1] });
                Polyline u3 = new Polyline(new Point3d[] { pA0B1[1], pA0B1Mid + v1, pA1B1Mid + v3, pA1B1[1] });

                return new Polyline[] { u0, u1, u2, u3 };
            }

            return new Polyline[0];
        }

        public static Point3d[] PolylinePlane(Polyline p0, Plane pl0)
        {
            List<Point3d> pts = new List<Point3d>();
            foreach (Line line in p0.GetSegments())
            {
                double t;
                if (Intersection.LinePlane(line, pl0, out t))
                {
                    if (t > 1 || t < 0) continue;
                    Point3d pTemp = line.PointAt(t);
                    pts.Add(pTemp);

                    if (pts.Count == 2)
                        break;
                }
            }
            //Rhino.RhinoApp.WriteLine(pts.Count.ToString());
            if (pts.Count == 2)
                return new Point3d[] { pts[0], pts[1] };
            else
                return new Point3d[] { };
        }

        public static Line PlinePlane(Polyline p0, Plane pl0)
        {
            List<Point3d> pts = new List<Point3d>();
            foreach (Line line in p0.GetSegments())
            {
                double t;
                if (Intersection.LinePlane(line, pl0, out t))
                {
                    if (t > 1 || t < 0) continue;
                    Point3d pTemp = line.PointAt(t);
                    pts.Add(pTemp);

                    if (pts.Count == 2)
                        break;
                }
            }
            //Rhino.RhinoApp.WriteLine(pts.Count.ToString());
            //if (pts.Count == 2)
            //    pts[0].Bake();
            if (pts.Count == 2)
                return new Line(pts[0], pts[1]);
            else
                return Line.Unset;
        }

        public static Line LineLineCP(Line l0, Line l1)
        {
            double[] t = new double[4];
            t[0] = 0;
            t[1] = 1;
            t[2] = l0.ClosestParameter(l1.From);
            t[3] = l0.ClosestParameter(l1.To);
            Array.Sort(t);

            Line l = new Line(l0.PointAt(t[1]), l0.PointAt(t[2]));

            return l;
        }

        public static Line LineLineCPAverage(Line l0, Line l1)
        {
            Line lA = LineLineCP(l0, l1);
            Line lB = LineLineCP(l1, l0);

            double d0_lAFrom_lBFrom = lA.From.DistanceToSquared(lB.From);
            double d0_lAFrom_lBTo = lA.From.DistanceToSquared(lB.To);
            double d0_lATo_lBFrom = lA.To.DistanceToSquared(lB.From);
            double d0_lATo_lBTo = lA.To.DistanceToSquared(lB.To);

            double[] d = new double[] { d0_lAFrom_lBFrom, d0_lAFrom_lBTo, d0_lATo_lBFrom, d0_lATo_lBTo };
            int[] id = new int[] { 0, 1, 2, 3 };
            Line[] lines = new Line[] {
      new Line((lA.From + lB.From) * 0.5, (lA.To + lB.To) * 0.5),
      new Line((lA.From + lB.To) * 0.5, (lA.To + lB.From) * 0.5),
      new Line((lA.To + lB.From) * 0.5, (lA.From + lB.To) * 0.5),
      new Line((lA.To + lB.To) * 0.5, (lA.From + lB.From) * 0.5),
      };

            Array.Sort(d, lines);

            return lines[0];
        }

        public static Point3d[] PolylinePlane(Polyline p0, Polyline p1)
        {
            Line[] segmentsP0 = p0.GetSegments();
            Line[] segmentsP1 = p1.GetSegments();

            Plane pl0 = p0.plane();
            Plane pl1 = p1.plane();

            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p0);
            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1);

            Point3d ptP0 = Point3d.Unset;
            Point3d ptP1 = Point3d.Unset;

            foreach (Line line in segmentsP0)
            {
                double t;
                if (Intersection.LinePlane(line, pl1, out t))
                {
                    if (t > 1 || t < 0) continue;
                    Point3d pTemp = line.PointAt(t);

                    if (p1.ToNurbsCurve().Contains(pTemp, pl1, 0.01) == PointContainment.Inside)
                    {
                        ptP0 = pTemp;
                        break;
                    }
                }
            }

            foreach (Line lineP1 in segmentsP1)
            {
                double t;
                if (Intersection.LinePlane(lineP1, pl0, out t))
                {
                    if (t > 1 || t < 0) continue;
                    Point3d pTemp = lineP1.PointAt(t);
                    if (p0.ToNurbsCurve().Contains(pTemp, pl0, 0.01) == PointContainment.Inside)
                    {
                        ptP1 = pTemp;
                        break;
                    }
                }
            }

            return new Point3d[] { ptP0, ptP1 };
        }

        public static Line[] LoftLine(Polyline p0, Polyline p1)
        {
            Line[] line = new Line[p0.Count - 1];

            for (int i = 0; i < p0.Count - 1; i++)
            {
                line[i] = new Line(p0[i], p1[i]);
            }
            return line;
        }

        public static void InsertPoint(this Polyline polyline, Point3d p)
        {
            polyline.Insert((int)Math.Ceiling(polyline.ClosestParameter(p)), p);
        }

        public static List<Polyline> MappedFromSurfaceToSurface(this List<Polyline> polylines, Surface s, Surface t)
        {
            s.SetDomain(0, new Interval(0, 1));
            s.SetDomain(1, new Interval(0, 1));
            t.SetDomain(0, new Interval(0, 1));
            t.SetDomain(1, new Interval(0, 1));
            //  Rhino.RhinoDoc.ActiveDoc.Objects.AddSurface(s);

            List<Polyline> mapped = new List<Polyline>();

            for (int i = 0; i < polylines.Count; i++)
            {
                Polyline pol = new Polyline(polylines[i]);

                bool flag = true;

                for (int j = 0; j < pol.Count; j++)
                {
                    double u, v;
                    Point3d pTemp = new Point3d(pol[j]);
                    s.ClosestPoint(pol[j], out u, out v);

                    pol[j] = t.PointAt(u, v);

                    if (s.PointAt(u, v).DistanceTo(pTemp) > 0.01)
                    {
                        flag = false;
                        break;
                    }
                }//for j
                if (flag)
                {
                    mapped.Add(pol);
                }
            }//for i
            return mapped;
        }

        public static List<Polyline> MappedFromMeshToMesh(this List<Polyline> polylines, Mesh s, Mesh t)
        {
            //    s.SetDomain(0, new Interval(0, 1));
            //    s.SetDomain(1, new Interval(0, 1));
            //    t.SetDomain(0, new Interval(0, 1));
            //    t.SetDomain(1, new Interval(0, 1));
            //  Rhino.RhinoDoc.ActiveDoc.Objects.AddSurface(s);

            List<Polyline> mapped = new List<Polyline>();

            for (int i = 0; i < polylines.Count; i++)
            {
                Polyline pol = new Polyline(polylines[i]);

                bool flag = true;

                for (int j = 0; j < pol.Count; j++)
                {
                    //point3d = mesh.PointAt(index, t, num, t1, num1);
                    //vector3d = mesh.NormalAt(index, t, num, t1, num1);

                    Point3d pTemp = new Point3d(pol[j]);

                    MeshPoint mp = s.ClosestMeshPoint(pol[j], 10.01);
                    if (mp == null)
                    {
                        flag = false;
                        break;
                    }

                    //   Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(mp.Point);

                    pol[j] = t.PointAt(mp);

                    if (s.PointAt(mp).DistanceTo(pTemp) > 0.01)
                    {
                        flag = false;
                        break;
                    }
                }//for j

                //try to trim curve
                if (!flag)
                {
                    //How to check if curve is on the first?
                    MeshPoint mp0 = s.ClosestMeshPoint(pol[0], 10);
                    MeshPoint mp1 = s.ClosestMeshPoint(pol[pol.Count - 1], 10);
                }

                if (flag)
                {
                    mapped.Add(pol);
                    //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(pol);
                }
            }//for i
            return mapped;
        }

        public static Polyline OutlineFromFaceEdgeCorner(Plane facePlane, Plane[] edgePlanes, Plane[] bisePlanes, int T = 1, double tolerance = 0.1)
        {
            Polyline polyline = new Polyline();

            switch (T)
            {
                case (2):

                    for (int j = 0; j < edgePlanes.Length; j++)
                    {
                        Plane currPlane = edgePlanes[j];
                        Plane nextPlane = edgePlanes[MathUtil.Wrap(j + 1, edgePlanes.Length)];

                        if (Vector3d.VectorAngle(currPlane.XAxis, nextPlane.XAxis) < tolerance)
                        {
                            Vector3d vv = new Vector3d(currPlane.XAxis);
                            vv.Rotate(Math.PI * 0.5, currPlane.YAxis);
                            nextPlane = new Plane(bisePlanes[j].Origin, vv, currPlane.YAxis);
                        }

                        Line line = PlaneUtil.PlanePlane(currPlane, nextPlane);
                        polyline.Add(PlaneUtil.LinePlane(line, facePlane));
                    }
                    polyline.Close();
                    break;

                default:
                    for (int j = 0; j < bisePlanes.Length; j++)
                    {
                        Point3d pt;
                        Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(facePlane, bisePlanes[j], edgePlanes[j], out pt);
                        polyline.Add(pt);
                    }
                    polyline.Close();
                    break;
            }

            return polyline;
        }

        public static Polyline ProjectPolyline(this Polyline p, bool averagePlane = true)
        {
            Polyline p_ = new Polyline(p.Count);

            Plane plane = p.plane();
            if (!averagePlane)
                Plane.FitPlaneToPoints(p, out plane);

            int closed = (p[0].DistanceToSquared(p[p.Count - 1]) < 0.001) ? 1 : 0;

            for (int i = 0; i < p.Count - closed; i++)
            {
                p_.Add(plane.ClosestPoint(p[i]));
            }

            if (closed == 1)
                p_.Close();

            return p_;
        }

        public static Polyline ProjectPolyline(this Polyline pline, Plane plane)
        {
            Polyline p_ = new Polyline(pline.Count);

            foreach (Point3d p in pline)
                p_.Add(plane.ClosestPoint(p));

            return p_;
        }

        public static List<Curve> ShatterCurve(Curve C, List<Point3d> P, double D = 0.01)
        {
            List<Curve> curves = new List<Curve>();
            try
            {
                List<double> shatterT = new List<double>();

                Curve c = C.DuplicateCurve();
                //c.Domain = new Interval(0, 1);
                double a = c.Domain.T0;
                double b = c.Domain.T1;

                for (int i = 0; i < P.Count; i++)
                {
                    double t;
                    bool flag = c.ClosestPoint(P[i], out t, D);

                    if (flag)
                    {
                        shatterT.Add((double)Math.Round(t, 5));
                        // shatterT.Add(t);
                    }
                }

                if (shatterT.Count > 0)
                {
                    shatterT.Sort();
                    shatterT = shatterT.Distinct().ToList();

                    if (shatterT[0] == a && shatterT[shatterT.Count - 1] == b)
                        shatterT.RemoveAt(0);

                    if (c.PointAtStart.DistanceToSquared(c.PointAtEnd) < 0.001)
                    {
                        for (int i = 0; i < shatterT.Count; i++)
                        {
                            //Rhino.RhinoApp.WriteLine(shatterT[i].ToString() +  " " + shatterT[(i + 1) % shatterT.Count].ToString()) ;

                            if (shatterT[shatterT.Count - 1] == b)
                            {
                                Curve curve = c.Trim(shatterT[MathUtil.Wrap(i - 1, shatterT.Count)], shatterT[i]);
                                curves.Add(curve);
                            }
                            else
                            {
                                Curve curve = c.Trim(shatterT[i], shatterT[(i + 1) % shatterT.Count]);
                                curves.Add(curve);
                            }
                        }
                    }
                    else
                    {
                        if (shatterT[0] != a)
                            shatterT.Insert(0, a);

                        if (shatterT[shatterT.Count - 1] != b)
                            shatterT.Insert(shatterT.Count, b);

                        for (int i = 0; i < shatterT.Count - 1; i++)
                        {
                            Curve curve = c.Trim(shatterT[i], shatterT[(i + 1)]);
                            curves.Add(curve);
                        }
                    }
                }
                else
                {
                    curves.Add(c);
                }
            }
            catch (Exception e)
            {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return curves;
        }

        public static Polyline IntersectPlanarLines(Line[] lines, bool close = true)
        {
            Polyline polyline = new Polyline();

            for (int i = 0; i < lines.Length; i++)
            {
                double a, b;
                Rhino.Geometry.Intersect.Intersection.LineLine(lines[i], lines[(i + 1) % lines.Length], out a, out b, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance, false);
                Point3d p = lines[i].PointAt(a);
                polyline.Add(p);
            }

            if (close)
                polyline.Add(polyline[0]);

            return polyline;
        }

        public static Polyline IntersectPlanarLines(Line[] lines, Plane projectToPlane, bool close = true)
        {
            for (int i = 0; i < lines.Length; i++)
            {
                lines[i].From = projectToPlane.ClosestPoint(lines[i].From);
                lines[i].To = projectToPlane.ClosestPoint(lines[i].To);
            }

            return IntersectPlanarLines(lines, close);
        }

        //public static List<Polyline> JoinPolylines(IEnumerable<Polyline> Polylines, ref List<List<int>> Groups) {
        //    List<Point3d> point3ds = new List<Point3d>();
        //    SortedList<TopologyEdge, Polyline> topologyEdges = new SortedList<TopologyEdge, Polyline>();
        //    SortedList<TopologyEdge, int> topologyEdges1 = new SortedList<TopologyEdge, int>();
        //    SortedList<TopologyEdge, int> item = new SortedList<TopologyEdge, int>();
        //    int num = checked(Polylines.Count<Polyline>() - 1);
        //    for (int i = 0; i <= num; i = checked(i + 1)) {
        //        point3ds.Add(Polylines.ElementAtOrDefault<Polyline>(i)[0]);
        //        point3ds.Add(Polylines.ElementAtOrDefault<Polyline>(i)[checked(Polylines.ElementAtOrDefault<Polyline>(i).Count - 1)]);
        //        TopologyEdge topologyEdge = new TopologyEdge(checked(point3ds.Count - 2), checked(point3ds.Count - 1), -1, -1);
        //        topologyEdges1[topologyEdge] = i;
        //        topologyEdges.Add(topologyEdge, Polylines.ElementAtOrDefault<Polyline>(i));
        //    }
        //    Graphs.DuplicateRemover duplicateRemover = new Graphs.DuplicateRemover(point3ds, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 2, 123);
        //    duplicateRemover.Solve();
        //    SortedList<TopologyEdge, Polyline> topologyEdges2 = new SortedList<TopologyEdge, Polyline>();
        //    List<int> nums = new List<int>();
        //    int num1 = 0;
        //    int count = checked(topologyEdges.Keys.Count - 1);
        //    for (int j = 0; j <= count; j = checked(j + 1)) {
        //        TopologyEdge item1 = topologyEdges.Keys[j];
        //        TopologyEdge topologyEdge1 = new TopologyEdge(duplicateRemover.Map[item1.From], duplicateRemover.Map[item1.To], -1, -1);
        //        if (!topologyEdges2.ContainsKey(topologyEdge1)) {
        //            item[topologyEdge1] = topologyEdges1[item1];
        //            topologyEdges2.Add(topologyEdge1, topologyEdges[item1]);
        //        } else {
        //            topologyEdges2.Remove(topologyEdge1);
        //            num1 = checked(num1 + 2);
        //            nums.Add(topologyEdge1.From);
        //            nums.Add(topologyEdge1.To);
        //        }
        //    }
        //    List<int>[] listArrays = new List<int>[checked(checked((int)duplicateRemover.UniquePoints.Length - 1) + 1)];
        //    int length = checked((int)listArrays.Length - 1);
        //    for (int k = 0; k <= length; k = checked(k + 1)) {
        //        listArrays[k] = new List<int>();
        //    }
        //    int count1 = checked(point3ds.Count - 1);
        //    for (int l = 0; l <= count1; l = checked(l + 2)) {
        //        listArrays[duplicateRemover.Map[l]].Add(duplicateRemover.Map[checked(l + 1)]);
        //        listArrays[duplicateRemover.Map[checked(l + 1)]].Add(duplicateRemover.Map[l]);
        //    }
        //    bool[] flagArray = new bool[checked(checked((int)duplicateRemover.UniquePoints.Length - 1) + 1)];
        //    int count2 = num1;
        //    int num2 = checked(nums.Count - 1);
        //    for (int m = 0; m <= num2; m = checked(m + 1)) {
        //        flagArray[nums[m]] = true;
        //    }
        //    SortedList<int, List<int>> nums1 = new SortedList<int, List<int>>()
        //    {
        //        { 0, new List<int>() },
        //        { 1, new List<int>() },
        //        { 2, new List<int>() }
        //    };
        //    int length1 = checked((int)listArrays.Length - 1);
        //    for (int n = 0; n <= length1; n = checked(n + 1)) {
        //        if (!flagArray[n]) {
        //            nums1[listArrays[n].Count].Add(n);
        //        }
        //    }
        //    List<int> nums2 = new List<int>();
        //    nums2.AddRange(nums1[1]);
        //    nums2.AddRange(nums1[2]);
        //    List<Polyline> polylines = new List<Polyline>();
        //    while (count2 < (int)flagArray.Length) {
        //        int count3 = checked(nums2.Count - 1);
        //        for (int o = 0; o <= count3; o = checked(o + 1)) {
        //            if (!flagArray[nums2[o]]) {
        //                List<int> nums3 = new List<int>();
        //                nums3.Clear();
        //                Polyline polyline = new Polyline();
        //                List<int> nums4 = (List<int>)PointUtil.Walk(nums2[o], listArrays);
        //                int num3 = checked(nums4.Count - 2);
        //                for (int p = 0; p <= num3; p = checked(p + 1)) {
        //                    TopologyEdge topologyEdge2 = new TopologyEdge(nums4[p], nums4[checked(p + 1)], -1, -1);
        //                    Polyline polyline1 = topologyEdges2[topologyEdge2];
        //                    int item2 = item[topologyEdge2];
        //                    int num4 = topologyEdges2.IndexOfKey(topologyEdge2);
        //                    if (topologyEdges2.Keys[num4].From != topologyEdge2.From) {
        //                        polyline1.Reverse();
        //                    }
        //                    if (polyline.Count <= 0) {
        //                        polyline.AddRange(polyline1);
        //                    } else if (polyline[checked(polyline.Count - 1)] != polyline1[0]) {
        //                        polyline.AddRange(polyline1);
        //                    } else {
        //                        int count4 = checked(polyline1.Count - 1);
        //                        for (int q = 1; q <= count4; q = checked(q + 1)) {
        //                            polyline.Add(polyline1[q]);
        //                        }
        //                    }
        //                    nums3.Add(item2);
        //                }
        //                Groups.Add(nums3);
        //                polylines.Add(polyline);
        //                count2 = checked(count2 + nums4.Count);
        //                int count5 = checked(nums4.Count - 1);
        //                for (int r = 0; r <= count5; r = checked(r + 1)) {
        //                    flagArray[nums4[r]] = true;
        //                }
        //            }
        //        }
        //    }
        //    return polylines;
        //}

        private static Polyline OrderPolyline(this Polyline polyline)
        {
            if (polyline != null)
            {
                if (polyline.IsValid)
                {
                    bool flag = IsClockwiseClosedPolylineOnXYPlane(polyline);
                    //isClockWise = flag;
                    if (flag)
                    {
                        Polyline polyline0 = new Polyline(polyline);
                        polyline0.Reverse();

                        return polyline0;
                    }
                    else
                    {
                        return polyline;
                    }
                }
            }
            return polyline;
        }

        public static bool IsClockwiseClosedPolyline(this Polyline p, Plane plane)
        {
            Polyline polygon = new Polyline(p);
            polygon.Transform(Rhino.Geometry.Transform.plane_to_plane(plane, Plane.WorldXY));

            double sum = 0;

            for (int i = 0; i < polygon.Count - 1; i++)
                sum += (polygon[i + 1].X - polygon[i].X) * (polygon[i + 1].Y + polygon[i].Y);

            return sum > 0;
        }

        public static bool IsClockwiseClosedPolylineOnXYPlane(this Polyline polygon)
        {
            double sum = 0;

            for (int i = 0; i < polygon.Count - 1; i++)
                sum += (polygon[i + 1].X - polygon[i].X) * (polygon[i + 1].Y + polygon[i].Y);

            return sum > 0;
        }

        public static Polyline Chamfer(this Polyline polyline, double value = 0.001)
        {
            Line[] lines = polyline.GetSegments();

            if (value == 0)
                return polyline;

            Polyline p = new Polyline();

            if (value < 0)
            {
                foreach (Line l in lines)
                {
                    Line lShorter = LineUtil.ExtendLine(l, -Math.Abs(value));
                    p.Add(lShorter.From);
                    p.Add(lShorter.To);
                }
            }
            else
            {
                foreach (Line l in lines)
                {
                    p.Add(l.PointAt(value));
                    p.Add(l.PointAt(1 - value));
                }
            }

            p.Add(p[0]);

            List<Point3d> points = new List<Point3d>();

            for (int i = 1; i < p.Count - 1; i += 2)
            {
                points.Add(new Point3d(
                  (p[i + 1].X + p[i].X) * 0.5,
                  (p[i + 1].Y + p[i].Y) * 0.5,
                  (p[i + 1].Z + p[i].Z) * 0.5
                  )
                  );
            }

            return p;
        }

        public static Polyline Chamfer(this Polyline polyline, bool[] flags, double value = 0.001)
        {
      
            //RhinoApp.WriteLine(" ");
            Line[] lines = polyline.GetSegments();

            if (value <= 0)
                return polyline;

            Polyline p = new Polyline();
            Polyline points = new Polyline(polyline.GetRange(0, polyline.Count-1).ToArray());
       
       
                    for (int i = 0; i < points.Count; i++)
                    {
                        int curr = i;
                        int next = rhino_util.MathUtil.Wrap((i + 1), points.Count);
                        int prev = rhino_util.MathUtil.Wrap((i - 1), points.Count);
                //RhinoApp.WriteLine(next.ToString() +" " + prev.ToString());

                Vector3d v0 = points[prev] - points[curr];
                Vector3d v1 = points[next] - points[curr];
                double angle = Vector3d.VectorAngle(v0, v1, Vector3d.CrossProduct(v0, v1));

                if (flags[i] && angle<Math.PI*0.5)
                        {

                    double value_ = MathUtil.RemapNumbers(angle,0,Math.PI*0.5, value, value * 0.1);
                    //RhinoApp.WriteLine(angle.ToString());
                        v0.Unitize();
                        v1.Unitize();
                        v0 *= value_;
                        v1 *= value_;
                        p.Add(points[curr] + v0);
                        p.Add(points[curr] + v1);
                    }
                        else
                        {
                           p.Add(points[curr]);
                            p.Add(points[curr]);

                        }
                    }


             
            p.Add(p[0]);
            
            return p;
        }

        /// <summary>
        /// Boolean union of joint outline and rectangle
        /// jointPolyline - Rectangle outline with joints
        /// platePolyline - Outline to insert joints
        /// </summary>
        /// <param name="jointBBox - Rectangle outline without joints"></param>
        /// <param name="jointPolyline - Rectangle outline with joints"></param>
        /// <param name="platePolyline - Outline to insert joints"></param>
        /// <returns></returns>
        /// <summary>
        /// Boolean union of joint outline and rectangle
        /// jointPolyline - Rectangle outline with joints
        /// platePolyline - Outline to insert joints
        /// </summary>
        /// <param name="jointBBox - Rectangle outline without joints"></param>
        /// <param name="jointPolyline - Rectangle outline with joints"></param>
        /// <param name="platePolyline - Outline to insert joints"></param>
        /// <returns></returns>
        /// <summary>
        ///
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        //public static Polyline InsertClosedPlineToClosedPline(Polyline jointBBox, Polyline jointPolyline, Polyline platePolyline, Polyline platePolylineBackUp, bool takeOutside = false) {
        //    Polyline p1Larger = new Polyline();
        //    try {
        //        //jointBBox.Bake();
        //        //jointPolyline.Bake();
        //        //platePolyline.Bake();
        //        //platePolylineBackUp.Bake();

        //        double tol = 0.1;

        //        //X is a Joint
        //        //Y is a Plate
        //        //Joints cannot be simplified upon insertion
        //        Polyline x = jointPolyline;
        //        Polyline y = platePolyline;

        //        //Take two lines of a joint that must intersect
        //        //List < Line > lines = new List<Line>{x.SegmentAt(0),x.SegmentAt(x.SegmentCount - 4)};
        //        List<Line> lines = jointBBox.GetSegments().ToList();

        //        //Find 2 intersection points
        //        var pts = new List<Point3d>(2);
        //        var xt0 = new List<double>(2);
        //        var xt1 = new List<double>(2);

        //        for (int i = 0; i < lines.Count; i++) {
        //            Line l0 = lines[i];
        //            for (int j = 0; j < y.SegmentCount; j++) {
        //                Line l1 = y.SegmentAt(j);
        //                if (l0.Direction.IsParallelTo(l1.Direction, 0.01) != 0) continue;
        //                double t0, t1;
        //                bool result = Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out t0, out t1, 0.1, true);

        //                if (!result) continue;
        //                if (pts.Count == 2) break;
        //                Point3d pi0 = l0.PointAt(t0);
        //                Point3d pi1 = l1.PointAt(t1);
        //                pts.Add(pi0);
        //                xt0.Add(i + t0);
        //                xt1.Add(j + t1);
        //            }
        //        }

        //        // Rhino.RhinoApp.WriteLine(pts.Count.ToString());
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(pts[0]);
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(pts[1]);
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(jointBBox);
        //        //If no intersection is found modify existing polyline
        //        if (pts.Count < 2) {
        //            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(jointBBox);

        //            //Rhino.RhinoApp.WriteLine("Hi22");

        //            //List<Point3d> cpPoints = new List<Point3d>();
        //            //List<double> cpPointsT = new List<double>();

        //            //for (int i = 0; i < platePolylineBackUp.SegmentCount; i++)
        //            //{
        //            //   bool flagFound = false;
        //            //    for(int j = 0; j < jointPolyline.Count-1; j++)
        //            //    {
        //            //        double cpT = platePolylineBackUp.SegmentAt(i).ClosestParameter(jointPolyline[j]);
        //            //        if (cpT < 0 || cpT > 1) continue;
        //            //        Point3d cpOutline = platePolylineBackUp.SegmentAt(i).PointAt(cpT);
        //            //        if (cpOutline.DistanceToSquared(jointPolyline[j]) < 0.0001)
        //            //        {
        //            //            cpPoints.Add(jointPolyline[j]);
        //            //            cpPointsT.Add(cpT);
        //            //            //jointPolyline[j].Bake();
        //            //            flagFound = true;
        //            //        }
        //            //    }
        //            //    if (flagFound) break;
        //            //}

        //            //var cpPointsArray = cpPoints.ToArray();
        //            //var cpPointsTArray = cpPointsT.ToArray();
        //            //Array.Sort(cpPointsTArray, cpPointsArray);
        //            //cpPointsArray[0].Bake();
        //            //cpPointsArray[cpPointsArray.Length - 1].Bake();
        //            //pts.Clear();
        //            //pts.Add(cpPointsArray[0]);
        //            //pts.Add(cpPointsArray[cpPointsArray.Length - 1]);

        //            double[] dist = new double[platePolylineBackUp.SegmentCount];
        //            Tuple<int, int>[] ids = new Tuple<int, int>[platePolylineBackUp.SegmentCount];
        //            int[] idsBbox = new int[jointBBox.SegmentCount];

        //            for (int i = 0; i < platePolylineBackUp.SegmentCount; i++) {
        //                //Take edge mid point
        //                Point3d edgeMidP = platePolylineBackUp.SegmentAt(i).PointAt(0.5);

        //                double cpT = jointBBox.ClosestParameter(edgeMidP);
        //                Point3d cp = jointBBox.PointAt(cpT);

        //                dist[i] = cp.DistanceToSquared(edgeMidP);
        //                ids[i] = Tuple.Create(i, (int)Math.Floor(cpT));
        //            }

        //            Array.Sort(dist, ids);
        //            var cpe = ids[0];//closest edge
        //            int cpE = (int)Math.Floor(platePolyline.ClosestParameter(platePolylineBackUp.SegmentAt(cpe.Item1).PointAt(0.5)));

        //            int n = platePolyline.SegmentCount;
        //            int next = ((cpE + 1 % n) + n) % n;
        //            int prev = ((cpE - 1 % n) + n) % n;

        //            Line l = jointBBox.SegmentAt(cpe.Item2);
        //            Line l0 = platePolyline.SegmentAt(next);
        //            Line l1 = platePolyline.SegmentAt(prev);

        //            double t0, t1;
        //            Rhino.Geometry.Intersect.Intersection.LineLine(l, l0, out t0, out t1, 0.01, false);
        //            Point3d ptA = l.PointAt(t0);

        //            // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(ptA);

        //            y[cpE + 1] = ptA;

        //            pts.Add(ptA);

        //            if (cpE + 1 == n)
        //                y[0] = ptA;

        //            Rhino.Geometry.Intersect.Intersection.LineLine(l, l1, out t0, out t1, 0.01, false);

        //            y[cpE] = l.PointAt(t0);
        //            Point3d ptB = l.PointAt(t0);
        //            pts.Add(ptB);

        //            if (cpE == 0)
        //                y[y.Count - 1] = ptB;
        //        }

        //        //Insert both point to curve by change seam
        //        Polyline p0 = new Polyline(x);//joint curve
        //        Polyline p1 = new Polyline(y);//curve to insert to

        //        p0 = ChangeSeam(p0, pts[0]);//ChangeClosedPolylineSeam(p0, p0.ClosestParameter(pts[0]));
        //        p0 = ChangeSeam(p0, pts[1]);//ChangeClosedPolylineSeam(p0, p0.ClosestParameter(pts[1]));
        //        p1 = ChangeSeam(p1, pts[0]);//ChangeClosedPolylineSeam(p1, p1.ClosestParameter(pts[0]));
        //        p1 = ChangeSeam(p1, pts[1]);//ChangeClosedPolylineSeam(p1, p1.ClosestParameter(pts[1]));

        //        ////////////////////////////////////////////
        //        //Check which side is correct
        //        //Take point

        //        //A = p0;
        //        double t = (int)Math.Round(p0.ClosestParameter(pts[0]), 0) % p0.Count;
        //        t = (int)Math.Round(t, 0);
        //        Polyline p0A = new Polyline(p0.GetRange(0, 1 + (int)t));
        //        Polyline p0B = new Polyline(p0.GetRange((int)t, p0.Count - (int)t));

        //        //return platePolyline;

        //        Point3d pt0 = p0A.ToNurbsCurve().PointAtNormalizedLength(0.5);
        //        Point3d pt1 = p0B.ToNurbsCurve().PointAtNormalizedLength(0.5);

        //        //Print(Math.Round(p1.ClosestParameter(pts[0])).ToString());
        //        t = (int)Math.Round(p1.ClosestParameter(pts[0]), 0) % p1.Count;
        //        //Print(t.ToString());
        //        //Print(p1.ClosestParameter(pts[1]).ToString());

        //        p1.CollapseShortSegments(0.01);
        //        Polyline p1A = new Polyline(p1.GetRange(0, 1 + (int)t));
        //        Polyline p1B = new Polyline(p1.GetRange((int)t, p1.Count - (int)t));
        //        p1A.CollapseShortSegments(0.01);
        //        p1B.CollapseShortSegments(0.01);

        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p0A);
        //        // Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p0B);

        //        //  Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1A);
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1B) ;
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1B);

        //        //    Curve[] split = p1.ToNurbsCurve().Split(new double[]{0,t});
        //        //    Polyline p1A = new Polyline();
        //        //    Polyline p1B = new Polyline();
        //        //    split[0].TryGetPolyline(out p1A);
        //        //    split[1].TryGetPolyline(out p1B);

        //        double p1A_Len = p1A.Length;
        //        double p1B_Len = p1B.Length;
        //        p1Larger = p1A_Len > p1B_Len ? p1A : p1B;//

        //        //if(p1.Count > 7){
        //        //Print(p1.Count.ToString());

        //        //
        //        //      Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1A);
        //        //      Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p1B);
        //        //
        //        //      Curve curve = p1.ToNurbsCurve();
        //        //      curve.ChangeClosedCurveSeam(t);
        //        //      double tt = 0;
        //        //      curve.ClosestPoint(pts[1], out tt);
        //        //      Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(curve.PointAt(tt));

        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(   p1.ClosestPoint(pts[0]));
        //        // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(   pts[1]);
        //        //p1 = ChangeClosedPolylineSeam(p1, p1.ClosestParameter(pts[1]));
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(p1[0]);
        //        // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint( p1.PointAt(p1.ClosestParameter(pts[1])));

        //        // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(pts[0]);
        //        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(pts[1]);

        //        //}

        //        //Check Inclusion
        //        Polyline p0Joint = new Polyline();

        //        bool flag = false;

        //        if (jointPolyline.Count == 5) {
        //            if (takeOutside) {
        //                p0Joint = NGonsCore.Geometry.RhinClip.IsPointInside(y, pt0, 1e10, 0.0001) ? p0B : p0A;//0.0001
        //            } else {
        //                p0Joint = NGonsCore.Geometry.RhinClip.IsPointInside(y, pt0, 1e10, 0.0001) ? p0A : p0B;//0.0001
        //            }

        //            //Rhino.RhinoApp.WriteLine(NGonsCore.Geometry.RhinClip.IsPointInside(y, pt0, 1e10, 0.01).ToString());
        //            //pt0.Bake(); //p0A.Bake();
        //            //y.Bake();
        //            //Rhino.RhinoApp.WriteLine("hi");
        //        } else {
        //            for (int i = 1; i < p0A.Count - 1; i++) { //Iterate through first polyline points
        //                if (jointBBox.ClosestPoint(p0A[i]).DistanceToSquared(p0A[i]) > 0.0001) {//BUG - Changed from 0.1 to 0.001
        //                    flag = true;
        //                    break;
        //                }
        //            }
        //            p0Joint = flag ? p0A : p0B;
        //        }

        //        //p0Joint.Bake();
        //        //jointBBox.Bake();
        //        //

        //        //if (p0Joint.Length > p0A.Length)
        //        //{
        //        //    pt0.Bake();
        //        //    y.Bake();
        //        //    p0Joint.Bake();
        //        //}

        //        //if (p0Joint.Length > p0B.Length)
        //        //{
        //        //    pt1.Bake();
        //        //    y.Bake();
        //        //    p0Joint.Bake();
        //        //}

        //        //Curve c = y.ToNurbsCurve();
        //        //Plane plane = Plane.Unset;
        //        //Plane.FitPlaneToPoints(y, out plane);
        //        //PointContainment pCon0 = c.Contains(pt0, plane, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
        //        //PointContainment pCon1 = c.Contains(pt1, plane, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);

        //        ////If none of point are inside plate
        //        //Polyline p0Joint = new Polyline();
        //        //if (pCon0 == PointContainment.Outside && pCon1 == PointContainment.Outside)
        //        //{
        //        //    p0Joint = y.ClosestPoint(pt0).DistanceToSquared(pt0) < y.ClosestPoint(pt1).DistanceToSquared(pt1) ? p0A : p0B;
        //        //    //Print("a");
        //        //}
        //        //else
        //        //{
        //        //    p0Joint = (pCon0 == PointContainment.Inside || pCon0 == PointContainment.Coincident) ? p0A : p0B;
        //        //    //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(p0Joint);
        //        //    //Print("b");
        //        //}

        //        ////////////////////////////////////////////

        //        //MergePolylines
        //        if (p0Joint[0].DistanceToSquared(p1Larger[p1Larger.Count - 1]) > 0.01)
        //            p0Joint.Reverse();

        //        p0Joint.CollapseShortSegments(0.01);

        //        p1Larger.AddRange(p0Joint.GetRange(1, p0Joint.Count - 2));
        //        if (!p1Larger.IsClosed)
        //            p1Larger.Add(p1Larger[0]);
        //        p1Larger.CollapseShortSegments(0.01);
        //        //p1Larger.AddRange(p0Joint);
        //    } catch (Exception e) {
        //        Rhino.RhinoApp.WriteLine(e.ToString());
        //        Rhino.RhinoApp.WriteLine(e.ToString());
        //        //jointBBox.Bake();
        //        return platePolyline;
        //    }

        //    return p1Larger;

        //    //Remove sharp point from a polyline
        //    Polyline p1LargerClean = new Polyline();
        //    p1LargerClean.Add(p1Larger[0]);
        //    Plane plane = jointBBox.plane();

        //    for (int i = 1; i < p1Larger.Count - 1; i++) {
        //        Vector3d v0 = p1Larger[i] - p1Larger[i + 1];
        //        Vector3d v1 = p1Larger[i] - p1Larger[i - 1];

        //        double angle = Math.Abs(Vector3d.VectorAngle(v0, v1, plane));
        //        if (angle > Rhino.RhinoMath.ToRadians(1) && angle < Rhino.RhinoMath.ToRadians(359))
        //            p1LargerClean.Add(p1Larger[i]);
        //    }
        //    p1LargerClean.Close();

        //    //Output
        //    return p1LargerClean;
        //}

        public static Polyline ChangeSeam(Polyline p, Point3d pt)
        {
            double t = p.ClosestParameter(pt);
            t = t % 1 < 0.0001 ? Math.Round(t, 0) : t;

            if (t == 0 || t == p.Count - 1) return p;

            Polyline pline = new Polyline(p);
            pline.RemoveAt(pline.Count - 1);

            var pts = GetRingedRange(pline.ToList(), (int)Math.Ceiling(t), pline.Count);
            Polyline polyline = new Polyline();
            if ((t % 1) > 0.0001 && (t % 1) < 0.9999)
            {
                polyline.Add(p.PointAt(t));
            }
            polyline.AddRange(pts);
            polyline.Add(polyline[0]);

            return polyline;
        }

        public static List<Point3d> GetRingedRange<Point3d>(
        List<Point3d> t,
        int start,
        int n
        )
        {
            Func<decimal, decimal, int> trueModulo = (a, b) =>
            {
                if (a >= 0)
                    return (int)(a % b);
                var roundsToPositive = Math.Ceiling(Math.Abs(Convert.ToDecimal(a) / b));
                a += Convert.ToInt32(roundsToPositive * b);
                return (int)(a % b);
            };

            start = trueModulo(start, t.Count);

            start = start % t.Count;
            List<Point3d> range = new List<Point3d>();

            for (int i = start; n > 0 ? i < start + n : i > start + n; i += n > 0 ? 1 : -1)
            {
                range.Add(t[trueModulo(i, t.Count)]);
            }

            return range;
        }

        public static Polyline ChangeClosedPolylineSeam(Polyline polyline, double t)
        {
            Polyline tempPolyline;
            Curve curve = polyline.ToNurbsCurve();
            curve.ChangeClosedCurveSeam(t);

            curve.TryGetPolyline(out tempPolyline);
            tempPolyline.CollapseShortSegments(0.01);
            return new Polyline(tempPolyline);
        }

        public static void InsertPolyline(this Polyline x, IEnumerable<Polyline> y)
        {
            foreach (Polyline poly in y)
            {
                double t0 = x.ClosestParameter(poly[0]);
                double t1 = x.ClosestParameter(poly.Last);
                if (t0 > t1)
                    poly.Reverse();
                x.InsertRange((int)Math.Ceiling(t0), poly);
            }
        }

        public static Polyline InsertPline(this Polyline xx, Polyline poly)
        {
            Polyline pline = xx.Duplicate();
            Polyline plineToInsert = new Polyline(poly);
            double t0 = pline.ClosestParameter(plineToInsert[0]);
            double t1 = pline.ClosestParameter(plineToInsert.Last);
            if (t0 > t1)
                plineToInsert.Reverse();
            pline.InsertRange((int)Math.Ceiling(t0), plineToInsert);

            //Rhino.RhinoApp.WriteLine(pline.ClosestIndex(xx[0]).ToString());
            //Rhino.RhinoApp.WriteLine(pline.ClosestIndex(xx[1]).ToString());

            return pline;
        }

        public static void InsertPolyline(this Polyline x, Polyline poly)
        {
            double t0 = x.ClosestParameter(poly[0]);
            double t1 = x.ClosestParameter(poly.Last);
            if (t0 > t1)
                poly.Reverse();
            x.InsertRange((int)Math.Ceiling(t0), poly);
        }

        //public _static void ReplacePlineSegments(this Polyline x, Polyline pline)
        //{
        //    Polyline poly = new Polyline(pline);

        //    //Find closest polyline parameters
        //    double t0 = x.ClosestParameter(poly[0]);
        //    double t1 = x.ClosestParameter(poly.Last);
        //    double tmid = x.ClosestParameter(poly.PointAt(poly.Count * 0.5));
        //    if (t0 > t1)
        //    {
        //        poly.Reverse();
        //        double t0_ = t0;
        //        double t1_ = t1;
        //        t1 = t0_;
        //        t0 = t1_;

        //    }

        //    //Check direction where to insert
        //    Interval interval0 = new Interval((int)Math.Ceiling(t0), (int)Math.Floor(t1));

        //    Polyline plineSide = new Polyline();
        //    //Take positive side
        //    if (interval0.IncludesParameter(tmid))
        //    {
        //        for (int i = (int)interval0.T0; i != (int)interval0.T1 + 1; i++)
        //        {
        //            plineSide.Add(poly[MathUtil.Wrap(i,poly.Count)]);
        //        }

        //        //Take negative side
        //    }
        //    else
        //    {
        //        for (int i = (int)interval0.T0; i != (int)interval0.T1 + 1; i++)
        //        {
        //            plineSide.Add(poly[MathUtil.Wrap(i, poly.Count)]);
        //        }

        //    }

        //}

        public static List<Polyline> InterpolateTwoLines(Line l0, Line l1, int n = 1)
        {
            List<Polyline> squares = new List<Polyline>();

            if (n > 0)
            {
                Point3d[] interpolatePt0 = PointUtil.InterpolatePoints(l0.From, l0.To, n); //bottom interpolation
                Point3d[] interpolatePt1 = PointUtil.InterpolatePoints(l1.From, l1.To, n); //top inerpolation

                for (int i = 0; i < n + 1; i++)
                {
                    Polyline polyline = new Polyline(new[] {
                    interpolatePt0[i],
                    interpolatePt0[i+1],
                    interpolatePt1[i+1],
                    interpolatePt1[i],
                    interpolatePt0[i]
                });
                    squares.Add(polyline);
                }

                return squares;
            }
            else
            {
                squares.Add(new Polyline(new[] {
                    l0.From,
                    l0.To,
                    l1.To,
                    l1.From,
                    l0.From
                    }));
            }

            return squares;
        }

        public static Line PlanePlanePlanePlane(Plane planeToIntersectLine0, Plane planeToIntersectLine1, Plane planeToCutLine0, Plane planeToCutLine1)
        {
            Rhino.Geometry.Intersect.Intersection.PlanePlane(planeToIntersectLine0, planeToIntersectLine1, out Line intersectedLine);
            Rhino.Geometry.Intersect.Intersection.LinePlane(intersectedLine, planeToCutLine0, out double t0);
            Rhino.Geometry.Intersect.Intersection.LinePlane(intersectedLine, planeToCutLine1, out double t1);
            Line line = new Line(intersectedLine.PointAt(t0), intersectedLine.PointAt(t1));
            return line;
        }

        public static Polyline PlaneLines(Plane plane, IEnumerable<Line> lines, bool close = true)
        {
            Polyline polyline = new Polyline();

            foreach (Line l in lines)
            {
                Rhino.Geometry.Intersect.Intersection.LinePlane(l, plane, out double t);
                polyline.Add(l.PointAt(t));
            }
            polyline.Add(polyline[0]);
            return polyline;
        }

        public static Plane MovePlane(Plane Base, double Thickness)
        {
            return new Plane(Base.Origin + Base.Normal * (Thickness / 2), Base.Normal);
        }

        public static Point3d IntPtPln1(Line line, Plane plane)
        {
            double pm;
            Rhino.Geometry.Intersect.Intersection.LinePlane(line, plane, out pm);
            return line.PointAt(pm);
        }

        public static Line IntPtPln(Point3d origin, Vector3d axis, Plane planeBot, Plane planeTop)
        {
            Line lineTEMP = new Line(origin, origin + axis);
            double pmBot;
            double pmTop;
            Rhino.Geometry.Intersect.Intersection.LinePlane(lineTEMP, planeBot, out pmBot);
            Rhino.Geometry.Intersect.Intersection.LinePlane(lineTEMP, planeTop, out pmTop);

            return new Line(lineTEMP.PointAt(pmBot), lineTEMP.PointAt(pmTop));
        }

        public static Line tweenLine(Line l0, Line l1, double t = 0.5)
        {
            return new Line(MathUtil.Lerp(l0.From, l1.From, t), MathUtil.Lerp(l0.To, l1.To, t));
        }

        public static Polyline tweenPolylines(Polyline l0, Polyline l1, double t = 0.5)
        {
            Polyline p = new Polyline(l0);

            for (int i = 0; i < l0.Count; i++)
            {
                p[i] = MathUtil.Lerp(l0[i], l1[i], t);
            }

            return p;
        }

        public static Polyline PolylineFromPlanes(Plane basePlane, List<Plane> sidePlanes, bool close = true)
        {
            Polyline polyline = new Polyline();

            for (int i = 0; i < sidePlanes.Count - 1; i++)
            {
                Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(basePlane, sidePlanes[i], sidePlanes[i + 1], out Point3d pt);
                polyline.Add(pt);
            }

            Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(basePlane, sidePlanes[sidePlanes.Count - 1], sidePlanes[0], out Point3d pt1);
            polyline.Add(pt1);

            if (close)
                polyline.Add(polyline[0]);

            return polyline;
        }

        /// <summary>
        /// Move two planes by z axis and output one that is closer to target point
        /// </summary>
        /// <param name="polyline"></param>
        /// <param name="point"></param>
        /// <param name="dist"></param>
        /// <returns></returns>
        public static Plane MovePolylinePlaneToPoint(this Polyline polyline, Point3d point, double dist)
        {
            Plane planeA = polyline.plane();
            planeA.Translate(planeA.ZAxis * dist);

            Plane planeB = polyline.plane();
            planeB.Translate(planeB.ZAxis * -dist);

            double dA = PointUtil.FastDistance(point, planeA.Origin);
            double dB = PointUtil.FastDistance(point, planeB.Origin);

            if (dA > dB)
                return planeB;

            return planeA;
        }

        //Only works for fully closed objects
        //public static Line[] MaleFemale(this Polyline[] twoPlanarPolys, double dist) {
        //    Mesh mesh = Loft(twoPlanarPolys, 0.001);

        //    //Ngons planes
        //    //Get ngons planes and copy it to have offset and original set
        //    Plane[] planesO = mesh.GetNgonPlanes();

        //    //1.Get all edges
        //    int[][] nGonTV = mesh.GetNGonsTopoBoundaries();
        //    HashSet<int> allE = mesh.GetAllNGonEdges(nGonTV);

        //    //2.Get all edges in ngon faces
        //    int[][] ngonEdges = mesh.GetNGonFacesEdges(nGonTV);

        //    //3.Get ngons connected to edges
        //    int[][] edgeNgons = mesh.GetNgonsConnectedToNGonsEdges(allE);

        //    //loop through one face edges and get lines
        //    int i = 0;

        //    //Offset planes instead of current one
        //    Plane[] planes = mesh.GetNgonPlanes();
        //    planes.MovePlaneArrayByAxis(dist, i);

        //    Line[] lines = new Line[ngonEdges[i].Length];

        //    for (int j = 0; j < ngonEdges[i].Length; j++) {
        //        //Get edge faces
        //        int[] ngons = edgeNgons[ngonEdges[i][j]];
        //        Intersection.PlanePlane(planes[ngons[0]], planes[ngons[1]], out lines[i]);
        //    }

        //    //Mark edges id for insection vector?? If none supplied use perpedicular

        //    //Intersect 2 planes

        //    //Intersect plane line
        //    //Intersect other plane line

        //    return lines;
        //}

        ////Only works for fully closed objects
        //public static Line[][] OffsetClosedMesh(this Polyline[] twoPlanarPolys, double dist) {
        //    //Loft two polylines to get solid box to extract normals
        //    Mesh mesh = Loft(twoPlanarPolys, 0.001);
        //    int[][] boundaries = mesh.GetNGonsTopoBoundaries();
        //    HashSet<int> allE = mesh.GetAllNGonEdges(boundaries);
        //    HashSet<int> allTV = mesh.GetAllNGonsTopoVertices();

        //    //Get ngons planes and copy it to have offset and original set
        //    Plane[] planesO = mesh.GetNgonPlanes();
        //    Plane[] planes = mesh.GetNgonPlanes();

        //    for (int j = 0; j < planes.Length; j++)
        //        planes[j].Translate(planes[j].Normal * -dist);

        //    //Get all edges end id
        //    int[][] pairs = mesh.GetAllNGonEdges_TopoVertices(boundaries, allE);

        //    //Now since we have closed volue we can compute planeplaneplane intersection

        //    //This is an array of all vertices
        //    //But they are not in order 2 6 4 0 12 and so on because numbers represent mesh vertices not ngon vertices
        //    var vToFDictionary = mesh.GetNGonsConnectedToNGonTopologyVerticesDictionary(allTV);

        //    //These are all ngons all ngons topology edges - order matches -> allE numbers
        //    Dictionary<int, Line> lines = new Dictionary<int, Line>(allE.Count);

        //    for (int i = 0; i < allE.Count; i++) {
        //        int vertexIDA = pairs[i][0];
        //        int vertexIDB = pairs[i][1];
        //        int[] adjFA = vToFDictionary[vertexIDA];
        //        int[] adjFB = vToFDictionary[vertexIDB];
        //        Intersection.PlanePlanePlane(planes[adjFA[0]], planes[adjFA[1]], planes[adjFA[2]], out Point3d a);
        //        Intersection.PlanePlanePlane(planes[adjFB[0]], planes[adjFB[1]], planes[adjFB[2]], out Point3d b);
        //        lines.Add(allE.ElementAt(i), new Line(a, b));
        //        //lines[i] = new Line(a, b);
        //    }

        //    //Output
        //    int[][] NGonsE = mesh.GetNGonFacesEdges(boundaries);

        //    Line[][] faceLines = new Line[NGonsE.Length][];

        //    for (int i = 0; i < NGonsE.Length; i++) {
        //        faceLines[i] = new Line[NGonsE[i].Length];
        //        for (int j = 0; j < NGonsE[i].Length; j++) {
        //            faceLines[i][j] = lines[(NGonsE[i][j])];
        //        }
        //    }

        //    return faceLines;
        //}

        //public static Tuple<Polyline[][]> SingleDegreeJoints(this Polyline[] twoPlanarPolys, double dist, double jointSize, int d) {
        //    //Loft two polylines to get solid box to extract normals
        //    Mesh mesh = Loft(twoPlanarPolys, 0.001);
        //    Plane[] planesO = mesh.GetNgonPlanes();
        //    Plane[] planes = new Plane[planesO.Length - 2];
        //    Plane[] planesOO = new Plane[planesO.Length - 2];
        //    Array.Copy(planesO, 0, planes, 0, planesO.Length - 2);
        //    Array.Copy(planesO, 0, planesOO, 0, planesO.Length - 2);

        //    Plane[] biPlanesO = planes.BisectorPlaneArray();
        //    Plane[] biPlanes = biPlanesO;

        //    for (int j = 0; j < planes.Length; j++) {
        //        planes[j].Translate(planes[j].Normal * -dist);
        //        biPlanes[j].Translate(biPlanes[j].Normal * -jointSize);
        //    }

        //    //Take one segment of a line
        //    //Move it towards negative direction of plane normal
        //    //Intersect with neighbours polyline segmensts

        //    Polyline[][] po = new Polyline[twoPlanarPolys[0].SegmentCount][];

        //    for (int i = 0; i < twoPlanarPolys[0].SegmentCount; i++)
        //    //for (int i = 0; i < 1; i++)
        //    {
        //        int n = twoPlanarPolys[0].SegmentCount;
        //        int prev = (i - 1).Wrap(n);
        //        int next = (i + 1).Wrap(n);

        //        //Take original segments:
        //        Line lineT = twoPlanarPolys[0].SegmentAt(i);
        //        Line lineB = twoPlanarPolys[1].SegmentAt(i);

        //        //Create line from interesecting neighbour lines with current plane
        //        Line lineTO = IntersectionPlaneTwoLines(planes[i], twoPlanarPolys[0].SegmentAt(prev), twoPlanarPolys[0].SegmentAt(next));
        //        Line lineBO = IntersectionPlaneTwoLines(planes[i], twoPlanarPolys[1].SegmentAt(prev), twoPlanarPolys[1].SegmentAt(next));

        //        //Trim/Create new horizontal lines that polylines with bisect planes
        //        //Trim that polylines to know connection offset
        //        lineTO = lineTO.IntersectionLineTwoPlanes(planes[prev], planes[next]);
        //        lineBO = lineBO.IntersectionLineTwoPlanes(planes[prev], planes[next]);

        //        Line a = IntersectionLineTwoPlanes(lineT, biPlanes[i], biPlanes[prev]);
        //        Line b = IntersectionLineTwoPlanes(lineB, biPlanes[i], biPlanes[prev]);

        //        Line aO = IntersectionLineTwoPlanes(lineTO, biPlanes[i], biPlanes[prev]);
        //        Line bO = IntersectionLineTwoPlanes(lineBO, biPlanes[i], biPlanes[prev]);

        //        Line chamferA = new Line(a.From, aO.From);
        //        Line chamferB = new Line(b.From, bO.From);

        //        Intersection.LinePlane(chamferA, planesOO[next], out double t1);
        //        Intersection.LinePlane(chamferB, planesOO[next], out double t2);
        //        chamferA.From = chamferA.PointAt(t1);
        //        chamferB.From = chamferB.PointAt(t2);

        //        a.From = chamferA.From;
        //        b.From = chamferB.From;

        //        //Construct vector to trim and extend segments
        //        //po[i] = new[]
        //        //{
        //        //    lineT.ToP(),
        //        //    lineB.ToP(),
        //        //    lineTO.ToP(),
        //        //    lineBO.ToP()
        //        //};

        //        //Interpolate points for divisions this will not result in perpedicular elements
        //        po[i] = new[] {
        //            //DovetailPolyline(lineA, lineB, lineA_, lineB_, d),
        //            // DovetailPolyline(lineTO,lineBO,aO,  bO, d),

        //            DovetailPolylineShifted(  new []
        //                {
        //                    lineT.To ,
        //                    lineB.To,
        //                    a.From,//+ new Point3d(0,0,-4),
        //                    b.From,//+ new Point3d(0,0,4),
        //                    a.To ,
        //                    b.To,
        //                    lineT.From,
        //                    lineB.From
        //                } ,
        //                d),

        //            DovetailPolylineShifted(  new []
        //                {
        //                    lineTO.To,
        //                    lineBO.To,
        //                    aO.From,
        //                    bO.From,
        //                    aO.To,
        //                    bO.To,
        //                    lineTO.From,
        //                    lineBO.From,
        //                } ,
        //                d),

        //            lineT.ToP(),
        //            lineB.ToP(),
        //            lineTO.ToP(),
        //            lineBO.ToP(),
        //            chamferA.ToP(),
        //            chamferB.ToP()

        //            //    //new Polyline(new[]{lineTO.From,lineTO.To}),
        //            //    //new Polyline(new[]{lineBO.From,lineBO.To}),
        //            //    //new Polyline(new[]{aO.From,aO.To}),
        //            //    //new Polyline(new[]{bO.From,bO.To}),
        //        };
        //    }

        //    return new Tuple<Polyline[][]>(po);
        //}

        //public static Tuple<Polyline[][]> Dovetail(this Polyline[] twoPlanarPolys, double dist, int d) {
        //    //Loft two polylines to get solid box to extract normals
        //    Mesh mesh = Loft(twoPlanarPolys, 0.001);
        //    Plane[] planesO = mesh.GetNgonPlanes();
        //    Plane[] planes = mesh.GetNgonPlanes();

        //    for (int j = 0; j < planes.Length; j++)
        //        planes[j].Translate(planes[j].Normal * -dist);

        //    //Take one segment of a line
        //    //Move it towards negative direction of plane normal
        //    //Intersect with neighbours polyline segmensts

        //    Polyline[][] po = new Polyline[twoPlanarPolys[0].SegmentCount][];

        //    for (int j = 0; j < twoPlanarPolys[0].SegmentCount; j++) {
        //        int i = j;
        //        int n = twoPlanarPolys[0].SegmentCount;
        //        int prev = (i - 1).Wrap(n);
        //        int next = (i + 1).Wrap(n);

        //        //Trip original segments:
        //        Line lineA_O = twoPlanarPolys[0].SegmentAt(i).IntersectionLineTwoPlanes(planes[prev], planes[next]);
        //        Line lineB_O = twoPlanarPolys[1].SegmentAt(i).IntersectionLineTwoPlanes(planes[prev], planes[next]);

        //        //Offset polyline by intersection points
        //        Line lineA = IntersectionPlaneTwoLines(planes[i], twoPlanarPolys[0].SegmentAt(prev), twoPlanarPolys[0].SegmentAt(next));
        //        Line lineB = IntersectionPlaneTwoLines(planes[i], twoPlanarPolys[1].SegmentAt(prev), twoPlanarPolys[1].SegmentAt(next));

        //        //Trim that polylines to know connection offset
        //        Line lineA_ = lineA.IntersectionLineTwoPlanes(planes[prev], planes[next]);
        //        Line lineB_ = lineB.IntersectionLineTwoPlanes(planes[prev], planes[next]);

        //        //Interpolate points for divisions this will not result in perpedicular elements
        //        po[i] = new[] {
        //            DovetailPolyline(lineA, lineB, lineA_, lineB_, d),
        //            DovetailPolyline(twoPlanarPolys[0].SegmentAt(i), twoPlanarPolys[1].SegmentAt(i),lineA_O, lineB_O, d)
        //        };
        //    }

        //    return new Tuple<Polyline[][]>(po);
        //}

        public static Polyline DovetailPolyline(Line lineA, Line lineB, Line lineA_, Line lineB_, int d)
        {
            Point3d[] interA = PointUtil.InterpolatePoints(lineA.From, lineB.From, d);
            Point3d[] interA_ = PointUtil.InterpolatePoints(lineA_.From, lineB_.From, d);

            Point3d[] interB;
            Point3d[] interB_;

            if (d % 2 == 1)
            {
                interB = PointUtil.InterpolatePoints(lineB.To, lineA.To, d);
                interB_ = PointUtil.InterpolatePoints(lineB_.To, lineA_.To, d);
            }
            else
            {
                interB_ = PointUtil.InterpolatePoints(lineB.To, lineA.To, d);
                interB = PointUtil.InterpolatePoints(lineB_.To, lineA_.To, d);
            }

            Polyline poly = new Polyline();
            Polyline temp = new Polyline();

            for (int j = 0; j < interA.Length - 1; j++)
            {
                if (j % 2 == 0)
                {
                    poly.Add(interA[j]);
                    poly.Add(interA[j + 1]);
                    temp.Add(interB[j]);
                    temp.Add(interB[j + 1]);
                }
                else
                {
                    poly.Add(interA_[j]);
                    poly.Add(interA_[j + 1]);
                    temp.Add(interB_[j]);
                    temp.Add(interB_[j + 1]);
                }
            }

            poly.AddRange(temp);
            poly.Close();
            return poly;
        }

        public static Polyline DovetailPolylineShifted(Point3d[] pts, int d)
        {
            Point3d[] interA = PointUtil.InterpolatePoints(pts[0], pts[1], d);
            Point3d[] interA_ = PointUtil.InterpolatePoints(pts[2], pts[3], d);

            Point3d[] interB;
            Point3d[] interB_;

            if (d % 2 == 1)
            {
                interB = PointUtil.InterpolatePoints(pts[5], pts[4], d);
                interB_ = PointUtil.InterpolatePoints(pts[7], pts[6], d);
            }
            else
            {
                interB_ = PointUtil.InterpolatePoints(pts[5], pts[4], d);
                interB = PointUtil.InterpolatePoints(pts[7], pts[6], d);
            }

            Polyline poly = new Polyline();
            Polyline temp = new Polyline();

            for (int j = 0; j < interA.Length - 1; j++)
            {
                if (j % 2 == 0)
                {
                    poly.Add(interA[j]);
                    poly.Add(interA[j + 1]);
                    temp.Add(interB[j]);
                    temp.Add(interB[j + 1]);
                }
                else
                {
                    poly.Add(interA_[j]);
                    poly.Add(interA_[j + 1]);
                    temp.Add(interB_[j]);
                    temp.Add(interB_[j + 1]);
                }
            }

            poly.AddRange(temp);
            poly.Close();
            return poly;
        }

        public static Line IntersectionPlaneTwoLines(Plane p, Line lnA, Line lnB)
        {
            Intersection.LinePlane(lnA, p, out double t1);
            Intersection.LinePlane(lnB, p, out double t2);
            return new Line(lnA.PointAt(t1), lnB.PointAt(t2));
        }

        public static double[] IntersectionPlaneTwoLinesT(Plane p, Line lnA, Line lnB)
        {
            Intersection.LinePlane(lnA, p, out double t1);
            Intersection.LinePlane(lnB, p, out double t2);
            return new[] { t1, t2 };
        }

        public static Line IntersectionLineTwoPlanes(this Line line, Plane pa, Plane pb)
        {
            Intersection.LinePlane(line, pa, out double t1);
            Intersection.LinePlane(line, pb, out double t2);
            return new Line(line.PointAt(t1), line.PointAt(t2));
        }

        public static double[] IntersectionLineTwoPlanesT(this Line line, Plane pa, Plane pb)
        {
            Intersection.LinePlane(line, pa, out double t1);
            Intersection.LinePlane(line, pb, out double t2);
            return new[] { t1, t2 };
        }

        public static Mesh Loft(Polyline[] twoPolys, double weld, bool cap = true)
        {
            Mesh mesh = new Mesh();
            mesh.Vertices.AddVertices(twoPolys[0]);
            mesh.Vertices.AddVertices(twoPolys[1]);

            if (cap)
            {
                Mesh Top = MeshCreate.MeshFromPolylines(new[] { twoPolys[0] }, weld);
                Polyline p = new Polyline(twoPolys[1]);

                p.CollapseShortSegments(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                Mesh Bottom = Mesh.CreateFromClosedPolyline(p);
                Bottom.Flip(true, true, true);
                int[] tempV = Enumerable.Range(0, Bottom.Vertices.Count).ToArray();
                int[] tempF = Enumerable.Range(0, Bottom.Faces.Count).ToArray();
                Bottom.Ngons.AddNgon(MeshNgon.Create(tempV, tempF));
                Bottom.Normals.ComputeNormals();

                int n = twoPolys[1].Count;

                if (twoPolys[0].Count >= 2)
                {
                    int f = mesh.Faces.Count - 1;
                    if (twoPolys[0].Count == twoPolys[1].Count)
                        for (int j = 0; j < n - 1; j++)
                        {
                            mesh.Faces.AddFace(n + j, n + j + 1, j + 1, j);
                            f++;
                            mesh.Ngons.AddNgon(MeshNgon.Create(new[] { n + j, n + j + 1, j + 1, j }, new[] { f }));
                        }
                }

                mesh.Append(Top);
                mesh.Append(Bottom);
            }
            else
            {
                int n = twoPolys[1].Count;

                if (twoPolys[0].Count >= 2)
                {
                    int f = mesh.Faces.Count - 1;
                    if (twoPolys[0].Count == twoPolys[1].Count)
                        for (int j = 0; j < n - 1; j++)
                        {
                            mesh.Faces.AddFace(n + j, n + j + 1, j + 1, j);
                            f++;
                            mesh.Ngons.AddNgon(MeshNgon.Create(new[] { n + j, n + j + 1, j + 1, j }, new[] { f }));
                        }
                }
            }

            //This is optional.
            if (weld > 0)
                mesh.WeldUsingRTree(weld);
            else
                mesh.Flip(true, true, true);

            return mesh;
        }

        public static Polyline[] ToPolylines(this IEnumerable<Curve> nurbsCurves, bool collapseShortSegments = true)
        {
            Polyline[] p = new Polyline[nurbsCurves.Count()];

            for (int i = 0; i < nurbsCurves.Count(); i++)
            {
                nurbsCurves.ElementAt(i).TryGetPolyline(out Polyline polyline);
                p[i] = nurbsCurves.ElementAt(i).ToPolyline(collapseShortSegments);
            }

            return p;
        }

        public static Polyline ToPolyline(this Curve curve, bool collapseShortSegments = true)
        {
            curve.TryGetPolyline(out Polyline polyline);
            if (collapseShortSegments)
                polyline.CollapseShortSegments(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
            return polyline;
        }

        public static Polyline[] ToPolylinesFromCP(this IEnumerable<Curve> curves, double collapseShortSegments = 0.01)
        {
            Polyline[] p = new Polyline[curves.Count()];

            int j = 0;
            foreach (Curve curve in curves)
            {
                p[j++] = ToPolylineFromCP(curve, collapseShortSegments);
            }

            return p;
        }

        public static Polyline ToPolylineFromCP(this Curve curve, double collapseShortSegments = 0.01)
        {
            Polyline polyline = new Polyline();
            if (curve.TryGetPolyline(out polyline))
            {
                polyline.CollapseShortSegments(collapseShortSegments);
                return polyline;
            }

            NurbsCurve c = curve.ToNurbsCurve();

            Point3d[] points = new Point3d[c.Points.Count];

            for (int i = 0; i < c.Points.Count; i++)
                c.Points.GetPoint(i, out points[i]);

            polyline = new Polyline(points);

            //What the fuck these two lines
            c = polyline.ToNurbsCurve();
            c.TryGetPolyline(out polyline);

            if (collapseShortSegments > 0)
                polyline.CollapseShortSegments(collapseShortSegments);
            polyline = new Polyline(polyline);

            //polyline.CollapseShortSegments(1);
            return polyline;
        }

        public static int[] append(int j)
        {
            var srsEnum = Enumerable.Range(0, j + 1);
            var arrVal = new List<int>();
            arrVal = srsEnum.ToList();
            arrVal.Add(0);
            return arrVal.ToArray();
        }

        public static Plane[] Planes(this Polyline[] polylines)
        {
            Plane[] p = new Plane[polylines.Length];

            for (int i = 0; i < polylines.Length; i++)
            {
                //p[i] = new Plane(polylines[i].CenterPoint(), polylines[i].Normal());
                p[i] = polylines[i].plane();
            }

            return p;
        }

        public static Plane plane(this Polyline polylines)
        {
            //return polylines.plane();
            return new Plane(polylines.CenterPoint(), polylines.AverageNormal());
        }

        public static Vector3d AverageNormal(this Polyline p)
        {
            //PolyFace item = this[index];
            int len = p.Count - 1;
            Vector3d vector3d = new Vector3d();
            int count = checked(len - 1);

            for (int i = 0; i <= count; i++)
            {
                int num = ((i - 1) + len) % len;
                int item1 = (checked(i + 1) + len) % len;
                Point3d point3d = p[num];
                Point3d point3d1 = p[item1];
                Point3d item2 = p[i];
                vector3d = vector3d + Vector3d.CrossProduct(new Vector3d(item2 - point3d), new Vector3d(point3d1 - item2));
            }

            if (vector3d.X == 0 & vector3d.Y == 0 & vector3d.Z == 0)
                vector3d.Unitize();

            return vector3d;
        }

        public static Vector3d Normal(this Polyline p)
        {
            var n = Vector3d.Unset;
            if (null != p && p.Count - 1 > 3)
            {
                var a = p[2] - p[0];
                var b = p[3] - p[1];
                n = Vector3d.CrossProduct(a, b);
                n.Unitize();
            }
            return n;
        }

        public static Polyline[][] LoftPolylines(Polyline[][] polylines)
        {
            Polyline[][] p = new Polyline[polylines.Length][];

            for (int i = 0; i < polylines.Length; i++)
            {
                if (polylines[i].Length >= 2)
                {
                    if (polylines[i][0].Count == polylines[i][1].Count)
                    {
                        p[i] = new Polyline[polylines[i][0].Count - 1];

                        for (int j = 0; j < polylines[i][0].Count - 1; j++)
                            p[i][j] = new Polyline() { polylines[i][0][j], polylines[i][0][j + 1], polylines[i][1][j + 1], polylines[i][1][j], polylines[i][0][j] };
                    }
                    else
                        p[i] = new Polyline[] { new Polyline() };
                }
            }

            return p;
        }

        public static Polyline[] LoftTwoPolylines(Polyline[] polylines)
        {
            Polyline[] p = new Polyline[polylines[0].Count - 1];

            if (polylines.Length >= 2)
            {
                if (polylines[0].Count == polylines[1].Count)
                    for (int j = 0; j < polylines[0].Count - 1; j++)
                        p[j] = new Polyline() { polylines[0][j], polylines[0][j + 1], polylines[1][j + 1], polylines[1][j], polylines[0][j] };
                else
                    p = new Polyline[] { new Polyline() };
            }
            return p;
        }

        public static void ShiftPolyline(this Polyline A, int n)
        {
            A.RemoveAt(A.Count - 1);

            for (int j = 0; j < n; j++)
            {
                int len = A.Count; //self explanatory
                var tmp = A[len - 1]; //save last element value
                for (int i = len - 1; i > 0; i--) //starting from the end to begining
                    A[i] = A[i - 1]; //assign value of the previous element
                A[0] = tmp; //now "rotate" last to first.
            }

            A.Add(A[0]);
        }

        public static Polyline ShiftPline(this Polyline a, int n = 1)
        {
            Polyline A = new Polyline(a);

            A.RemoveAt(A.Count - 1);
            int nn = MathUtil.Wrap(n, A.Count);

            for (int j = 0; j < nn; j++)
            {
                int len = A.Count; //self explanatory
                var tmp = A[len - 1]; //save last element value
                for (int i = len - 1; i > 0; i--) //starting from the end to begining
                    A[i] = A[i - 1]; //assign value of the previous element
                A[0] = tmp; //now "rotate" last to first.
            }

            A.Add(A[0]);

            return A;
        }

        public static void Close(this Polyline p)
        {
            if (p.Count > 2)
                if (!p.IsClosed)
                    p.Add(p[0]);
        }

        public static Polyline ToP(this Line line)
        {
            return new Polyline(new[] { line.From, line.To });
        }

        public static Curve[] ToCurveArray(this Polyline[] polyline)
        {
            Curve[] curves = new Curve[polyline.Length];

            for (int i = 0; i < polyline.Length; i++)
                curves[i] = polyline[i].ToNurbsCurve();

            return curves;
        }

        private static Random random = new Random();

        public static Polyline ExpandDuplicatedPoints(this Polyline polyline, double tolerance = 1E-5)
        {
            Polyline poly = new Polyline(polyline.ToArray());

            Plane.FitPlaneToPoints(poly, out Plane plane);

            for (int i = 1; i < poly.Count - 1; i += 2)
            {
                double dist = poly[i].DistanceTo(poly[i + 1]);
                double dist2 = poly[i].DistanceTo(poly[MathUtil.Wrap(i - 1, poly.Count)]);
                if (dist < tolerance)
                {
                    Point3d pt = poly[i];
                    double t = poly.ClosestParameter(pt);
                    poly[i] = poly.PointAt(t + 0.00001);
                }

                if (dist2 < tolerance)
                {
                    Point3d pt = poly[i];
                    double t = poly.ClosestParameter(pt);
                    poly[i] = poly.PointAt(t - 0.00001);
                }
            }

            return poly;
        }

        //public static Rhino.Geometry.Mesh LoftPolylineWithHoles(Polyline[] polylines0, Polyline[] polylines1) {
        //    Mesh mesh0 = NGonsCore.MeshCreate.MeshFromClosedPolylineWithHoles(polylines0);
        //    Mesh mesh1 = NGonsCore.MeshCreate.MeshFromClosedPolylineWithHoles(polylines1);
        //    //mesh0.Bake();
        //    //polylines0[0].Bake();
        //   // polylines1[0].Bake();

        //    mesh1.Flip(true, true, true);
        //    Mesh m = new Mesh();
        //    m.Append(mesh0);
        //    m.Append(mesh1);

        //    int n = 0;

        //    foreach (var p in polylines0)
        //        n += p.Count - 1;

        //    int c = 0;

        //    for (int i = 0; i < polylines0.Length; i++) {
        //        int counter = polylines0[i].Count - 1;

        //        for (int j = 0; j < counter; j++) {
        //            if (j == counter - 1)
        //                m.Faces.AddFace(c + j, c + 0, c + n + 0, c + n + j);
        //            else
        //                m.Faces.AddFace(c + j, c + j + 1, c + n + (j + 1), c + n + j);
        //        }

        //        c += counter;
        //    }

        //    //m.Clean();
        //    if (m.SolidOrientation() == -1)
        //        m.Flip(true, true, true);

        //    //m.Vertices.CombineIdentical(true, true);
        //   // m.Vertices.CullUnused();
        //    //m.Weld(3.14159265358979);
        //    m.FaceNormals.ComputeFaceNormals();
        //    m.Normals.ComputeNormals();

        //    // m.WeldFull(0.001);

        //    //m.Unweld(0, true);

        //    m.UnifyNormals();
        //    m.FaceNormals.UnitizeFaceNormals();
        //    m.Unweld(0.001, true);
        //    return m;

        //}

        //public static Rhino.Geometry.Mesh[][] LoftPolylineWithHoles(Polyline[][][] polylines0, Polyline[][][] polylines1) {
        //    Mesh[][] meshes = new Mesh[polylines0.Length][];

        //    for (int i = 0; i < polylines0.Length; i++) {
        //        meshes[i] = new Mesh[polylines0[i].Length];

        //        for (int j = 0; j < polylines0[i].Length; j++) {
        //            meshes[i][j] = LoftPolylineWithHoles(polylines0[i][j], polylines1[i][j]);
        //        }

        //    }

        //    return meshes;
        //}

        public static Rhino.Geometry.Mesh LoftTwoCurves(Curve C0, Curve C1, int N = 2, bool E = true)
        {
            if (N < 1)
                N = 2;

            Mesh mesh = new Mesh();

            if (C0 != null && C1 != null)
            {
                Polyline p0;
                Polyline p1;

                bool f0 = C0.TryGetPolyline(out p0);
                bool f1 = C1.TryGetPolyline(out p1);

                if ((f0 && f1 && p0.Count == p1.Count) == false)
                {
                    p0 = divideByCount(C0, N, C0.IsClosed);
                    p1 = divideByCount(C1, N, C0.IsClosed);
                }

                //If 2 polylines and number of points the same

                mesh.Vertices.AddVertices(p0);
                mesh.Vertices.AddVertices(p1);
                int n = p0.Count;

                int counter = 0;
                for (int i = 0; i < n - 1; i++)
                {
                    mesh.Faces.AddFace(i, i + 1, i + 1 + n, i + n);
                    mesh.Ngons.AddNgon(MeshNgon.Create(new int[] { i, i + 1, i + 1 + n, i + n }, new int[] { counter++ }));
                }

                if (p0.IsClosed && p1.IsClosed && E)
                {
                    Mesh m0 = Mesh.CreateFromClosedPolyline(p0);
                    Mesh m1 = Mesh.CreateFromClosedPolyline(p1);

                    int fc0 = mesh.Faces.Count;
                    int fc1 = m0.Faces.Count;

                    foreach (MeshFace f in m0.Faces)
                        mesh.Faces.AddFace(f);

                    mesh.Ngons.AddNgon(MeshNgon.Create(Enumerable.Range(0, n - 1).ToList(), Enumerable.Range(fc0, fc1).ToList()));

                    fc0 = mesh.Faces.Count;
                    fc1 = m1.Faces.Count;

                    foreach (MeshFace f in m1.Faces)
                        mesh.Faces.AddFace(f.A + n, f.B + n, f.C + n);

                    mesh.Ngons.AddNgon(MeshNgon.Create(Enumerable.Range(n, n - 1).ToList(), Enumerable.Range(fc0, fc1).ToList()));
                }//if polyline is closed
            }

            if (mesh.IsValid)
            {
                mesh.Clean();
                mesh.Normals.ComputeNormals();
                mesh.UnifyNormals();
                mesh.UnifyNormalsNGons();
                mesh.Weld(0.001);
            }

            return mesh;
        }

        public static Polyline divideByCount(Curve curve, int n, bool close)
        {
            curve.Domain = new Interval(0, 1);

            if (n < 1)
                return null;

            switch (n)
            {
                case (1):
                    if (curve.IsClosed)
                        return new Polyline(new[] { curve.PointAt(0.0), curve.PointAt(0.5) });
                    else
                        return new Polyline(new[] { curve.PointAt(0.0), curve.PointAt(1.0) });

                case (2):
                    if (curve.IsClosed)
                        return new Polyline(new[] { curve.PointAt(0.0), curve.PointAt(0.33333), curve.PointAt(0.66666) });
                    else
                        return new Polyline(new[] { curve.PointAt(0.0), curve.PointAt(0.5), curve.PointAt(1.0) });

                default:

                    Point3d[] pts;
                    curve.DivideByCount(n, true, out pts);

                    Polyline polyline = new Polyline(pts);
                    if (close)
                        polyline.Add(polyline[0]);

                    return polyline;
            }
        }

        public static void CutMesh_FoldedPlates(ref Polyline[][] polylines, List<Plane> P)
        {

            foreach(var plane in P)
                for (int i = 0; i < polylines.Length; i++)
                        polylines[i] = CutMesh(polylines[i].ToList(), plane).ToArray();

    

        }

        public static List<Polyline> CutMesh(List<Polyline> polylines, List<Plane> P)
        {

            var plines = polylines.ToList();

            for (int i = 0; i < P.Count; i++)
            {
                plines = CutMesh(plines, P[i]);
            }
            return plines;

        }

        public static List<Polyline> CutMesh(this List<Polyline> plines3D, Plane P)
        {

            // Rhino.RhinoApp.WriteLine("H");

            double tolerance = Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance;

            Transform xform = Rhino.Geometry.Transform.plane_to_plane(P, Plane.WorldXY);
            Transform xformI = Rhino.Geometry.Transform.plane_to_plane(Plane.WorldXY, P);

            //Transform to XY Plane
            Polyline[] plines = new Polyline[plines3D.Count];


            for (int i = 0; i < plines.Length; i++)
            {
                Polyline pline = new Polyline(plines3D[i]);
                pline.Transform(xform);
                plines[i] = pline;
            }





            List<Polyline> plinesCulled = new List<Polyline>();
            for (int i = 0; i < plines.Length; i++)
            {

                bool flag = true;
                bool[] below = new bool[plines[i].Count];
                int counter = 0;
                for (int j = 0; j < plines[i].Count; j++)
                {
                    if (plines[i][j].Z < 0)
                    {
                        below[j] = true;
                        flag = false;
                        counter++;
                    }
                    else
                    {
                        below[j] = false;
                    }
                }

                //For faces that coincide with plane
                if (counter != 0 && counter != plines[i].Count)
                {



                    Curve curve = plines[i].ToNurbsCurve();
                    Rhino.Geometry.Intersect.CurveIntersections ci = Rhino.Geometry.Intersect.Intersection.CurvePlane(curve, Plane.WorldXY, tolerance * 0.01);

                    List<double> t = new List<double>();

                    for (int j = 0; j < ci.Count; j++)
                    {
                        t.Add(ci[j].ParameterA);
                    }

                    Curve[] curves = curve.Split(t);
                    foreach (Curve c in curves)
                    {
                        if (c.PointAt(0.5 * (c.Domain.T0 + c.Domain.T1)).Z > tolerance)
                        {
                            Polyline splitPline;
                            if (c.TryGetPolyline(out splitPline))
                            {
                                splitPline.Add(splitPline[0]);
                                plinesCulled.Add(splitPline);
                            }
                        }
                    }


                }



                if (flag)
                {

                    plinesCulled.Add(plines[i]);
                }


            }

            for (int i = 0; i < plinesCulled.Count; i++)
            {
                Polyline plineCopy = plinesCulled[i];
                plineCopy.Transform(xformI);
                plinesCulled[i] = plineCopy;
            }

            return plinesCulled;
        }
    }

    public struct TopologyEdge : IComparable<TopologyEdge>
    {
        private int _from;

        private int _to;

        private int _min;

        private int _max;

        private int _right;

        private int _left;

        public int From
        {
            get
            {
                return this._from;
            }
        }

        public bool IsNaked
        {
            get
            {
                bool flag;
                flag = (!(this._left == -1 | this._right == -1) ? false : true);
                return flag;
            }
        }

        public int LeftFace
        {
            get
            {
                return this._left;
            }
        }

        public int NakedSides
        {
            get
            {
                int num = 0;
                if (this._left == -1)
                {
                    num = checked(num + 1);
                }
                if (this._right == -1)
                {
                    num = checked(num + 1);
                }
                return num;
            }
        }

        public int OtherFace(int ThisFace)
        {
            //get{
            int num;

            if (this._left != ThisFace)
                num = (this._right != ThisFace ? -1 : this._left);
            else
                num = this._right;

            return num;
            //}
        }

        public int RightFace
        {
            get
            {
                return this._right;
            }
        }

        public int To
        {
            get
            {
                return this._to;
            }
        }

        public TopologyEdge(int From, int To, int LeftFace = -1, int RightFace = -1)
        {
            this = new TopologyEdge()
            {
                _from = From,
                _to = To,
                _min = Math.Min(From, To),
                _max = Math.Max(From, To),
                _left = LeftFace,
                _right = RightFace
            };
        }

        public int CompareTo(TopologyEdge other)
        {
            int num;
            int num1 = other.Minimum();
            int num2 = this.Minimum();
            if (num2 > num1)
            {
                num = 1;
            }
            else if (num2 != num1)
            {
                num = -1;
            }
            else
            {
                int num3 = other.Maximum();
                int num4 = this.Maximum();
                if (num4 <= num3)
                {
                    num = (num4 != num3 ? -1 : 0);
                }
                else
                {
                    num = 1;
                }
            }
            return num;
        }

        public bool IsValid()
        {
            return !(this.From == -1 | this.To == -1);
        }

        public int Maximum()
        {
            return this._max;
        }

        public int Minimum()
        {
            return this._min;
        }

        public static object operator ==(TopologyEdge A, TopologyEdge B)
        {
            object obj;
            obj = (!(A.Minimum() == B.Minimum() & A.Maximum() == B.Maximum()) ? false : true);
            return obj;
        }

        public static object operator !=(TopologyEdge A, TopologyEdge B)
        {
            return A != B;
        }

        public void Orient()
        {
            int num = Math.Min(this.From, this.To);
            int num1 = Math.Max(this.From, this.To);
            this._from = num;
            this._to = num1;
        }

        public override string ToString()
        {
            return string.Concat(new string[] { "{From:", this.From.ToString(), " To:", this.To.ToString(), "}" });
        }
    }
}
