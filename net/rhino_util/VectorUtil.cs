using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class VectorUtil {

        public static Vector3d VectorProjection(Vector3d v0, Vector3d v1, bool extendV1 = false) {
            double v0L = v0.Length;
            double v1L = v1.Length;
            v0.Unitize();
            v1.Unitize();
            double dotProduct = v0 * v1;//, new Plane(L0.From, v1, v0));
            double v0ProjectionLen = v0L * dotProduct;

            // return v0ProjectionLen;
            if (extendV1)
                return v1 * (v0ProjectionLen + v1L);
            return v1 * v0ProjectionLen;
        }

        public static double GetAngle90(this Vector3d x, Vector3d y) {
            if (x.IsParallelTo(y) == 0) {
                return 90 - Math.Abs(Rhino.RhinoMath.ToDegrees(Vector3d.VectorAngle(x, y)) - 90);
            } else {
                return 0;
            }
        }

        public static Line ToLine(this Vector3d v, Point3d p, double scale = 1) {
            return new Line(p, p + v * scale);
        }

        public static Line ToLine(this Point3d p, Vector3d v, double scale = 1) {
            return new Line(p, p + v * scale);
        }

        public static int[] VectorSort(Plane plane, List<Point3d> pts) {
            Vector3d[] v = new Vector3d[pts.Count];
            double[] angles = new double[pts.Count];
            int[] id = new int[pts.Count];

            for (int i = 0; i < pts.Count; i++) {
                v[i] = pts[i] - plane.Origin;
                v[i].Unitize();
                angles[i] = Vector3d.VectorAngle(plane.XAxis, v[i], plane);
                id[i] = i;
            }

            Array.Sort(angles, id);

            return id;
        }

        public static int[] VectorSort(Plane plane, List<Vector3d> v) {
            double[] angles = new double[v.Count];
            int[] id = new int[v.Count];

            for (int i = 0; i < v.Count; i++) {
                v[i].Unitize();
                angles[i] = Vector3d.VectorAngle(plane.XAxis, v[i], plane);
                id[i] = i;
            }

            Array.Sort(angles, id);

            return id;
        }

        public static Vector3d UnitVector(this Vector3d vec) {
            Vector3d v = new Vector3d(vec);
            v.Unitize();
            return v;
        }

        /// <summary>
        /// Vectors must follow each other
        /// </summary>
        /// <param name="V0"></param>
        /// <param name="V1"></param>
        /// <param name="Z"></param>
        /// <returns></returns>
        public static Vector3d BisectorVector(Vector3d V0, Vector3d V1, Vector3d Z, bool followEachOther = true) {
            Vector3d v0 = new Vector3d(V0);
            Vector3d v1 = new Vector3d(V1);

            v0.Unitize();
            v1.Unitize();

            if (v0.IsParallelTo(v1) == -1)
                v1.Reverse();

            //Incase vector are consequtive or starting from the same point
            if (followEachOther) {
                v0 += v1;
            } else {
                v0 -= v1;
            }

            v0.Rotate(Math.PI * 0.5, Z);
            v0.Unitize();
            return v0;
        }

        public static Vector3d BisectorVector(Line V0, Line V1, Vector3d Z) {
            Line V2 = V1;
            if (V0.From.DistanceToSquared(V1.From) < 0.001 && V0.To.DistanceToSquared(V1.To) < 0.001)
                V2.Flip();

            bool flip = V0.From.DistanceToSquared(V2.From) < 0.001 || V0.To.DistanceToSquared(V2.To) < 0.001;

            return BisectorVector(V0.Direction, V2.Direction, Z, !flip);
        }

        public static Vector3d BisectorVector(Line V0, Vector3d Z) {
            return BisectorVector(V0, V0, Z);
        }

        /// <summary>
        /// line must follow one after another
        /// </summary>
        /// <param name="V"></param>
        /// <param name="Z"></param>
        /// <returns></returns>
        public static Vector3d BisectorVector(List<Line> V, Vector3d Z, bool order = true) {
            if (V.Count == 1)
                return BisectorVector(V[0], Z);

            List<Line> VOrdered = new List<Line>(V.Count);

            if (order) {
                VOrdered.Add(V[0]);

                for (int i = 1; i < V.Count; i++) {
                    Line l0 = VOrdered[i - 1];
                    Line l1 = V[i];

                    if (l0.From.DistanceToSquared(l1.From) < 0.001 || l0.To.DistanceToSquared(l1.To) < 0.001)
                        l1.Flip();

                    VOrdered.Add(l1);
                }
            } else {
                VOrdered = V;
            }

            Vector3d sumBisector = Vector3d.Zero;

            for (int i = 0; i < VOrdered.Count - 1; i++) {
                Vector3d bisector = BisectorVector(VOrdered[i], VOrdered[i + 1], Z);
                sumBisector += bisector;
            }
            sumBisector.Unitize();
            return sumBisector;
        }
    }
}