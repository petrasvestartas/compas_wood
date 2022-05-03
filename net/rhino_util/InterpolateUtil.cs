using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class InterpolateUtil {

        /// <summary>
        /// from http://paulbourke.net/miscellaneous/interpolation/
        /// Tension: 1 is high, 0 normal, -1 is low
        /// Bias: 0 is even,
        /// positive is towards first segment,
        /// negative towards the other
        /// </summary>
        /// <param name="y0"></param>
        /// <param name="y1"></param>
        /// <param name="y2"></param>
        /// <param name="y3"></param>
        /// <param name="mu"></param>
        /// <param name="tension"></param>
        /// <param name="bias"></param>
        /// <returns></returns>
        public static double HermiteInterpolate(double y0, double y1, double y2, double y3, double mu, double tension = 0.0, double bias = 0.0) {
            double m0, m1, mu2, mu3;
            double a0, a1, a2, a3;

            mu2 = mu * mu;
            mu3 = mu2 * mu;
            m0 = (y1 - y0) * (1 + bias) * (1 - tension) / 2;
            m0 += (y2 - y1) * (1 - bias) * (1 - tension) / 2;
            m1 = (y2 - y1) * (1 + bias) * (1 - tension) / 2;
            m1 += (y3 - y2) * (1 - bias) * (1 - tension) / 2;
            a0 = 2 * mu3 - 3 * mu2 + 1;
            a1 = mu3 - 2 * mu2 + mu;
            a2 = mu3 - mu2;
            a3 = -2 * mu3 + 3 * mu2;

            return (a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
        }

        /// <summary>
        /// from http://paulbourke.net/miscellaneous/interpolation/
        /// </summary>
        /// <param name="y0"></param>
        /// <param name="y1"></param>
        /// <param name="y2"></param>
        /// <param name="y3"></param>
        /// <param name="mu"></param>
        /// <returns></returns>
        public static double CubicInterpolate(double y0, double y1, double y2, double y3, double mu) {
            double a0, a1, a2, a3, mu2;

            mu2 = mu * mu;
            a0 = y3 - y2 - y0 + y1;
            a1 = y0 - y1 - a0;
            a2 = y2 - y0;
            a3 = y1;

            return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
        }

        /// <summary>
        /// from http://paulbourke.net/miscellaneous/interpolation/
        /// </summary>
        /// <param name="y1"></param>
        /// <param name="y2"></param>
        /// <param name="mu"></param>
        /// <returns></returns>
        public static double Lerp(double y1, double y2, double mu) =>
            y1 + (y2 - y1) * mu;

        //return y1 * (1 - mu) + y2 * mu;

        public static int Lerp(int y1, int y2, double mu) =>
            (int)(y1 + (y2 - y1) * mu);

        /*
        /// <summary>
        /// Simple lerp between two colors.
        /// </summary>
        /// <param name="c1">Color A.</param>
        /// <param name="c2">Color B.</param>
        /// <param name="t">t-value.</param>
        /// <returns>Interpolated color.</returns>
        public static Color Lerp(Color c1, Color c2, double t)
        {
            return Color.FromArgb(
                Lerp(c1.R, c2.R, t),
                Lerp(c1.G, c2.G, t),
                Lerp(c1.B, c2.B, t)
                );
        }
        */

        /// <summary>
        /// Simple linear interpolation between two points.
        /// </summary>
        /// <param name="pA"></param>
        /// <param name="pB"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        public static Point3d Lerp(Point3d pA, Point3d pB, double t) {
            return pA + (pB - pA) * t;
        }

        /// <summary>
        /// Simple linear interpolation between two vectors.
        /// </summary>
        /// <param name="vA"></param>
        /// <param name="vB"></param>
        /// <param name="t"></param>
        /// <returns></returns>
        public static Vector3d Lerp(Vector3d vA, Vector3d vB, double t) =>
            vA + t * (vB - vA);

        public static double Unlerp(double a, double b, double c) {
            if (a > b)
                return 1.0 - (c - b) / (a - b);
            return (c - a) / (b - a);
        }

        /// <summary>
        /// from http://paulbourke.net/miscellaneous/interpolation/
        /// </summary>
        /// <param name="y1"></param>
        /// <param name="y2"></param>
        /// <param name="mu"></param>
        /// <returns></returns>
        public static double CosineInterpolate(double y1, double y2, double mu) =>
            Lerp(y1, y2, (1 - Math.Cos(mu * Math.PI)) / 2);

        /// <summary>
        /// Spherical interpolation using quaternions.
        /// </summary>
        /// <param name="qA">Quaternion A.</param>
        /// <param name="qB">Quaternion B.</param>
        /// <param name="t">t-value.</param>
        /// <returns></returns>
        public static Quaternion Slerp(Quaternion qA, Quaternion qB, double t) {
            if (t == 0)
                return qA;
            if (t == 1.0)
                return qB;

            Quaternion qC = new Quaternion();
            double cosHT = qA.A * qB.A + qA.B * qB.B + qA.C * qB.C + qA.D * qB.D;

            if (cosHT < 0.0) {
                qC.A = -qB.A;
                qC.B = -qB.B;
                qC.C = -qB.C;
                qC.D = -qB.D;
                cosHT = -cosHT;
            } else
                qC = qB;

            if (cosHT >= 1.0) {
                qC.A = qA.A;
                qC.B = qA.B;
                qC.C = qA.C;
                qC.D = qA.D;
                return qC;
            }
            double HT = Math.Acos(cosHT);
            double sinHT = Math.Sqrt(1.0 - cosHT * cosHT);

            if (Math.Abs(sinHT) < 0.001) {
                qC.A = 0.5 * (qA.A + qC.A);
                qC.B = 0.5 * (qA.B + qC.B);
                qC.C = 0.5 * (qA.C + qC.C);
                qC.D = 0.5 * (qA.D + qC.D);
                return qC;
            }

            double ratioA = Math.Sin((1 - t) * HT) / sinHT;
            double ratioB = Math.Sin(t * HT) / sinHT;

            qC.A = qA.A * ratioA + qC.A * ratioB;
            qC.B = qA.B * ratioA + qC.B * ratioB;
            qC.C = qA.C * ratioA + qC.C * ratioB;
            qC.D = qA.D * ratioA + qC.D * ratioB;
            return qC;
        }

        public static Vector3d Slerp(Vector3d v1, Vector3d v2, double t) {
            double dot = v1 * v2;
            double theta = Math.Acos(dot) * t;
            Vector3d rel = v2 - v1 * dot;
            rel.Unitize();

            return ((v1 * Math.Cos(theta)) + rel * Math.Sin(theta));
        }

        /// <summary>
        /// Simple plane interpolation using interpolated vectors. Not ideal.
        /// Fails spectacularly in extreme cases.
        /// </summary>
        /// <param name="A">Plane A.</param>
        /// <param name="B">Plane B.</param>
        /// <param name="t">t-value.</param>
        /// <returns></returns>
        public static Plane InterpolatePlanes(Plane A, Plane B, double t) {
            return new Plane(Lerp(A.Origin, B.Origin, t),
                                     Lerp(A.XAxis, B.XAxis, t),
                                     Lerp(A.YAxis, B.YAxis, t));
        }

        /// <summary>
        /// Better plane interpolation using quaternions.
        /// </summary>
        /// <param name="A">Plane A.</param>
        /// <param name="B">Plane B.</param>
        /// <param name="t">t-value.</param>
        /// <returns></returns>
        public static Plane InterpolatePlanes2(Plane A, Plane B, double t) {
            Quaternion qA = Quaternion.Rotation(Plane.WorldXY, A);
            Quaternion qB = Quaternion.Rotation(Plane.WorldXY, B);

            Quaternion qC = Slerp(qA, qB, t);
            Point3d p = Lerp(A.Origin, B.Origin, t);

            Plane plane;
            qC.GetRotation(out plane);
            plane.Origin = p;

            return plane;
        }
    }
}