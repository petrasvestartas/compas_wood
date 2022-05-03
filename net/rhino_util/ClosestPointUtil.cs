using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util
{
    public static class ClosestPointUtil
    {
        public class ClosestCurveT
        {
            public List<Curve> cp = new List<Curve>();
            public List<Curve> L0 = new List<Curve>();
            public List<Curve> L1 = new List<Curve>();

            //minus means curve is touching by end
            public List<int> ID0 = new List<int>();

            public List<int> ID1 = new List<int>();
            public List<double> T0 = new List<double>();
            public List<double> T1 = new List<double>();
            public List<string> Mask = new List<string>();

            public override string ToString()
            {
                return "ClosestCurvesT" + " " + L0.Count.ToString();
            }
        }

        public static Tuple<int, double> CPCurvePoint(List<Curve> curves, Point3d p, double tolerance, int visited = -1)
        {
            int id0 = -1;
            double t0 = -1;

            for (int i = 0; i < curves.Count; i++)
            {
                if (i == visited)
                    continue;
                double t = curves[i].TCP(p);
                if (curves[i].PointAt(t).DistanceToSquared(p) < tolerance)
                {
                    id0 = i;
                    t0 = t;
                    break;
                }
            }

            return new Tuple<int, double>(id0, t0);
        }

        public static Tuple<int, double, int, double> CPCurvePoint(List<Curve> curves, Point3d p0, Point3d p1, double sqtolerance)
        {
            int id0 = -1;
            double t0 = -1;
            int id1 = -1;
            double t1 = -1;
            bool flag0 = false;
            bool flag1 = false;

            for (int i = 0; i < curves.Count; i++)
            {
                if (flag0 && flag1)
                    break;

                if (!flag0)
                {
                    double t0_ = curves[i].TCP(p0);
                    if (curves[i].PointAt(t0_).DistanceToSquared(p0) < sqtolerance)
                    {
                        id0 = i;
                        t0 = t0_;
                        flag0 = true;
                        continue;
                    }
                }

                if (!flag1)
                {
                    double t1_ = curves[i].TCP(p1);
                    if (curves[i].PointAt(t1_).DistanceToSquared(p1) < sqtolerance)
                    {
                        id1 = i;
                        t1 = t1_;
                        flag1 = true;
                        continue;
                    }
                }
            }

            return new Tuple<int, double, int, double>(id0, t0, id1, t1);
        }

        public static void ClosestCurveToNeedles(List<Curve> curves, List<Tuple<Point3d, Point3d>> needles, ref ClosestCurveT cp, List<string> mask = null, double sqTolerance = 20)
        {
            cp = new ClosestCurveT();

            int i = 0;
            foreach (Tuple<Point3d, Point3d> pair in needles)
            {
                // Rhino.RhinoApp.WriteLine("hi");
                var IndexAndParameter = CPCurvePoint(curves, pair.Item1, pair.Item2, sqTolerance);//how to define correct tolerance when beams are curved?

                //If at least one needle is found
                if (IndexAndParameter.Item1 != -1 || IndexAndParameter.Item3 != -1)
                {
                    cp.ID0.Add(IndexAndParameter.Item1);
                    cp.T0.Add(IndexAndParameter.Item2);

                    cp.ID1.Add(IndexAndParameter.Item3);
                    cp.T1.Add(IndexAndParameter.Item4);

                    //Mask
                    if (mask != null)
                        if (mask.Count == needles.Count)
                            cp.Mask.Add(mask[i]);
                }//if

                i++;
            }//for
        }

        public static void ClosestCurves(List<Curve> L, double tolerance, ref ClosestCurveT t)
        {
            double toleranceEnds = Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance;
            t = new ClosestCurveT();

            HashSet<long> pairs = new HashSet<long>();

            for (int i = 0; i < L.Count; i++)
            {
                for (int j = 0; j < L.Count; j++)
                {
                    //In order to check the same pair again
                    if (i == j)
                        continue;

                    long key = (i < j) ? MathUtil.GetKey(i, j) : MathUtil.GetKey(j, i);

                    if (pairs.Contains(key))
                        continue;

                    pairs.Add(key);

                    //Check order

                    bool checkEnds0 = L[i].PointAtStart.DistanceToSquared(L[j].PointAtStart) < toleranceEnds;//connected by ends
                    bool checkEnds1 = L[i].PointAtStart.DistanceToSquared(L[j].PointAtEnd) < toleranceEnds;//connected by ends
                    double t0, t1;

                    if (checkEnds0)
                    {
                        t.T0.Add(0);
                        t.T1.Add(0);
                        t.ID0.Add(-i);
                        t.ID1.Add(-j);
                        t.L0.Add(L[i]);
                        t.L1.Add(L[j]);
                    }
                    else if (checkEnds1)
                    {
                        t.T0.Add(0);
                        t.T1.Add(1);
                        t.ID0.Add(-i);
                        t.ID1.Add(-j);
                        t.L0.Add(L[i]);
                        t.L1.Add(L[j]);
                    }
                    else
                    {
                        Rhino.Geometry.Intersect.CurveIntersections ci = Rhino.Geometry.Intersect.Intersection.CurveCurve(L[i], L[j], tolerance, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                        if (ci.Count > 0)
                        {
                            Line line = new Line(ci[0].PointA, ci[0].PointB);

                            t0 = ci[0].ParameterA;
                            t1 = ci[0].ParameterB;

                            //Identify how lines are connected
                            int EndSide0 = (t0 < toleranceEnds || t0 > 1 - toleranceEnds) ? -1 : 1;
                            int EndSide1 = (t1 < toleranceEnds || t1 > 1 - toleranceEnds) ? -1 : 1;

                            t.T0.Add(t0);
                            t.T1.Add(t1);
                            t.ID0.Add(i * EndSide0);
                            t.ID1.Add(j * EndSide1);
                            t.L0.Add(L[i]);
                            t.L1.Add(L[j]);
                        }

                        //Touching - Not Touching
                        //Print(EndSide0.ToString() + " " + EndSide1.ToString());
                    }
                }
            }
        }

        public class ClosestLinesT
        {
            public List<Line> cp = new List<Line>();
            public List<Line> L0 = new List<Line>();
            public List<Line> L1 = new List<Line>();
            public List<int> ID0 = new List<int>();
            public List<int> ID1 = new List<int>();
            public List<double> T0 = new List<double>();
            public List<double> T1 = new List<double>();

            public override string ToString()
            {
                return "ClosestLinesT" + " " + L0.Count.ToString();
            }
        }

        public static void ClosestLines(List<Line> L, double tolerance, double toleranceEnds, ref ClosestLinesT t)
        {
            t = new ClosestLinesT();

            HashSet<long> pairs = new HashSet<long>();

            for (int i = 0; i < L.Count; i++)
            {
                for (int j = 0; j < L.Count; j++)
                {
                    //In order to check the same pair again
                    if (i == j)
                        continue;

                    long key = (i < j) ? MathUtil.GetKey(i, j) : MathUtil.GetKey(j, i);

                    if (pairs.Contains(key))
                        continue;

                    pairs.Add(key);

                    //Check order

                    bool checkEnds0 = L[i].From.DistanceToSquared(L[j].From) < toleranceEnds;
                    bool checkEnds1 = L[i].From.DistanceToSquared(L[j].To) < toleranceEnds;
                    double t0, t1;

                    if (checkEnds0)
                    {
                        t.T0.Add(0);
                        t.T1.Add(0);
                        t.ID0.Add(-i);
                        t.ID1.Add(-j);
                        t.L0.Add(L[i]);
                        t.L1.Add(L[j]);
                    }
                    else if (checkEnds1)
                    {
                        t.T0.Add(0);
                        t.T1.Add(1);
                        t.ID0.Add(-i);
                        t.ID1.Add(-j);
                        t.L0.Add(L[i]);
                        t.L1.Add(L[j]);
                    }
                    else if (Rhino.Geometry.Intersect.Intersection.LineLine(L[i], L[j], out t0, out t1, tolerance, true))
                    {
                        Line line = new Line(L[i].PointAt(t0), L[j].PointAt(t1));

                        //Identify how lines are connected
                        int EndSide0 = (t0 < toleranceEnds || t0 > 1 - toleranceEnds) ? -1 : 1;
                        int EndSide1 = (t1 < toleranceEnds || t1 > 1 - toleranceEnds) ? -1 : 1;

                        t.T0.Add(t0);
                        t.T1.Add(t1);
                        t.ID0.Add(i * EndSide0);
                        t.ID1.Add(j * EndSide1);
                        t.L0.Add(L[i]);
                        t.L1.Add(L[j]);

                        //Touching - Not Touching
                        //Print(EndSide0.ToString() + " " + EndSide1.ToString());
                    }
                }
            }
        }

        public static List<int[]> RTreeSearch(List<Point3d> pointsToSearchFrom, double dist, ref object A)
        {
            //If not empty
            if (pointsToSearchFrom.Count == 0)
                return new List<int[]>();

            //Search
            IEnumerable<int[]> found = RTree.Point3dClosestPoints(pointsToSearchFrom, pointsToSearchFrom, dist);
            HashSet<long> pairs = new HashSet<long>();

            List<int[]> result = new List<int[]>();

            int i = 0;
            foreach (var item in found)
            {
                int[] data = item;
                for (int j = 0; j < data.Length; ++j)
                {
                    int[] p = (i < data[j]) ? new int[] { i, data[j] } : new int[] { data[j], i };//sorts ids
                    long key = MathUtil.GetKey(p[0], p[1]);//create key

                    if (!pairs.Contains(key))
                    {//add only if pair does not exist
                        result.Add(p);
                        pairs.Add(key);
                    }
                }
                i++;
            }

            return result;
        }

        public static List<Point3d> RTreeSearch(List<Point3d> pointsToSearchFrom, List<Point3d> needles, double dist)
        {
            //If not empty
            if (pointsToSearchFrom.Count == 0 || needles.Count == 0)
                return new List<Point3d>();

            //Search
            IEnumerable<int[]> found = RTree.Point3dClosestPoints(pointsToSearchFrom, needles, dist);

            List<Point3d> result = new List<Point3d>();

            foreach (var item in found)
            {
                int[] data = item;
                for (int j = 0; j < data.Length; ++j)
                    result.Add(pointsToSearchFrom[data[j]]);
            }

            return result;
        }

        public static bool[] RTreeSearchID(List<Point3d> pointsToSearchFrom, List<Point3d> needles, double dist)
        {
            //If not empty
            if (pointsToSearchFrom.Count == 0 || needles.Count == 0)
                return Enumerable.Repeat(false, pointsToSearchFrom.Count).ToArray();

            //Search
            IEnumerable<int[]> found = RTree.Point3dClosestPoints(pointsToSearchFrom, needles, dist);
            bool[] flag = Enumerable.Repeat(false, pointsToSearchFrom.Count).ToArray();

            List<int> result = new List<int>();

            foreach (var item in found)
            {
                int[] data = item;
                for (int j = 0; j < data.Length; ++j)
                    flag[data[j]] = true;
            }

            return flag;
        }

        /// <summary>
        /// return indices of xArray in yArray
        /// point positions of xArray must be equal to yArray
        /// </summary>
        /// <param name="xArray"></param>
        /// <param name="yArray"></param>
        /// <returns></returns>
        public static int[] SortSetsOfPoints(Point3d[] xArray, Point3d[] yArray)
        {
            int[] xIndex = new int[xArray.Length];
            int[] yIndex = new int[yArray.Length];
            for (int i = 0; i < xArray.Length; i++)
                xIndex[i] = yIndex[i] = i;

            Array.Sort(xArray, xIndex);
            Array.Sort(yArray, yIndex);
            Array.Sort(yIndex, xIndex);
            return xIndex;
        }

        public static int[] RTreeSearchIDOnlyOne(Point3d[] pointsToSearchFrom, Point3d[] needles, double dist)
        {
            //If not empty
            if (pointsToSearchFrom.Length == 0 || needles.Length == 0)
                return Enumerable.Repeat(-1, pointsToSearchFrom.Length).ToArray();

            //Search
            IEnumerable<int[]> found = RTree.Point3dClosestPoints(pointsToSearchFrom, needles, dist);

            int[] result = new int[needles.Length];
            int count = 0;
            foreach (var item in found)
            {
                result[count++] = item.Length > 0 ? item[0] : -1;
            }

            return result;
        }

        public static List<Point3d> RTreeSearch(List<Point3d> pointsToSearchFrom, List<Point3d> needles, int C)
        {
            if (pointsToSearchFrom.Count == 0 || needles.Count == 0)
                return new List<Point3d>();

            IEnumerable<int[]> found = RTree.Point3dKNeighbors(pointsToSearchFrom, needles, C);

            List<Point3d> result = new List<Point3d>();

            foreach (int[] item in found)
            {
                int[] data = item;
                for (int j = 0; j < data.Length; ++j)
                    result.Add(pointsToSearchFrom[data[j]]);
            }

            return result;
        }
    }
}