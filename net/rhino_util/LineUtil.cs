using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class LineUtil {

        public static Line[] SortLines(this Line[] segments) {
            double[] len = new double[segments.Length];
            for (int i = 0; i < segments.Length; i++)
                len[i] = segments[i].Length;
            Array.Sort(len, segments);
            segments = segments.Reverse().ToArray();
            return segments;
        }

        public static Line ExtendLineProportionally(this Line l, double d0 = 0.1, double d1 = 0.0) {
            Point3d p0 = l.From;
            Point3d p1 = l.To;
            return new Line(p0 - l.Direction * d0, p1 + l.Direction * d1);
        }

        public static Line ExtendLine(this Line l, double d) {
            Point3d p0 = l.From;
            Point3d p1 = l.To;

            return new Line(p0 - l.UnitTangent * d, p1 + l.UnitTangent * d);
        }

        public static Line ExtendLine(this Line l, double d0, double d1) {
            Point3d p0 = l.From;
            Point3d p1 = l.To;

            return new Line(p0 - l.UnitTangent * d0, p1 + l.UnitTangent * d1);
        }

        public static Point3d Mid(this Line l) {
            return (l.From + l.To) * 0.5;
        }

        public static Polyline ExtrudeLine(this Line l, Vector3d vector, double d) {
            Vector3d v = new Vector3d(vector);
            v.Unitize();

            if (d < 0) {
                Line l0 = l.XForm(Rhino.Geometry.Transform.Translation(d * v * 0.5));
                Line l1 = l.XForm(Rhino.Geometry.Transform.Translation(-d * v * 0.5));
                return new Polyline { l0.From, l0.To, l1.To, l1.From, l0.From };
            } else {
                Line l0 = l;
                Line l1 = l.XForm(Rhino.Geometry.Transform.Translation(d * v));
                return new Polyline { l0.From, l0.To, l1.To, l1.From, l0.From };
            }

            return l.ToP();
        }

        public static Line XForm(this Line line, Transform xform) {
            Line l = new Line(line.From, line.To);
            l.Transform(xform);
            return l;
        }

        public static Line XForm(this Line line, Vector3d v) {
            Line l = new Line(line.From, line.To);
            l.Transform(Transform.Translation(v));
            return l;
        }

        public static Line GetLargestLineFromCP(Line l, Point3d[] pts) {
            if (l == Line.Unset || pts == null)
                return l;
            if (pts.Length < 2) return l;

            double[] t = new double[pts.Length];

            for (int i = 0; i < pts.Length; i++)
                t[i] = l.ClosestParameter(pts[i]);

            Array.Sort(t);

            return new Line(l.PointAt(t[0]), l.PointAt(t[t.Length - 1]));
        }

        public static Point3d GetLargestPointLineFromCP(Line l, Point3d[] pts) {
            double[] t = new double[pts.Length];

            for (int i = 0; i < pts.Length; i++)
                t[i] = l.ClosestParameter(pts[i]);

            Array.Sort(t);

            return l.PointAt(t[t.Length - 1]);
        }

        public static Point3d GetSmallestPointLineFromCP(Line l, Point3d[] pts) {
            double[] t = new double[pts.Length];

            for (int i = 0; i < pts.Length; i++)
                t[i] = l.ClosestParameter(pts[i]);

            Array.Sort(t);

            return l.PointAt(t[0]);
        }

        public static Tuple<List<Line>, List<Line>, List<Line>, List<double>, List<double>, List<int>, List<int>> LineLine(List<Line> L, bool Bbox, double tolerance) {
            List<Line> lines = new List<Line>();

            HashSet<long> pairs = new HashSet<long>();

            List<int> pairA = new List<int>();
            List<int> pairB = new List<int>();
            List<double> pairA_T = new List<double>();
            List<double> pairB_T = new List<double>();

            List<Line> pairA_L = new List<Line>();
            List<Line> pairB_L = new List<Line>();

            for (int i = 0; i < L.Count; i++) {
                for (int j = 0; j < L.Count; j++) {
                    if (i == j) continue;

                    long key0 = GetKey(i, j);
                    long key1 = GetKey(j, i);

                    if (pairs.Contains(key0)) continue;

                    pairs.Add(key0);
                    pairs.Add(key1);

                    if (Bbox) {
                        BoundingBox bbox0 = L[i].BoundingBox;
                        BoundingBox bbox1 = L[j].BoundingBox;
                        if (!Intersects(bbox0, bbox1))
                            continue;
                    }

                    double t0, t1;
                    if (Rhino.Geometry.Intersect.Intersection.LineLine(L[i], L[j], out t0, out t1, tolerance, true)) {
                        lines.Add(new Line(L[i].PointAt(t0), L[j].PointAt(t1)));
                        pairA_T.Add(t0);
                        pairB_T.Add(t1);
                        pairA.Add(i);
                        pairB.Add(j);
                        pairA_L.Add(L[i]);
                        pairB_L.Add(L[j]);
                    }
                }
            }

            return new Tuple<List<Line>, List<Line>, List<Line>, List<double>, List<double>, List<int>, List<int>>(lines, pairA_L, pairB_L, pairA_T, pairB_T, pairA, pairB);
        }

        public static long GetKey(int i, int j) {
            return (UInt32)i << 16 | (UInt32)j;
        }

        public static Line Rotate(this Line line, double angle, Plane plane) {
            Line l = line;
            l.Transform(Transform.Rotation(Rhino.RhinoMath.ToRadians(angle), plane.ZAxis, plane.Origin));
            return l;
        }

        public static bool Intersects(BoundingBox current, BoundingBox other) {
            return
              (current.Min.X < other.Max.X) && (current.Max.X > other.Min.X) &&
              (current.Min.Y < other.Max.Y) && (current.Max.Y > other.Min.Y) &&
              (current.Min.Z < other.Max.Z) && (current.Max.Z > other.Min.Z);
        }

        public static Point3d P(this Line line, int i) {
            if (i == 0)
                return line.From;
            else if (i == 1)
                return line.To;
            else
                return Point3d.Unset;
        }

        public static void ChangeEnd(ref Line line, int i, Point3d newPoint) {
            if (i == 0)
                line.From = newPoint;
            else if (i == 1)
                line.To = newPoint;
        }

        public static Point3d LineLine(this Line l0, Line l1) {
            double a, b;
            bool flag = Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out a, out b, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance, false);
            Point3d p = !flag ? Point3d.Unset : l0.PointAt(a);

            return p;
        }

        public static Line LineLineProximity(this Line l0, Line l1, out double t0, out double t1) {
            double a, b;
            bool flag = Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out a, out b, 100000000, true);
            Point3d p0 = l0.PointAt(a);
            Point3d p1 = l1.PointAt(b);

            t0 = a;
            t1 = b;
            Line line = new Line(p0, p1);
            return line;
        }

        public static Line LineLineCP(this Line l0, Line l1) {
            double a, b;
            Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out a, out b);
            return new Line(l0.PointAt(a), l1.PointAt(b));
        }

        public static Line LineLineOverlap(Line l0, Line l1) {
            double[] t = new double[4];
            t[0] = 0;
            t[1] = 1;
            t[2] = l0.ClosestParameter(l1.From);
            t[3] = l0.ClosestParameter(l1.To);
            Array.Sort(t);

            Line l = new Line(l0.PointAt(t[1]), l0.PointAt(t[2]));

            return l;
        }

        public static Line LineLineOverlapAverage(Line l0, Line l1) {
            Line lA = LineLineOverlap(l0, l1);
            Line lB = LineLineOverlap(l1, l0);

            Line a = new Line((lA.From + lB.From) * 0.5, (lA.To + lB.To) * 0.5);
            Line b = new Line((lA.From + lB.To) * 0.5, (lA.To + lB.From) * 0.5);

            if (a.Length > b.Length) return a;
            return b;

            //      double d0_lAFrom_lBFrom = lA.From.DistanceToSquared(lB.From);
            //      double d0_lAFrom_lBTo = lA.From.DistanceToSquared(lB.To);
            //      double d0_lATo_lBFrom = lA.To.DistanceToSquared(lB.From);
            //      double d0_lATo_lBTo = lA.To.DistanceToSquared(lB.To);

            //      double[] d = new double[] { d0_lAFrom_lBFrom, d0_lAFrom_lBTo, d0_lATo_lBFrom, d0_lATo_lBTo };
            //      int[] id = new int[] { 0, 1, 2, 3 };
            //      Line[] lines = new Line[] {
            //new Line((lA.From + lB.From) * 0.5, (lA.To + lB.To) * 0.5),
            //new Line((lA.From + lB.To) * 0.5, (lA.To + lB.From) * 0.5),
            //new Line((lA.To + lB.From) * 0.5, (lA.From + lB.To) * 0.5),
            //new Line((lA.To + lB.To) * 0.5, (lA.From + lB.From) * 0.5),

            //};

            //      Array.Sort(d, lines);

            //      return lines[0];
        }

        public static Line LineLineCP(this Line l0, Line l1, out double[] t) {
            t = new double[] { 0, 0 };
            Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out t[0], out t[1]);
            return new Line(l0.PointAt(t[0]), l1.PointAt(t[1]));
        }

        public static double[] LineLineCPT(this Line l0, Line l1) {
            double a, b;
            Rhino.Geometry.Intersect.Intersection.LineLine(l0, l1, out a, out b);
            return new double[] { a, b };
        }

        public static Point3d Center(this Line l) {
            return (l.From + l.To) * 0.5;
        }
    }
}