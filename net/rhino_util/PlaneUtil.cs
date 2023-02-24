using System;
using System.Collections.Generic;
using System.Linq;
using Rhino.Geometry;

namespace rhino_util {

    public static class PlaneUtil {

        public static Plane[] PointsToPlanes(this Point3d[] pts, Plane plane) {
            Plane[] planes = new Plane[pts.Length];

            for (int k = 0; k < pts.Length; k++) {
                planes[k] = plane.ChangeOrigin(pts[k]);
            }

            return planes;
        }

        //https://www.grasshopper3d.com/forum/topics/convex-or-concave-angle-between-faces?id=2985220%3ATopic%3A954247&page=1#comments
        public static double AngleBetweenTwoFaces(Plane plane0, Plane plane1, Point3d edgeMiddlePoint) {
            if (plane0.ZAxis.IsParallelTo(plane1.ZAxis, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance) == -1)
                return 0;

            if (plane0.ZAxis.IsParallelTo(plane1.ZAxis, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance) == 1)
                return 180;

            double num = Math.PI - Vector3d.VectorAngle(plane0.ZAxis, plane1.ZAxis, new Plane(edgeMiddlePoint, plane0.ZAxis, plane1.ZAxis));

            //Solve concave / convex cases

            Vector3d v = plane1.Origin - plane0.Origin;
            double angleBetweenMidPoints = Vector3d.VectorAngle(plane0.ZAxis, v, new Plane(edgeMiddlePoint, plane0.ZAxis, v));

            if (angleBetweenMidPoints > Math.PI * 0.5)
                num = Math.PI * 2 - num;

            num = Math.Round(Rhino.RhinoMath.ToDegrees(num), 3);
            return num;
        }

        public static double DihedralAngle(this Plane pl0_0, Plane pl1_0) {
            //Calculate angle by line intersection
            double dihedralAngle = 0;
            Line angleLine0 = pl0_0.ZAxis.ToLine(pl0_0.Origin);
            Line angleLine1 = pl1_0.ZAxis.ToLine(pl1_0.Origin);

            //pl0_0.BakeAxes(20);
            //pl1_0.BakeAxes(20);

            if (pl0_0.ZAxis.IsParallelTo(pl1_0.ZAxis) == 0) {
                Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1, out double t0, out double t1);
                Point3d center = angleLine0.PointAt(t0);
                Vector3d v0 = (pl0_0.Origin - center).Unit();
                Vector3d v1 = (pl1_0.Origin - center).Unit();
                Plane dihedralPlane = new Plane(center, v0, v1);
                //dihedralPlane.BakeAxes(20);
                dihedralAngle = Vector3d.VectorAngle(v0, v1, dihedralPlane);
            }

            return Rhino.RhinoMath.ToDegrees(dihedralAngle);
        }

        public static bool IsSamePosition(this Plane pl0, Plane pl1, double tolerance = 0.0001) {
            return pl0.ClosestPoint(pl1.Origin).DistanceToSquared(pl1.Origin) < tolerance && pl1.ClosestPoint(pl0.Origin).DistanceToSquared(pl0.Origin) < tolerance;
        }

        public static bool IsSameDirection(this Plane pl0, Plane pl1, bool canBeFlipped = true, double tolerance = 0.0001) {
            if (canBeFlipped) {
                return pl0.ZAxis.IsParallelTo(pl1.ZAxis, tolerance) != 0;
            } else {
                return pl0.ZAxis.IsParallelTo(pl1.ZAxis, tolerance) == 1;
            }
        }

        public static Boolean IsCoplanar(this Plane pl0, Plane pl1, bool canBeFlipped = true, double tolerance = 0.0001) {
            //Rhino.RhinoApp.WriteLine(IsSameDirection(pl0, pl1, canBeFlipped, 1).ToString() + " " + IsSamePosition(pl0, pl1, tolerance).ToString())  ;

            return IsSameDirection(pl0, pl1, canBeFlipped, tolerance) && IsSamePosition(pl0, pl1, tolerance);
        }

        public static int IsAbovePlane(this Plane plane, Point3d p) {
            Point3d pCopy = new Point3d(p);
            pCopy.Transform(Rhino.Geometry.Transform.plane_to_plane(plane, Plane.WorldXY));
            if (pCopy.Z == 0) return 0;
            else if (pCopy.Z < 0) return -1;
            else return 1;
        }

        public static Plane CrossProductPlane(Plane plane0, Plane plane1) {
            if (plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) == 0) {
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Plane between two Planes
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Rhino.Geometry.Intersect.Intersection.PlanePlane(plane0, plane1, out Line l);
                Rhino.Geometry.Intersect.Intersection.LinePlane(l, plane0, out double t);
                Plane plane = new Plane(l.PointAt(t), l.Direction);
                return plane;
            } else return Plane.Unset;
        }

        public static Plane DihedralPlane(Plane plane0, Plane plane1) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Plane between two Planes
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            Rhino.Geometry.Intersect.Intersection.PlanePlane(plane0, plane1, out Line l);
            Rhino.Geometry.Intersect.Intersection.LinePlane(l, plane0, out double t);
            Point3d centerDihedral = l.PointAt(t);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Convert ZAxis to lines, cant it be done on origin
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            Line angleLine0 = plane0.ZAxis.ToLine(plane0.Origin);
            Line angleLine1 = plane1.ZAxis.ToLine(plane1.Origin);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Plane cannot be anti/parallel and they must be apart from each other
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) == 0 && plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001) {
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Intersect two lines to get center
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1, out double t0, out double t1);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Construct plane from LineLine center and plane origins
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Point3d center = angleLine0.PointAt(t0);
                Vector3d v0 = (plane0.Origin - center).Unit();
                Vector3d v1 = (plane1.Origin - center).Unit();

                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Compute Plane
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Plane dihedralPlane = new Plane(centerDihedral, l.Direction, v0 + v1);

                return dihedralPlane;
            } else
                return Plane.Unset;//Axes are paralell or anti-parallel |.| or _._ or planes are at the same location
        }

        public static double SmallerDihedralAngleDeg(Plane plane0, Plane plane1) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Convert ZAxis to lines, cant it be done on origin
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            Line angleLine0 = plane0.ZAxis.ToLine(plane0.Origin);
            Line angleLine1 = plane1.ZAxis.ToLine(plane1.Origin);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Plane cannot be anti/parallel and they must be apart from each other
            ////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) == 0 && plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001) {
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Intersect two lines to get center
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1, out double t0, out double t1);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Construct plane from LineLine center and plane origins
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                Point3d center = angleLine0.PointAt(t0);
                Vector3d v0 = (plane0.Origin - center).Unit();
                Vector3d v1 = (plane1.Origin - center).Unit();
                Plane dihedralPlane = new Plane(center, v0, v1);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Compute angle
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                double dihedralAngle = Rhino.RhinoMath.ToDegrees(Vector3d.VectorAngle(v0, v1, dihedralPlane));

                return dihedralAngle;
            } else
                return 0;//Axes are paralell or anti-parallel |.| or _._ or planes are at the same location
        }

        public static Line ToLine(this Plane plane, string XYZ = "Z", double scale = 1) {
            switch (XYZ) {
                case ("X"):
                    return new Line(plane.Origin, plane.Origin + plane.YAxis * scale);

                case ("Y"):
                    return new Line(plane.Origin, plane.Origin + plane.XAxis * scale);

                default:
                    return new Line(plane.Origin, plane.Origin + plane.ZAxis * scale);
            }
        }

        public static Plane ToPlane(this Line l, bool startorend) {
            if (startorend)
                return new Plane(l.From, l.Direction);
            else
                return new Plane(l.To, l.Direction);
        }

        public static Plane Translation(this Plane plane, Vector3d v) {
            Plane p = new Plane(plane);
            p.Translate(v);
            return p;
        }

        public static Polyline ToPolyline(this List<Plane> planes) {
            Polyline pline = new Polyline();
            foreach (var pl in planes)
                pline.Add(pl.Origin);
            return pline;
        }

        public static Plane ChangeOriginCoord(this Plane plane, double val, string coord = "Z") {
            Plane p = new Plane(plane);

            switch (coord) {
                case ("X"):
                case ("x"):
                    p.Origin = new Point3d(val, p.Origin.Y, p.Origin.Z);
                    break;

                case ("Y"):
                case ("y"):
                    p.Origin = new Point3d(p.Origin.X, val, p.Origin.Z);
                    break;

                case ("Z"):
                case ("z"):
                    p.Origin = new Point3d(p.Origin.X, p.Origin.Y, val);
                    break;
            }

            return p;
        }

        public static Plane XForm(this Plane plane, Transform t) {
            Plane planeCopy = new Plane(plane);
            planeCopy.Transform(t);
            return planeCopy;
        }

        public static Tuple<Plane, Plane> GetSidePlanes(this Curve centreCurve, List<Tuple<double, Plane>> frames, List<double> radii, double t, bool XZ_YZ = true) {
            Plane plane = centreCurve.GetPlane(frames, t);

            //centreCurve.GetPlane(t);
            double tRemapped = centreCurve.TXAB(t);
            double r = MathUtil.Interpolate(radii, tRemapped);
            string flag = XZ_YZ ? "XZ" : "YZ";
            return new Tuple<Plane, Plane>(plane.Switch(flag).MovePlanebyAxis(r), plane.Switch(flag).MovePlanebyAxis(-r));
            //return new Tuple<Plane, Plane>(plane, plane);
        }

        public static void RecalculateFrames(this Curve centreCurve, ref List<Tuple<double, Plane>> frames) {
            Point3d ptCrv;
            Vector3d tan, xAxis, yAxis;

            for (int i = 0; i < frames.Count; i++) {
                ptCrv = centreCurve.PointAt(frames[i].Item1);
                tan = centreCurve.TangentAt(frames[i].Item1);
                xAxis = Vector3d.CrossProduct(frames[i].Item2.YAxis, tan);
                yAxis = Vector3d.CrossProduct(tan, xAxis);
                frames[i] = new Tuple<double, Plane>(frames[i].Item1, new Plane(ptCrv, xAxis, yAxis));

                //Rhino.RhinoApp.WriteLine(frames[i].Item1.ToString());
                //Line line = new Line(ptCrv, ptCrv+ xAxis*200);
                //line.Bake();
            }
            //Rhino.RhinoApp.WriteLine();
        }

        public static Plane GetPlane(this Curve centerCurve, List<Tuple<double, Plane>> frames, Point3d p) {
            centerCurve.ClosestPoint(p, out double t);

            Tuple<Plane, Plane, double> faround = FramesAround(centerCurve, frames, t);
            Plane plane;

            plane = InterpolateUtil.InterpolatePlanes(faround.Item1, faround.Item2, faround.Item3);
            plane.Origin = centerCurve.PointAt(t);
            plane.Transform(Rhino.Geometry.Transform.Rotation(plane.ZAxis, centerCurve.TangentAt(t), plane.Origin));

            return plane;
        }

        //this is the main method for beam class to get the plane
        public static Plane GetPlane(this Curve centerCurve, List<Tuple<double, Plane>> frames, double t) {
            Tuple<Plane, Plane, double> faround = FramesAround(centerCurve, frames, t);
            Plane plane;

            plane = InterpolateUtil.InterpolatePlanes(faround.Item1, faround.Item2, faround.Item3);
            plane.Origin = centerCurve.PointAt(t);
            plane.Transform(Rhino.Geometry.Transform.Rotation(plane.ZAxis, centerCurve.TangentAt(t), plane.Origin));

            return plane;
        }

        public static Tuple<Plane, Plane, double> FramesAround(this Curve centerCurve, List<Tuple<double, Plane>> frames, double t) {
            if (frames.Count < 1)
                return null;

            double tt = 0;
            int index = 1;
            int last = frames.Count - 1;

            //if frames are less than start and end
            if (t <= frames[0].Item1)
                return new Tuple<Plane, Plane, double>(frames[0].Item2, frames[0].Item2, 0.0);
            else if (t > frames[last].Item1) {
                return new Tuple<Plane, Plane, double>(frames[last].Item2, frames[last].Item2, 1.0);
            }

            //iterate planes to find the closest to t
            for (int i = 1; i < frames.Count; ++i) {
                if (frames[i].Item1 < t)
                    continue;
                else {
                    index = i;
                    break;
                }
            }
            tt = (t - frames[index - 1].Item1) / (frames[index].Item1 - frames[index - 1].Item1);
            return new Tuple<Plane, Plane, double>(frames[index - 1].Item2, frames[index].Item2, tt);
        }

        public static Rhino.Geometry.Plane ProjectPlaneXPlaneToYPlane(Rhino.Geometry.Plane x, Rhino.Geometry.Plane y) {
            Line l0 = new Line(x.Origin, x.Origin + x.ZAxis);
            Line l1 = new Line(x.Origin + x.XAxis, x.Origin + x.XAxis + x.ZAxis);
            Line l2 = new Line(x.Origin + x.YAxis, x.Origin + x.YAxis + x.ZAxis);

            double t0, t1, t2;
            Rhino.Geometry.Intersect.Intersection.LinePlane(l0, y, out t0);
            Rhino.Geometry.Intersect.Intersection.LinePlane(l1, y, out t1);
            Rhino.Geometry.Intersect.Intersection.LinePlane(l2, y, out t2);

            Point3d p0 = l0.PointAt(t0);
            Point3d p1 = l1.PointAt(t1);
            Point3d p2 = l2.PointAt(t2);
            Vector3d xaxis = p1 - p0;
            Vector3d yaxis = p2 - p0;

            Plane projectedPlane = new Plane(y.Origin, xaxis, yaxis);

            return projectedPlane;
        }

        public static Plane Switch(this Plane plane, string flag = "XY") {
            switch (flag) {
                case ("XZ"):
                    return new Plane(plane.Origin, plane.XAxis, plane.ZAxis);

                case ("YX"):
                    return new Plane(plane.Origin, plane.YAxis, plane.XAxis);

                case ("YZ"):
                    return new Plane(plane.Origin, plane.YAxis, plane.ZAxis);

                case ("ZX"):
                    return new Plane(plane.Origin, plane.ZAxis, plane.XAxis);

                case ("ZY"):
                    return new Plane(plane.Origin, plane.ZAxis, plane.YAxis);

                default:
                    return new Plane(plane);
            }
        }

        public static Plane FlipAndRotate(this Plane p) {
            Plane plane = new Plane(p);
            plane.Flip();
            plane.Rotate(Math.PI * 0.5, plane.ZAxis);
            return plane;
        }

        public static List<Plane> Transform(this List<Plane> planes, Transform t) {
            List<Plane> planesTransformed = new List<Plane>();
            foreach (Plane p in planes) {
                Plane pTransformed = new Plane(p);
                pTransformed.Transform(t);
                planesTransformed.Add(pTransformed);
            }
            return planesTransformed;
        }

        /// <summary>
        /// Creates a plane from a line and a plane
        /// </summary>
        /// <param name="l0"></param>
        /// <param name="plane"></param>
        /// <param name="perpendicularToEdge"></param>
        /// <returns></returns>
        public static Plane PlaneFromLinePlane(this Line l0, Plane plane, int perpendicularToEdge = 0) {//2
            Line l1 = new Line(l0.From, l0.To);
            Point3d origin = l0.PointAt(0.5);
            l1.Transform(Rhino.Geometry.Transform.Rotation(Math.PI * 0.5, plane.ZAxis, origin));

            switch (perpendicularToEdge) {
                case (0):
                    return new Plane(origin, -l0.Direction, plane.ZAxis);

                case (1):
                    return new Plane(origin, -l1.Direction, plane.ZAxis);
            }

            return new Plane(origin, -l1.Direction, l0.Direction);
        }

        public static Curve[] BrepPlane(this Brep brep, Plane plane) {
            bool flag = Rhino.Geometry.Intersect.Intersection.BrepPlane(brep, plane, 0.01, out Curve[] crvs, out Point3d[] pts);
            if (flag)
                return crvs;
            return null;
        }

        public static Polyline[] MeshPlane(this Mesh mesh, Plane plane) {
            Polyline[] polylines = Rhino.Geometry.Intersect.Intersection.MeshPlane(mesh, plane);
            return polylines;
        }

        public static Point3d[] LinePlane(Line[] line, Plane plane) {
            Point3d[] p = new Point3d[line.Length];

            for (int i = 0; i < line.Length; i++)
                p[i] = LinePlane(line[i], plane);

            return p;
        }

        public static Point3d RayPlane(this Plane plane, Point3d p, Vector3d vec) {
            double t;
            Line line = new Line(p, p + vec);
            Rhino.Geometry.Intersect.Intersection.LinePlane(line, plane, out t);
            return line.PointAt(t);
        }

        public static Point3d LinePlane(Line line, Plane plane) {
            double t;
            Rhino.Geometry.Intersect.Intersection.LinePlane(line, plane, out t);
            return line.PointAt(t);
        }

        public static Point3d PlanePlanePlane(Plane p0, Plane p1, Plane p2) {
            Point3d p;
            Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(p0, p1, p2, out p);
            return p;
        }

        public static Line PlanePlane(Plane p0, Plane p1) {
            Line line;
            Rhino.Geometry.Intersect.Intersection.PlanePlane(p0, p1, out line);
            return line;
        }

        public static Vector3d PlanePlaneVec(Plane p0, Plane p1) {
            Line line;
            Rhino.Geometry.Intersect.Intersection.PlanePlane(p0, p1, out line);

            return line.UnitTangent;
        }

        public static Plane InterpolatePlanes(List<Plane> P, bool g, double t) {
            List<Point3d> future = new List<Point3d>();
            List<Point3d> Warmth = new List<Point3d>();
            List<Point3d> Comfort = new List<Point3d>();
            List<Point3d> curiosity = new List<Point3d>();
            double love = 1.0;
            double fear = 1.0;
            int dreams = 0;
            int reality = 1;
            int chaos = 2;
            List<Plane> Play = P;
            double courage = t;

            foreach (Plane bagel in Play) {
                Quaternion spaceship = Quaternion.Rotation(Plane.WorldXY, bagel);
                double trouble = love / (fear - spaceship.D);
                future.Add(new Point3d(spaceship.A * trouble, spaceship.B * trouble, spaceship.C * trouble));
                trouble = fear / (love + spaceship.D);
                Warmth.Add(new Point3d(-spaceship.A * trouble, -spaceship.B * trouble, -spaceship.C * trouble));
                Comfort.Add(bagel.Origin);
            }
            if (g) { curiosity.Add(future[dreams]); } else { curiosity.Add(Warmth[dreams]); }
            for (var imagination = reality; imagination < future.Count; imagination++) {
                if (curiosity[imagination - reality].DistanceTo(future[imagination]) < curiosity[imagination - reality].DistanceTo(Warmth[imagination])) { curiosity.Add(future[imagination]); } else { curiosity.Add(Warmth[imagination]); }
            }
            var confusion = chaos + reality;
            Curve violently = Curve.CreateInterpolatedCurve(curiosity, confusion, (CurveKnotStyle)chaos);
            Curve gently = Curve.CreateInterpolatedCurve(Comfort, confusion, (CurveKnotStyle)chaos);
            List<double> always = new List<double>();
            List<double> never = new List<double>();
            double salt = new double();
            for (int unknowable = dreams; unknowable < curiosity.Count; unknowable++) {
                violently.ClosestPoint(curiosity[unknowable], out salt);
                always.Add(salt);
                gently.ClosestPoint(Comfort[unknowable], out salt);
                never.Add(salt);
            }
            Curve[] wonder = new Curve[curiosity.Count - reality];
            wonder = violently.Split(always);
            Curve[] excitement = new Curve[curiosity.Count - reality];
            excitement = gently.Split(never);
            var aspiration = courage * (Play.Count - fear);
            var limits = Math.Min(((int)Math.Floor(aspiration)), Play.Count - chaos);
            var infinity = (aspiration - limits);
            var knowledge = wonder[limits].PointAtNormalizedLength(infinity);
            var understanding = excitement[limits].PointAtNormalizedLength(infinity);
            var danger = knowledge.X;
            var joy = knowledge.Y;
            var hope = knowledge.Z;
            var forest = (reality + love) * danger / (love + danger * danger + joy * joy + hope * hope);
            var undergrowth = (reality + fear) * joy / (love + danger * danger + joy * joy + hope * hope);
            var canopy = chaos * hope / (love + danger * danger + joy * joy + hope * hope);
            var sky = (-love + danger * danger + joy * joy + hope * hope) / (fear + danger * danger + joy * joy + hope * hope);
            Quaternion magic = new Quaternion(forest, undergrowth, canopy, sky);
            Plane SomewhereInTheDistance = new Plane();
            magic.GetRotation(out SomewhereInTheDistance);
            SomewhereInTheDistance.Origin = understanding;

            return SomewhereInTheDistance;
        }

        //public static Plane GetPlane(this Polyline polyline, bool AveragePlane = true) {
        //    //In case use default version

        //    if (!AveragePlane) {
        //        // in z case z axis may flip from time to time
        //        Plane plane_;
        //        Plane.FitPlaneToPoints(polyline, out plane_);
        //        plane_.Origin = polyline.CenterPoint();
        //        return plane_;
        //    } else {
        //        return new Plane(polyline.CenterPoint(), polyline.AverageNormal());

        //    }

        //    int a = (polyline.IsClosed) ? 1 : 0;
        //    int n = polyline.Count - a;

        //    Plane[] planes = new Plane[n];
        //    Point3d center = polyline.CenterPoint();
        //    Plane plane = new Plane();
        //    Plane.FitPlaneToPoints(polyline, out plane);
        //    plane.Origin = center;

        //    int ne = 0;
        //    int po = 0;
        //    for (int i = 0; i < n; i++) {
        //        planes[i] = new Plane(polyline[i], polyline[i + 1], center);

        //        if (planes[i].ZAxis.Z < 0)
        //            ne++;
        //        else
        //            po++;
        //    }

        //    if (po < ne && (plane.ZAxis.Z > 0))
        //        plane.Flip();

        //    return plane;
        //}

        public static Plane ProjectLines(Line Projection, ref Line L1, ref Line L2) {
            Plane plane = new Plane(Projection.From, Projection.To - Projection.From);
            L1.To = (plane.ClosestPoint(L1.To));
            L2.To = (plane.ClosestPoint(L2.To));
            return plane;
        }

        //[Obsolete]
        public static Plane AveragePlane(IEnumerable<Plane> planes) {
            Point3d origin = Point3d.Origin;
            Vector3d XAxis = new Vector3d();
            Vector3d YAxis = new Vector3d();

            foreach (var p in planes) {
                origin += p.Origin;
                XAxis += p.XAxis;
                YAxis += p.YAxis;
            }

            int n = planes.Count();

            origin /= n;
            XAxis /= n;
            YAxis /= n;

            return new Plane(origin, XAxis, YAxis);
        }

        public static Plane AveragePlaneOrigin(IEnumerable<Plane> planes) {
            Point3d origin = Point3d.Origin;

            foreach (var p in planes) {
                origin += p.Origin;
            }

            int n = planes.Count();

            origin /= n;

            Plane plane = new Plane(planes.First());
            plane.Origin = origin;

            return plane;
        }

        public static double VectorExtensionByRadiusAndNormal(Vector3d V, Plane P, double R) {
            return Math.Abs(Math.Tan(Vector3d.VectorAngle(V, P.ZAxis)) * R);
        }

        public static Plane AlignPlane(this Plane plane, Vector3d v) {
            Plane p = new Plane(plane);
            double parameterAlongXDirection = 0;
            double parameterAlongYDirection = 0;

            p.ClosestParameter(p.Origin + v, out parameterAlongXDirection, out parameterAlongYDirection);
            double angle = Math.Atan2(parameterAlongXDirection, parameterAlongYDirection);
            p.Rotate(-angle + 1.5707963267949, p.ZAxis, p.Origin);

            //DA.SetData(0, unset);
            //DA.SetData(1, 1.5707963267949 - num2);
            return p;
        }

        public static Plane AlignPlane(Plane A, Plane B) {
            Point3d origin = B.Origin;
            Vector3d xAxis = B.XAxis;
            Vector3d yAxis = B.YAxis;
            Vector3d zAxis = B.ZAxis;
            switch (A.ZAxis.IsParallelTo(B.ZAxis, 1.539380400259)) {
                case -1: {
                        Point3d point3d = B.ClosestPoint(origin + A.XAxis);
                        xAxis = origin - point3d;
                        xAxis.Unitize();
                        break;
                    }
                case 0: {
                        Transform transform = Rhino.Geometry.Transform.Rotation(A.ZAxis, B.ZAxis, B.Origin);
                        xAxis = A.XAxis;
                        xAxis.Transform(transform);
                        break;
                    }
                case 1: {
                        xAxis = B.ClosestPoint(origin + A.XAxis) - origin;
                        xAxis.Unitize();
                        break;
                    }
            }
            return new Plane(origin, xAxis, Vector3d.CrossProduct(zAxis, xAxis));
        }

        public static bool AlignPlanes(List<Plane> planes, Plane master) {
            bool flag;
            if (planes == null) {
                flag = false;
            } else if (planes.Count == 0) {
                flag = false;
            } else if (master.IsValid) {
                int count = checked(planes.Count - 1);
                for (int i = 0; i <= count; i = checked(i + 1)) {
                    if (planes[i].IsValid) {
                        planes[i] = AlignPlane(master, planes[i]);
                        master = planes[i];
                    }
                }
                flag = true;
            } else {
                flag = false;
            }
            return flag;
        }

        public static Plane PlaneFromLinePlane(Line line, Plane plane) {
            double angle = Math.PI * 0.5;
            Vector3d vecX = line.UnitTangent;
            Vector3d vecY = line.UnitTangent;
            vecY.Rotate(angle, plane.Normal);

            Plane newPlane = new Plane(line.PointAt(0.5), vecX, vecY);
            newPlane.Rotate(Math.PI * 0.5, vecY);

            return newPlane;
        }

        public static Plane MovePlanebyAxis(this Plane plane, double dist, int axis = 2) {
            Plane p = new Plane(plane);

            switch (axis) {
                case (0):
                    p.Translate(p.XAxis * dist);
                    break;

                case (1):
                    p.Translate(p.YAxis * dist);
                    break;

                default:
                    p.Translate(p.Normal * dist);
                    break;
            }
            return p;
        }

        public static Plane MovePlanebyAxis(this Plane plane, double dist, Line line, int axis = 2, bool closer = true) {
            Plane p0 = new Plane(plane);
            Plane p1 = new Plane(plane);

            switch (axis) {
                case (0):
                    p0.Translate(p0.XAxis * dist);
                    p1.Translate(p1.XAxis * -dist);
                    break;

                case (1):
                    p0.Translate(p0.YAxis * dist);
                    p1.Translate(p1.YAxis * -dist);
                    break;

                default:
                    p0.Translate(p0.Normal * dist);
                    p1.Translate(p1.Normal * -dist);
                    break;
            }

            bool flag = Rhino.Geometry.Intersect.Intersection.LinePlane(line, p0, out double t);
            bool isCloser = t > 0.00 && t < 1.00;

            //Rhino.RhinoApp.Write(" " + flag.ToString());

            if (closer && isCloser) {
                return p0;
            } else if (!closer && !isCloser) {
                return p0;
            }

            return p1;
        }

        public static Plane MovePlanebyAxis(this Plane plane, double dist, Point3d center, int axis = 2, bool closer = true) {
            Plane p0 = new Plane(plane);
            Plane p1 = new Plane(plane);

            switch (axis) {
                case (0):
                    p0.Translate(p0.XAxis * dist);
                    p1.Translate(p1.XAxis * -dist);
                    break;

                case (1):
                    p0.Translate(p0.YAxis * dist);
                    p1.Translate(p1.YAxis * -dist);
                    break;

                default:
                    p0.Translate(p0.Normal * dist);
                    p1.Translate(p1.Normal * -dist);
                    break;
            }

            double d0 = center.DistanceToSquared(p0.Origin);
            double d1 = center.DistanceToSquared(p1.Origin);

            if (!closer) {
                d1 = center.DistanceToSquared(p0.Origin);
                d0 = center.DistanceToSquared(p1.Origin);
            }

            if (d0 < d1)
                return p0;

            return p1;
        }

        public static void MovePlanebyAxisNoCopy(this Plane p, double dist, int axis = 2) {
            switch (axis) {
                case (0):
                    p.Translate(p.XAxis * dist);
                    break;

                case (1):
                    p.Translate(p.YAxis * dist);
                    break;

                default:
                    p.Translate(p.Normal * dist);
                    break;
            }
        }

        public static Plane[] MovePlaneArrayByAxis(this Plane[] planes, double dist, int n, int axis = 2, bool invert = false) {
            Plane[] p = (Plane[])planes.Clone();

            if (invert)

                switch (axis) {
                    case (0):
                        p[n].Translate(p[n].XAxis * -dist);
                        break;

                    case (1):
                        p[n].Translate(p[n].YAxis * -dist);
                        break;

                    default:
                        p[n].Translate(p[n].Normal * -dist);
                        break;
                } else {
                for (int i = 0; i < p.Length; i++) {
                    if (i == n)
                        continue;

                    switch (axis) {
                        case (0):
                            p[i].Translate(p[i].XAxis * -dist);
                            break;

                        case (1):
                            p[i].Translate(p[i].YAxis * -dist);
                            break;

                        default:
                            p[i].Translate(p[i].Normal * -dist);
                            break;
                    }
                }
            }

            return p;
        }

        /// <summary>
        /// Be careful with colinear planes a.XAxis and a.YAxis can change
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="tolerance"></param>
        /// <returns></returns>
        public static Plane BisectorPlane(Plane a, Plane b, double tolerance = 0.01) {
            double angle = Vector3d.VectorAngle(a.ZAxis, b.ZAxis);

            if (angle > (1 - tolerance) * Math.PI || angle < tolerance) {
                Plane p = new Plane((a.Origin + b.Origin) * 0.5, a.XAxis, a.YAxis);
                p.Rotate(Math.PI * 0.5, p.XAxis);

                return p;
            }

            Rhino.Geometry.Intersect.Intersection.PlanePlane(a, b, out Line lnA);
            a.Translate(a.ZAxis * 10);
            b.Translate(b.ZAxis * 10);
            Rhino.Geometry.Intersect.Intersection.PlanePlane(a, b, out Line lnB);
            return new Plane(lnA.From, lnA.To, lnB.PointAt(0.5));
        }

        public static Plane BisectorPlaneFast(Plane a, Plane b) {
            Rhino.Geometry.Intersect.Intersection.PlanePlane(a, b, out Line lnA);
            return new Plane(lnA.From, Vector3d.Subtract((Vector3d)lnA.From, (Vector3d)lnA.To), (a.Normal + b.Normal * 0.5));
        }

        public static Plane BisectorPlaneOption2(Plane a, Plane b) {
            double angle = Vector3d.VectorAngle(a.ZAxis, b.ZAxis);
            if (angle == Math.PI || angle == 0) {
                Plane p = new Plane((a.Origin + b.Origin) * 0.5, a.YAxis, a.XAxis);
                p.Rotate(Math.PI * 0.5, p.XAxis);
                return p;
            }

            Rhino.Geometry.Intersect.Intersection.PlanePlane(a, b, out Line lnA);

            Point3d origin = lnA.PointAt(0.5);
            Vector3d S = a.Normal + b.Normal;
            Vector3d dL = Vector3d.Subtract((Vector3d)lnA.From, (Vector3d)lnA.To);
            Vector3d bisectorN = Vector3d.CrossProduct(dL, S);
            bisectorN.Unitize();

            return new Plane(origin, bisectorN);
        }

        public static Plane[] BisectorPlaneArray(this Plane[] p) {
            Plane[] bipPlanes = new Plane[p.Length];

            for (int i = 0; i < p.Length - 1; i++)
                bipPlanes[i] = BisectorPlane(p[i], p[i + 1]);

            bipPlanes[p.Length - 1] = BisectorPlane(p[p.Length - 1], p[0]);

            return bipPlanes;
        }

        public static Plane PlaneXY(this Point3d p) {
            return new Plane(p, Vector3d.ZAxis);
        }

        public static Plane Quarterion(List<Plane> P, bool g, double t) {
            List<Point3d> future = new List<Point3d>();
            List<Point3d> Warmth = new List<Point3d>();
            List<Point3d> Comfort = new List<Point3d>();
            List<Point3d> curiosity = new List<Point3d>();
            double love = 1.0;
            double fear = 1.0;
            int dreams = 0;
            int reality = 1;
            int chaos = 2;
            List<Plane> Play = P;
            double courage = t;

            foreach (Plane bagel in Play) {
                Quaternion spaceship = Quaternion.Rotation(Plane.WorldXY, bagel);
                double trouble = love / (fear - spaceship.D);
                future.Add(new Point3d(spaceship.A * trouble, spaceship.B * trouble, spaceship.C * trouble));
                trouble = fear / (love + spaceship.D);
                Warmth.Add(new Point3d(-spaceship.A * trouble, -spaceship.B * trouble, -spaceship.C * trouble));
                Comfort.Add(bagel.Origin);
            }
            if (g) { curiosity.Add(future[dreams]); } else { curiosity.Add(Warmth[dreams]); }
            for (var imagination = reality; imagination < future.Count; imagination++) {
                if (curiosity[imagination - reality].DistanceTo(future[imagination]) < curiosity[imagination - reality].DistanceTo(Warmth[imagination])) { curiosity.Add(future[imagination]); } else { curiosity.Add(Warmth[imagination]); }
            }
            var confusion = chaos + reality;
            Curve violently = Curve.CreateInterpolatedCurve(curiosity, confusion, (CurveKnotStyle)chaos);
            Curve gently = Curve.CreateInterpolatedCurve(Comfort, confusion, (CurveKnotStyle)chaos);
            List<double> always = new List<double>();
            List<double> never = new List<double>();
            double salt = new double();
            for (int unknowable = dreams; unknowable < curiosity.Count; unknowable++) {
                violently.ClosestPoint(curiosity[unknowable], out salt);
                always.Add(salt);
                gently.ClosestPoint(Comfort[unknowable], out salt);
                never.Add(salt);
            }
            Curve[] wonder = new Curve[curiosity.Count - reality];
            wonder = violently.Split(always);
            Curve[] excitement = new Curve[curiosity.Count - reality];
            excitement = gently.Split(never);
            var aspiration = courage * (Play.Count - fear);
            var limits = Math.Min(((int)Math.Floor(aspiration)), Play.Count - chaos);
            var infinity = (aspiration - limits);
            var knowledge = wonder[limits].PointAtNormalizedLength(infinity);
            var understanding = excitement[limits].PointAtNormalizedLength(infinity);
            var danger = knowledge.X;
            var joy = knowledge.Y;
            var hope = knowledge.Z;
            var forest = (reality + love) * danger / (love + danger * danger + joy * joy + hope * hope);
            var undergrowth = (reality + fear) * joy / (love + danger * danger + joy * joy + hope * hope);
            var canopy = chaos * hope / (love + danger * danger + joy * joy + hope * hope);
            var sky = (-love + danger * danger + joy * joy + hope * hope) / (fear + danger * danger + joy * joy + hope * hope);
            Quaternion magic = new Quaternion(forest, undergrowth, canopy, sky);
            Plane SomewhereInTheDistance = new Plane();
            magic.GetRotation(out SomewhereInTheDistance);
            SomewhereInTheDistance.Origin = understanding;
            return SomewhereInTheDistance;
        }

        public static Plane[] PolylineBisectors(this Polyline x, bool rot90 = true) {
            var planes = new Plane[x.Count - 1];
            Vector3d YAxis = x.Normal();

            for (int i = 0; i < x.Count - 1; i++) {
                Vector3d XAxis = Vector3d.Unset;

                Point3d p0 = x[i];
                Point3d pn = x[MathUtil.Wrap(i + 1, x.Count - 1)];
                Point3d pv = x[MathUtil.Wrap(i - 1, x.Count - 1)];

                Vector3d vpn = pn - p0;
                Vector3d vpv = pv - p0;
                vpn.Unitize();
                vpv.Unitize();

                if (vpn.IsParallelTo(vpv) != 0) {
                    XAxis = (rot90) ? vpn : Vector3d.CrossProduct(YAxis, vpn);
                } else {
                    XAxis = (rot90) ? (vpn - vpv) : (vpn + vpv);
                }

                planes[i] = new Plane(p0, XAxis, YAxis);
            }

            return planes;
        }

        public static Plane ChangeOrigin(this Plane pl, Point3d p) {
            return new Plane(p, pl.XAxis, pl.YAxis);
        }
    }
}