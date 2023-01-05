using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino.Geometry;

namespace rhino_util {

    public static class BoundingBoxUtil {

        public static bool BoxBoxCollision(this Box boxA, Box boxB, double tolerance = 0.001) {
            //Check for collision of two boxes by by bouncing closest
            //points between them, starting from the vertices of boxA

            bool collision = false;
            var lines = new List<Line>();
            foreach (var ptA in boxA.GetCorners()) {
                var clPtB = boxB.ClosestPoint(ptA);
                var clPtA = boxA.ClosestPoint(clPtB);
                Line line = new Line(clPtB, clPtA);
                if (Math.Round(line.Length, 4) <= tolerance) {
                    collision = true;
                    break;
                }
            }

            if (collision == true)
                return true;

            //Also try mesh mesh search
            MeshFace[] faces = new MeshFace[]{
            new MeshFace(0, 3, 2, 1), new MeshFace(0, 1, 5, 4),
            new MeshFace(1, 2, 6, 5), new MeshFace(2, 3, 7, 6),
            new MeshFace(3, 0, 4, 7), new MeshFace(4, 5, 6, 7)
            };
            Mesh MA = new Mesh();
            MA.Vertices.AddVertices(boxA.GetCorners());
            MA.Faces.AddFaces(faces);
            Mesh MB = new Mesh();
            MB.Vertices.AddVertices(boxB.GetCorners());
            MB.Faces.AddFaces(faces);

            return Rhino.Geometry.Intersect.Intersection.MeshMeshFast(MA, MB).Length > 0;
        }

        //Side pairs - 0-2 1-3 4-5
        public static Plane[] ToPlanePairs(this Box box) {
            Brep brep = box.ToBrep();
            if (brep == null) return null;
            if (!brep.IsValid) return null;

            int[] order = new int[] { 0, 2, 1, 3, 4, 5 };//Side pairs - 0-2 1-3 4-5

            Plane[] planes = new Plane[6];
            for (int i = 0; i < 6; i++) {
                int a = order[i];
                double u = brep.Surfaces[a].Domain(0).Mid;
                double v = brep.Surfaces[a].Domain(1).Mid;
                planes[i] = new Plane(brep.Surfaces[a].PointAt(u, v), brep.Surfaces[a].NormalAt(u, v));
            }
            return planes;
        }

        public static List<Point3d> DoubleToPoints(double[][] numbers) {
            var pts = new List<Point3d>();

            for (int i = 0; i < numbers.Length; i++) {
                if (numbers[i].Length == 3) {
                    pts.Add(new Point3d(numbers[i][0], numbers[i][1], numbers[i][2]));
                } else if (numbers[i].Length == 2) {
                    pts.Add(new Point3d(numbers[i][0], numbers[i][1], 0));
                } else if (numbers[i].Length == 1) {
                    pts.Add(new Point3d(numbers[i][0], 0, 0));
                }
            }
            return pts;
        }

        public static double[][] CloudToDouble(PointCloud cloud, int size = 3, bool cullDuplicates = true) {
            Point3d[] p = cullDuplicates ? Point3d.CullDuplicates(cloud.GetPoints(), 0.001) : cloud.GetPoints();

            double[][] points = new double[cloud.Count][];

            for (int i = 0; i < p.Length; i++) {
                if (size == 3) {
                    points[i] = new double[] { p[i].X, p[i].Y, p[i].Z };
                } else if (size == 2) {
                    points[i] = new double[] { p[i].X, p[i].Y };
                } else if (size == 1) {
                    points[i] = new double[] { p[i].X };
                }
            }
            return points;
        }

        public static bool BoxBox(BoundingBox a0, BoundingBox b0, double scale = 0.01) {
            BoundingBox b = b0;
            b.Inflate(scale);
            BoundingBox a = a0;
            a.Inflate(scale);

            if (a.Max.X < b.Min.X)
                return false;
            if (a.Min.X > b.Max.X)
                return false;
            if (a.Max.Y < b.Min.Y)
                return false;
            if (a.Min.Y > b.Max.Y)
                return false;
            if (a.Max.Z < b.Min.Z)
                return false;
            if (a.Min.Z > b.Max.Z)
                return false;
            return true;
        }

        public static BoundingBox BoundingBox(Curve[] crvs, double inflate = 0.001) {
            BoundingBox bbox = crvs[0].GetBoundingBox(false);
            foreach (Curve c in crvs) {
                bbox.Union(c.GetBoundingBox(true));
                //c.Bake();
            }
            //Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(bbox.ToBrep());

            bbox.Inflate(inflate);
            return bbox;
        }

        public static Box BoundingBox(this IEnumerable<Line> lines, Plane plane, bool midPoints = true) {
            List<Point3d> points = new List<Point3d>(lines.Count() * 2);

            if (midPoints)
                foreach (Line l in lines) {
                    points.Add(l.PointAt(0.5));
                }
            else
                foreach (Line l in lines) {
                    points.Add(l.From);
                    points.Add(l.To);
                }

            if (plane != Plane.Unset) {
                return GetBoundingBoxAligned(points, plane);
            }

            return new Box(new BoundingBox(points));
        }

        public static Box[] BoundingBoxes(this IEnumerable<Line> lines, Plane plane0, Plane plane1, bool midPoints = true) {
            List<Point3d> points = new List<Point3d>(lines.Count() * 2);

            if (midPoints)
                foreach (Line l in lines) {
                    points.Add(l.PointAt(0.5));
                }
            else
                foreach (Line l in lines) {
                    points.Add(l.From);
                    points.Add(l.To);
                }

            if (plane0 != Plane.Unset && plane1 != Plane.Unset) {
                return new Box[] { GetBoundingBoxAligned(points, plane0), GetBoundingBoxAligned(points, plane1) };
            }

            return new Box[] { new Box((new BoundingBox(points))), new Box((new BoundingBox(points))) };
        }

        public static Box GetBoundingBoxAligned(List<Point3d> pts, Plane plane, double inflate = 0) {
            Transform transform = Transform.change_basis(Plane.WorldXY, plane);
            BoundingBox b = new BoundingBox(pts, transform);

            if (inflate > 0) {
                b.Inflate(inflate);
            }

            Point3d min = b.Min;
            double x = min.X;
            min = b.Max;
            Interval interval = new Interval(x, min.X);
            min = b.Min;
            double y = min.Y;
            min = b.Max;
            Interval interval1 = new Interval(y, min.Y);
            min = b.Min;
            double z = min.Z;
            min = b.Max;
            Box box = new Box(plane, interval, interval1, new Interval(z, min.Z));

            return box;
        }

        public static Box GetBoundingBoxAligned(Brep brep, Plane plane) {
            Transform transform = Transform.change_basis(Plane.WorldXY, plane);
            BoundingBox b = brep.GetBoundingBox(transform);

            Point3d min = b.Min;
            double x = min.X;
            min = b.Max;
            Interval interval = new Interval(x, min.X);
            min = b.Min;
            double y = min.Y;
            min = b.Max;
            Interval interval1 = new Interval(y, min.Y);
            min = b.Min;
            double z = min.Z;
            min = b.Max;
            Box box = new Box(plane, interval, interval1, new Interval(z, min.Z));

            return box;
        }

        public static bool Intersects(this BoundingBox current, BoundingBox other) {
            return
              (current.Min.X < other.Max.X) && (current.Max.X > other.Min.X) &&
              (current.Min.Y < other.Max.Y) && (current.Max.Y > other.Min.Y) &&
              (current.Min.Z < other.Max.Z) && (current.Max.Z > other.Min.Z);
        }

        public static bool IntersectRay(this BoundingBox bbox, Ray3d r) {
            double t;
            Point3d dirfrac = Point3d.Unset;
            dirfrac.X = 1.0f / r.Direction.X;
            dirfrac.Y = 1.0f / r.Direction.Y;
            dirfrac.Z = 1.0f / r.Direction.Z;
            // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
            // r.org is origin of ray
            Point3d lb = bbox.Min;
            Point3d rt = bbox.Max;

            double t1 = (lb.X - r.Position.X) * dirfrac.X;
            double t2 = (rt.X - r.Position.X) * dirfrac.X;
            double t3 = (lb.Y - r.Position.Y) * dirfrac.Y;
            double t4 = (rt.Y - r.Position.Y) * dirfrac.Y;
            double t5 = (lb.Z - r.Position.Z) * dirfrac.Z;
            double t6 = (rt.Z - r.Position.Z) * dirfrac.Z;

            double tmin = Math.Max(Math.Max(Math.Min(t1, t2), Math.Min(t3, t4)), Math.Min(t5, t6));
            double tmax = Math.Min(Math.Min(Math.Max(t1, t2), Math.Max(t3, t4)), Math.Max(t5, t6));

            // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
            if (tmax < 0) {
                t = tmax;
                return false;
            }

            // if tmin > tmax, ray doesn't intersect AABB
            if (tmin > tmax) {
                t = tmax;
                return false;
            }

            t = tmin;
            return true;
        }

        public static bool IntersectRay(this BoundingBox bbox, Ray3d r, out double t) {
            Point3d dirfrac = Point3d.Unset;
            dirfrac.X = 1.0f / r.Direction.X;
            dirfrac.Y = 1.0f / r.Direction.Y;
            dirfrac.Z = 1.0f / r.Direction.Z;
            // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
            // r.org is origin of ray
            Point3d lb = bbox.Min;
            Point3d rt = bbox.Max;

            double t1 = (lb.X - r.Position.X) * dirfrac.X;
            double t2 = (rt.X - r.Position.X) * dirfrac.X;
            double t3 = (lb.Y - r.Position.Y) * dirfrac.Y;
            double t4 = (rt.Y - r.Position.Y) * dirfrac.Y;
            double t5 = (lb.Z - r.Position.Z) * dirfrac.Z;
            double t6 = (rt.Z - r.Position.Z) * dirfrac.Z;

            double tmin = Math.Max(Math.Max(Math.Min(t1, t2), Math.Min(t3, t4)), Math.Min(t5, t6));
            double tmax = Math.Min(Math.Min(Math.Max(t1, t2), Math.Max(t3, t4)), Math.Max(t5, t6));

            // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
            if (tmax < 0) {
                t = tmax;
                return false;
            }

            // if tmin > tmax, ray doesn't intersect AABB
            if (tmin > tmax) {
                t = tmax;
                return false;
            }

            t = tmin;
            return true;
        }

        public static bool IntersectRay(this BoundingBox bbox, Ray3d r, out float t) {
            Point3d dirfrac = Point3d.Unset;
            dirfrac.X = 1.0f / r.Direction.X;
            dirfrac.Y = 1.0f / r.Direction.Y;
            dirfrac.Z = 1.0f / r.Direction.Z;
            // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
            // r.org is origin of ray
            Point3d lb = bbox.Min;
            Point3d rt = bbox.Max;

            double t1 = (lb.X - r.Position.X) * dirfrac.X;
            double t2 = (rt.X - r.Position.X) * dirfrac.X;
            double t3 = (lb.Y - r.Position.Y) * dirfrac.Y;
            double t4 = (rt.Y - r.Position.Y) * dirfrac.Y;
            double t5 = (lb.Z - r.Position.Z) * dirfrac.Z;
            double t6 = (rt.Z - r.Position.Z) * dirfrac.Z;

            double tmin = Math.Max(Math.Max(Math.Min(t1, t2), Math.Min(t3, t4)), Math.Min(t5, t6));
            double tmax = Math.Min(Math.Min(Math.Max(t1, t2), Math.Max(t3, t4)), Math.Max(t5, t6));

            // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
            if (tmax < 0) {
                t = (float)tmax;
                return false;
            }

            // if tmin > tmax, ray doesn't intersect AABB
            if (tmin > tmax) {
                t = (float)tmax;
                return false;
            }

            t = (float)tmin;
            return true;
        }
    }
}