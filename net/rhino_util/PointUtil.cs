using System;
using System.Collections.Generic;
using System.Linq;
using Rhino.Geometry;

namespace rhino_util {

    public static class PointUtil {

        public static Point3d XForm(this Point3d p_, Transform xform) {
            Point3d p = new Point3d(p_);
            p.Transform(xform);
            return p;
        }

        public static Point3d[] XForm(this Point3d[] a, Transform t) {
            Point3d[] mid = new Point3d[a.Length];
            for (int i = 0; i < a.Length; i++) {
                mid[i] = new Point3d(a[i]);
                mid[i].Transform(t);
            }
            return mid;
        }

        public static Point3d[] TweenPoints(this Point3d[] a, Point3d[] b, double t) {
            Point3d[] mid = new Point3d[a.Length];
            for (int i = 0; i < a.Length; i++) {
                mid[i] = MathUtil.Lerp(a[i], b[i], t);
            }
            return mid;
        }

        //http://csharphelper.com/blog/2014/09/determine-where-two-circles-intersect-in-c/
        //Example double dist = SphereSphereIntersection(p0, r0, p1, r1,   out pIn0, out pIn1, out pMid, out overlap0, out overlap1, false, true);
        public static double SphereSphereIntersection(Vector3d dir, Point3d p0, double r0, Point3d p1, double r1,
        out Point3d intersection1,
        out Point3d intersection2,
        out Point3d overlapCenter,
        out Point3d overlapEnd0,
        out Point3d overlapEnd1,
        out Point3d End0,
        out Point3d End1,
        bool maxOverlap = true, bool cut = true
        ) {
            Point3d p0_ = p0;
            Point3d p1_ = p1;

            Plane plane = (p0.DistanceTo(p1) < (r0 + r1) * 0.5 * 0.01) ? new Plane(new Point3d(p0), dir) : new Plane(new Point3d(p0), p1 - p0);
            //Plane plane =  new Plane(new Point3d(p0), dir) ;
            Transform xform = Transform.plane_to_plane(plane, new Plane(Point3d.Origin, Vector3d.YAxis, Vector3d.ZAxis));
            Transform xformI = Transform.plane_to_plane(new Plane(Point3d.Origin, Vector3d.YAxis, Vector3d.ZAxis), plane);
            p0_.Transform(xform);
            p1_.Transform(xform);

            //Point3d p0_ = Point3d.Origin;
            //Point3d p1_ = new Point3d(p0.DistanceTo(p1), 0, 0);

            Point3f pIn0;
            Point3f pIn1;
            Point3f pMid;
            Point3f overlap0;
            Point3f overlap1;
            float dist = FindCircleCircleIntersections((Point3f)p0_, (float)r0, (Point3f)p1_, (float)r1,
              out pIn0, out pIn1, out pMid, out overlap0, out overlap1, maxOverlap);
            //
            pIn0.Transform(xformI);
            pIn1.Transform(xformI);
            pMid.Transform(xformI);
            overlap0.Transform(xformI);
            overlap1.Transform(xformI);

            intersection1 = (Point3d)pIn0;
            intersection2 = (Point3d)pIn1;
            overlapCenter = (Point3d)pMid;
            overlapEnd1 = (Point3d)overlap0;
            overlapEnd0 = (Point3d)overlap1;

            //get end points
            Vector3d v0 = (overlap0 - p0);
            v0.Unitize();
            Vector3d v1 = (overlap1 - p1);
            v1.Unitize();

            Point3d end0 = p0 + v0 * r0;
            Point3d end1 = p1 + v1 * r1;

            End1 = end1;
            End0 = end0;

            return (double)dist;
            //intersection1_ = (Point3d)
        }

        public static float FindCircleCircleIntersections(
          Point3f p0, float radius0,
          Point3f p1, float radius1,
          out Point3f intersection1,
          out Point3f intersection2,
          out Point3f overlapCenter,
          out Point3f overlapEnd0,
          out Point3f overlapEnd1,
          bool maxOverlap = true, bool cut = true) {
            float cx0 = p0.X;
            float cy0 = p0.Y;

            float cx1 = p1.X;
            float cy1 = p1.Y;

            // Find the distance between the centers.
            float dx = cx0 - cx1;
            float dy = cy0 - cy1;
            double dist = Math.Sqrt(dx * dx + dy * dy);

            // See how many solutions there are.
            if (dist > radius0 + radius1) {
                // No solutions, the circles are too far apart.
                intersection1 = new Point3f(float.NaN, float.NaN, float.NaN);
                intersection2 = new Point3f(float.NaN, float.NaN, float.NaN);
                overlapCenter = new Point3f(float.NaN, float.NaN, float.NaN);
                overlapEnd0 = new Point3f(float.NaN, float.NaN, float.NaN);
                overlapEnd1 = new Point3f(float.NaN, float.NaN, float.NaN);
                return 0;
            } else if (dist < Math.Abs(radius0 - radius1)) {
                // No solutions, one circle contains the other.
                intersection1 = new Point3f(float.NaN, float.NaN, float.NaN);
                intersection2 = new Point3f(float.NaN, float.NaN, float.NaN);

                if (maxOverlap) {
                    if (radius0 > radius1)
                        overlapCenter = new Point3f(cx0, cy0, 0);
                    else
                        overlapCenter = new Point3f(cx1, cy1, 0);
                } else {
                    if (radius0 > radius1)
                        overlapCenter = new Point3f(cx1, cy1, 0);
                    else
                        overlapCenter = new Point3f(cx0, cy0, 0);
                }

                overlapEnd0 = new Point3f(float.NaN, float.NaN, float.NaN);
                overlapEnd1 = new Point3f(float.NaN, float.NaN, float.NaN);

                float result = maxOverlap ? Math.Max(radius0 * 2, radius1 * 2) : Math.Min(radius0 * 2, radius1 * 2);
                if (cut) {
                    if (radius0 > radius1) {
                        overlapEnd0 = new Point3f(overlapCenter.X - result * 0.5f, overlapCenter.Y, 0);
                        overlapEnd1 = new Point3f(Math.Max(radius0, radius1), overlapCenter.Y, 0);
                    } else {
                        float diff = Math.Max(radius0, radius1) - Math.Min(radius0, radius1);
                        overlapEnd0 = new Point3f(cx1 - Math.Max(radius0, radius1), overlapCenter.Y, 0);
                        overlapEnd1 = new Point3f(overlapCenter.X + result * 0.5f, overlapCenter.Y, 0);
                    }
                }

                return -result;
            } else if ((dist == 0) && (radius0 == radius1)) {
                // No solutions, the circles coincide.
                intersection1 = new Point3f(float.NaN, float.NaN, float.NaN);
                intersection2 = new Point3f(float.NaN, float.NaN, float.NaN);
                overlapCenter = new Point3f(cx0, cy0, 0);

                overlapEnd0 = new Point3f(cx0 - radius0, cy0, 0);
                overlapEnd1 = new Point3f(cx0 + radius1, cy0, 0);
                return 0;             //radius0 * 2;
            } else {
                // Find a and h.
                double a = (radius0 * radius0 -
                  radius1 * radius1 + dist * dist) / (2 * dist);
                double h = Math.Sqrt(radius0 * radius0 - a * a);

                // Find P2.
                double cx2 = cx0 + a * (cx1 - cx0) / dist;
                double cy2 = cy0 + a * (cy1 - cy0) / dist;

                // Get the points P3.
                intersection1 = new Point3f(
                  (float)(cx2 + h * (cy1 - cy0) / dist),
                  (float)(cy2 - h * (cx1 - cx0) / dist),
                  0);
                intersection2 = new Point3f(
                  (float)(cx2 - h * (cy1 - cy0) / dist),
                  (float)(cy2 + h * (cx1 - cx0) / dist),
                  0);

                //Get center and ovelap distnace
                Point3f midP = new Point3f((p0.X + p1.X) * 0.5f, (p0.Y + p1.Y) * 0.5f, (p0.Z + p1.Z) * 0.5f);
                Vector3f v0 = midP - p0;
                v0.Unitize();
                v0 *= radius0;

                Vector3f v1 = midP - p1;
                v1.Unitize();
                v1 *= radius1;

                midP += new Vector3f((v0.X + v1.X) * 0.5f, (v0.Y + v1.Y) * 0.5f, (v0.Z + v1.Z) * 0.5f);
                ;
                overlapCenter = midP;
                float overlapDist = Math.Abs((float)p0.DistanceTo(p1) - radius0 - radius1);

                v0.Unitize();
                v1.Unitize();

                overlapEnd0 = Vector3f.Add(overlapCenter, -Vector3f.Multiply(v0, overlapDist * 0.5f));
                overlapEnd1 = Vector3f.Add(overlapCenter, -Vector3f.Multiply(v1, overlapDist * 0.5f));

                //overlapEnd0 = Vector3f.Add(midP, -  Vector3f.Multiply(v0,(2.0f * radius0 - overlapDist)));
                //overlapEnd1 = Vector3f.Add(midP, - Vector3f.Multiply(v1,(2.0f * radius1 - overlapDist)));
                return overlapDist;

                // See if we have 1 or 2 solutions.
                //if (dist == radius0 + radius1) return 1;
                //return 2;
            }
        }

        public static Plane[] InterpolatePlanes(Plane plane0, Plane plane1, int Steps, bool includeEnds = true) {
            Point3d[] origins = InterpolatePoints(plane0.Origin, plane1.Origin, Steps, includeEnds);
            Point3d[] XAxes = InterpolatePoints((Point3d)plane0.XAxis, (Point3d)plane1.XAxis, Steps, includeEnds);
            Point3d[] YAxes = InterpolatePoints((Point3d)plane0.YAxis, (Point3d)plane1.YAxis, Steps, includeEnds);

            Plane[] planes = new Plane[origins.Length];
            for (int i = 0; i < planes.Length; i++) {
                planes[i] = new Plane(origins[i], (Vector3d)XAxes[i], (Vector3d)YAxes[i]);
            }

            return planes;
        }

        public static List<Point3d> IsPointsCloseToCurve(this Curve C, List<Point3d> P, ref List<int> ID, double T = 0.01) {
            List<Point3d> cp = new List<Point3d>();
            List<int> cpID = new List<int>();
            double tolSQ = T * T;

            BoundingBox bbox = C.GetBoundingBox(false);
            bbox.Inflate(bbox.Diagonal.Length * 0.01);
            double t;
            for (int i = 0; i < P.Count; i++) {
                //if (bbox.Contains(P[i])) {
                if (!C.IsClosed) {
                    C.ClosestPoint(P[i], out t);
                    Point3d p = C.PointAt(t);
                    if (p.DistanceToSquared(P[i]) < tolSQ) {
                        cp.Add(P[i]);
                        cpID.Add(i);
                    }
                } else {
                    C.ClosestPoint(P[i], out t);
                    C.TryGetPlane(out Plane plane);
                    var containment = C.ToNurbsCurve().Contains(P[i], plane, T);

                    if (containment == PointContainment.Inside || containment == PointContainment.Coincident) {
                        cp.Add(P[i]);
                        cpID.Add(i);
                    }
                }

                //}
            }

            ID = cpID;
            return cp;
        }

        public static List<Point3d> IsPointsInsideMesh(this Mesh M, List<Point3d> P, ref List<int> ID, double T = 0.01) {
            List<Point3d> cp = new List<Point3d>();
            List<int> cpID = new List<int>();
            double tolSQ = T * T;

            Mesh mesh = M.DuplicateMesh();
            if (mesh.SolidOrientation() == -1)
                mesh.Flip(true, true, true);

            BoundingBox bbox = M.GetBoundingBox(false);

            bbox.Inflate(bbox.Diagonal.Length * 0.01);

            for (int i = 0; i < P.Count; i++) {
                if (mesh.IsClosed) {
                    if (mesh.IsPointInside(P[i], T, false)) {
                        cp.Add(P[i]);
                        cpID.Add(i);
                    }
                } else {
                    Point3d p = mesh.ClosestPoint(P[i]);
                    if (p.DistanceToSquared(P[i]) < tolSQ) {
                        cp.Add(P[i]);
                        cpID.Add(i);
                    }
                }
            }
            ID = cpID;
            return cp;
        }

        public static List<Curve> IsCurvesInsideMesh(this Mesh M, List<Curve> C, ref List<int> ID, double T = 0.01, bool center = true) {
            List<Curve> cp = new List<Curve>();
            List<int> cpID = new List<int>();
            double tolSQ = T * T;

            Mesh mesh = M.DuplicateMesh();
            if (mesh.SolidOrientation() == -1)
                mesh.Flip(true, true, true);

            BoundingBox bbox = M.GetBoundingBox(false);

            bbox.Inflate(bbox.Diagonal.Length * 0.01);

            for (int i = 0; i < C.Count; i++) {
                Polyline pline;
                bool flag = C[i].TryGetPolyline(out pline);

                //if (flag) {
                List<Point3d> P = center ? new List<Point3d> { pline.CenterPoint() } : pline.ToList();

                for (int j = 0; j < P.Count; j++) {
                    if (mesh.IsClosed) {
                        if (mesh.IsPointInside(P[j], T, false)) {
                            cp.Add(C[i]);
                            cpID.Add(i);
                            break;
                        }
                    } else {
                        Point3d p = mesh.ClosestPoint(P[j]);
                        if (p.DistanceToSquared(P[j]) < tolSQ) {
                            cp.Add(C[i]);
                            cpID.Add(i);
                            break;
                        }
                    }
                }

                // }
            }

            ID = cpID;
            return cp;
        }

        public static Vector3f ToVector3f(this Point3f p) {
            Vector3f v = new Vector3f(p.X, p.Y, p.Z);
            return v;
        }

        public static Vector3d ToVector3d(this Point3d p) {
            Vector3d v = new Vector3d(p.X, p.Y, p.Z);
            return v;
        }

        public static Point3f ToPoint3f(this Vector3f p) {
            Point3f v = new Point3f(p.X, p.Y, p.Z);
            return v;
        }

        public static Point3d ToPoint3d(this Vector3d p) {
            Point3d v = new Point3d(p.X, p.Y, p.Z);
            return v;
        }

        public static Point3f Unit(this Point3f p) {
            Vector3f v = new Vector3f(p.X, p.Y, p.Z);
            v.Unitize();
            return new Point3f(v.X, v.Y, v.Z);
        }

        public static Point3d Unit(this Point3d p) {
            Vector3d v = new Vector3d(p.X, p.Y, p.Z);
            v.Unitize();
            return new Point3d(v.X, v.Y, v.Z);
        }

        public static Vector3f Unit(this Vector3f p) {
            Vector3f v = new Vector3f(p.X, p.Y, p.Z);
            v.Unitize();
            return new Vector3f(v.X, v.Y, v.Z);
        }

        public static Vector3d Unit(this Vector3d p) {
            Vector3d v = new Vector3d(p.X, p.Y, p.Z);
            v.Unitize();
            return new Vector3d(v.X, v.Y, v.Z);
        }

        public static int ClosestPoint(Point3d P, Point3d[] list) {
            int num = -1;
            double num1 = double.MaxValue;

            for (int i = 0; i < list.Length; i++) {
                if (list[i] != null && list[i].IsValid) {
                    double num2 = P.DistanceToSquared(list[i]);
                    if (num2 < num1) {
                        num1 = num2;
                        num = i;
                    }
                }
            }
            return num;
        }

        public static List<int> ClosestPoints(Point3d P, Point3d[] list, int num) {
            checked {
                int[] array = new int[num - 1 + 1];
                double[] array2 = new double[num - 1 + 1];
                int arg_74_0 = 0;
                int num2 = array.Length - 1;
                for (int i = arg_74_0; i <= num2; i++) {
                    array[i] = -1;
                    array2[i] = 1.7976931348623157E+308;
                }
                int arg_A1_0 = 0;
                int num3 = list.Length - 1;
                for (int j = arg_A1_0; j <= num3; j++) {
                    if (list[j] != null) {
                        if (list[j].IsValid) {
                            double num4 = P.DistanceTo(list[j]);
                            int num5 = System.Array.BinarySearch<double>(array2, num4);
                            if (num5 < 0) {
                                num5 ^= -1;
                            }
                            if (num5 < array.Length) {
                                int arg_102_0 = array.Length - 1;
                                int num6 = num5 + 1;
                                for (int k = arg_102_0; k >= num6; k += -1) {
                                    array[k] = array[k - 1];
                                    array2[k] = array2[k - 1];
                                }
                                array[num5] = j;
                                array2[num5] = num4;
                            }
                        }
                    }
                }

                List<int> list3 = new List<int>(num);
                List<double> dist = new List<double>();

                int arg_161_0 = 0;
                int num7 = array.Length - 1;
                int num8 = arg_161_0;
                while (num8 <= num7 && array[num8] >= 0) {
                    list3.Add(array[num8]);
                    dist.Add(P.DistanceTo(list[array[num8]]));
                    num8++;
                }

                return list3;
            }
        }

        public static Point3d Barycentric(Point3d P, Point3d A, Point3d B, Point3d C) {
            Vector3d b = B - A;
            Vector3d c = C - A;
            Vector3d p = P - A;
            double num = DotProduct(b, b);
            double num1 = DotProduct(b, c);
            double num2 = DotProduct(c, c);
            double num3 = DotProduct(p, b);
            double num4 = DotProduct(p, c);
            double num5 = 1 / (num * num2 - num1 * num1);
            double num6 = (num2 * num3 - num1 * num4) * num5;
            double num7 = (num * num4 - num1 * num3) * num5;
            return new Point3d(num6, num7, 1 - num6 - num7);
        }

        public static Circle InscribedCircle(List<Point3d> Triangle) {
            Circle circle = new Circle();
            Point3d item = Triangle[0];
            double num = item.DistanceTo(Triangle[1]);
            item = Triangle[1];
            double num1 = item.DistanceTo(Triangle[2]);
            item = Triangle[2];
            double num2 = item.DistanceTo(Triangle[0]);
            double num3 = (num + num1 + num2) / 2;
            double num4 = Math.Sqrt(num3 * (num3 - num) * (num3 - num1) * (num3 - num2));
            double num5 = 2 * num4 / (num + num1 + num2);
            circle.Radius = (num5);
            double num6 = Vector3d.VectorAngle(Triangle[1] - Triangle[0], Triangle[2] - Triangle[0]) / 2;
            double num7 = 1 / Math.Sin(num6) * num5;
            Vector3d vector3d = Triangle[1] - Triangle[0];
            Vector3d item1 = Triangle[2] - Triangle[0];
            vector3d.Unitize();
            item1.Unitize();
            Vector3d vector3d1 = vector3d + item1;
            vector3d1.Unitize();
            vector3d1 *= num7;
            Point3d point3d = Triangle[0] + vector3d1;
            Vector3d item2 = Triangle[2] - Triangle[1];
            Vector3d vector3d2 = Triangle[1] - Triangle[0];
            item2.Unitize();
            vector3d2.Unitize();
            Vector3d vector3d3 = Vector3d.CrossProduct(item2, vector3d2);
            circle.Plane = (new Plane(point3d, vector3d3));
            return circle;
        }

        public static Point3d[][] InterpolatePointArrays(Point3d[] From, Point3d[] To, int Steps) {
            Point3d[][] point3dArray = new Point3d[checked(checked(Steps + 1) + 1)][];
            int length = checked((int)point3dArray.Length - 1);
            for (int i = 0; i <= length; i = checked(i + 1)) {
                double num = (double)i / (double)(checked((int)point3dArray.Length - 1));
                Point3d[] point3dArray1 = new Point3d[checked(checked((int)From.Length - 1) + 1)];
                int length1 = checked((int)From.Length - 1);
                for (int j = 0; j <= length1; j = checked(j + 1)) {
                    Point3d from = From[j];
                    Point3d to = To[j];
                    from *= num;
                    to = to * (1 - num);
                    point3dArray1[j] = from + to;
                }
                point3dArray[i] = point3dArray1;
            }
            return point3dArray;
        }

        public static Vector3d SumVector3d(IEnumerable<Vector3d> Vectors) {
            Vector3d vector3d = new Vector3d();
            int num = checked(Vectors.Count<Vector3d>() - 1);
            for (int i = 0; i <= num; i = checked(i + 1)) {
                vector3d += Vectors.ElementAtOrDefault<Vector3d>(i);
            }
            return vector3d;
        }

        public static Line TangentCircle(double R, Line L1, Line L2) {
            Vector3d to = L1.To - L1.From;
            Vector3d vector3d = L2.To - L2.From;
            to.Unitize();
            vector3d.Unitize();
            Vector3d vector3d1 = new Vector3d((to + vector3d) * 0.5);
            double num = Vector3d.VectorAngle(L1.To - L1.From, L2.To - L2.From) * 0.5;
            double r = R * MathUtil.Cot(num);
            return new Line(L1.From, vector3d1, MathUtil.Pitagoras(r, R));
        }

        internal static object Walk(int Start, List<int>[] Neighbors) {
            int start = Start;
            int num = Start;
            int item = Neighbors[num][0];
            List<int> nums = new List<int>()
            {
                num,
                item
            };
            do {
                if (Neighbors[item].Count != 1) {
                    int item1 = Neighbors[item][0];
                    int num1 = Neighbors[item][1];
                    if (item1 != num) {
                        num = item;
                        item = item1;
                    } else if (num1 != num) {
                        num = item;
                        item = num1;
                    }
                    nums.Add(item);
                } else {
                    if (num != start) {
                        break;
                    }
                    nums.Add(Neighbors[item][0]);
                    break;
                }
            }
            while (item != start);
            return nums;
        }

        public static double DotProduct(Vector3d a, Vector3d b) {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        public static void PlanarizePoint(this Point3d A0, Plane newplane) {
            Line line = new Line(A0, A0 + newplane.Normal * 10);
            double param = new double();
            Rhino.Geometry.Intersect.Intersection.LinePlane(line, newplane, out param);
            Point3d A0p = line.PointAt(param);
            A0 = A0p;
        }

        public static Point3d PlanarPoint(this Point3d A0, Plane newplane) {
            Line line = new Line(A0, A0 + newplane.Normal * 10);
            double param = new double();
            Rhino.Geometry.Intersect.Intersection.LinePlane(line, newplane, out param);
            Point3d A0p = line.PointAt(param);
            return A0p;
        }

        public static Line[] ToLineArray(this Point3d[] p, bool closed = false) {
            Line[] lines;

            if (!closed) {
                lines = new Line[p.Length - 1];
                for (int i = 0; i < p.Length - 1; i++)
                    lines[i] = new Line(p[i], p[i + 1]);
                return lines;
            } else {
                lines = new Line[p.Length];
                for (int i = 0; i < p.Length - 1; i++)
                    lines[i] = new Line(p[i], p[i + 1]);
                lines[p.Length - 1] = new Line(p[p.Length - 1], p[0]);
                return lines;
            }
        }

        public static double FastDistance(this Point3d p1, Point3d p2) {
            return (p2.X - p1.X) * (p2.X - p1.X) + (p2.Y - p1.Y) * (p2.Y - p1.Y) + (p2.Z - p1.Z) * (p2.Z - p1.Z);
        }

        public static Point3d AveragePoint(IEnumerable<Point3d> P) {
            Point3d point3d = new Point3d(0.0, 0.0, 0.0);
            foreach (Point3d pt in P) {
                point3d += pt;
            }
            return point3d / (double)P.Count();
        }

        public static Point3d[] InterpolatePoints(Point3d from, Point3d to, int Steps, bool includeEnds = true) {
            Point3d[] point3DArray;

            if (includeEnds) {
                point3DArray = new Point3d[Steps + 2];
                point3DArray[0] = from;

                for (int i = 1; i < Steps + 1; i++) {
                    double num = i / (double)(1 + Steps);

                    point3DArray[i] = new Point3d(
                        MathUtil.Lerp(from.X, to.X, num),
                        MathUtil.Lerp(from.Y, to.Y, num),
                        MathUtil.Lerp(from.Z, to.Z, num)
                    );
                }

                point3DArray[point3DArray.Length - 1] = to;
            } else {
                point3DArray = new Point3d[Steps];

                for (int i = 1; i < Steps + 1; i++) {
                    double num = i / (double)(1 + Steps);

                    point3DArray[i - 1] = new Point3d(
                        MathUtil.Lerp(from.X, to.X, num),
                        MathUtil.Lerp(from.Y, to.Y, num),
                        MathUtil.Lerp(from.Z, to.Z, num)
                    );
                }
            }

            return point3DArray;
        }

        public static Point3d[] InterpolateLine(this Line line, int Steps, bool includeEnds = true) {
            return InterpolatePoints(line.From, line.To, Steps, includeEnds);
        }

        public static Polyline[] InterpolatePolylines(Polyline from, Polyline to, int Steps, bool includeEnds = true) {
            if (Steps == 0)
                return new Polyline[] { from, to };

            if (from.Count == to.Count) {
                Polyline[] interpolatedPolylines = new Polyline[Steps + (Convert.ToInt32(includeEnds) * 2)];

                for (int i = 0; i < Steps + (Convert.ToInt32(includeEnds) * 2); i++)
                    interpolatedPolylines[i] = new Polyline();

                for (int i = 0; i < from.Count; i++) {
                    Point3d[] pt = PointUtil.InterpolatePoints(from[i], to[i], Steps, includeEnds);
                    for (int j = 0; j < Steps + (Convert.ToInt32(includeEnds) * 2); j++)
                        interpolatedPolylines[j].Add(pt[j]);
                }

                return interpolatedPolylines;
            }

            return null;
        }

        public static List<int> cp(Point3d P, Point3d[] list, int num) {
            checked {
                int[] array = new int[num - 1 + 1];
                double[] array2 = new double[num - 1 + 1];
                int arg_74_0 = 0;
                int num2 = array.Length - 1;
                for (int i = arg_74_0; i <= num2; i++) {
                    array[i] = -1;
                    array2[i] = 1.7976931348623157E+308;
                }
                int arg_A1_0 = 0;
                int num3 = list.Length - 1;
                for (int j = arg_A1_0; j <= num3; j++) {
                    if (list[j] != null) {
                        if (list[j].IsValid) {
                            double num4 = P.DistanceTo(list[j]);
                            int num5 = System.Array.BinarySearch<double>(array2, num4);
                            if (num5 < 0) {
                                num5 ^= -1;
                            }
                            if (num5 < array.Length) {
                                int arg_102_0 = array.Length - 1;
                                int num6 = num5 + 1;
                                for (int k = arg_102_0; k >= num6; k += -1) {
                                    array[k] = array[k - 1];
                                    array2[k] = array2[k - 1];
                                }
                                array[num5] = j;
                                array2[num5] = num4;
                            }
                        }
                    }
                }

                List<int> list3 = new List<int>(num);
                List<double> dist = new List<double>();

                int arg_161_0 = 0;
                int num7 = array.Length - 1;
                int num8 = arg_161_0;
                while (num8 <= num7 && array[num8] >= 0) {
                    list3.Add(array[num8]);
                    dist.Add(P.DistanceTo(list[array[num8]]));
                    num8++;
                }

                return list3;
            }
        }
    }
}