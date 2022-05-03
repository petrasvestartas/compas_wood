using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class SurfaceUtil {

        public static Brep OutputBrep(Polyline ContourTop, Polyline ContourBot) {
            List<Brep> Pbreps = new List<Brep>();

            List<PolylineCurve> capconts = new List<PolylineCurve>();           // Caps
            capconts.Add(new PolylineCurve(ContourTop));
            capconts.Add(new PolylineCurve(ContourBot));

            Brep[] capS = Brep.CreatePlanarBreps(capconts);
            foreach (Brep brep in capS)
                Pbreps.Add(brep);

            List<Curve> loftconts = new List<Curve>();
            loftconts.Add(new PolylineCurve(ContourTop));               // Main Loft
            loftconts.Add(new PolylineCurve(ContourBot));
            Brep[] loft_conts = Brep.CreateFromLoft(loftconts, Point3d.Unset, Point3d.Unset, LoftType.Normal, false);
            foreach (Brep brep in loft_conts)
                Pbreps.Add(brep);

            Brep[] Breps = Brep.JoinBreps(Pbreps, 0.1);       // join

            foreach (Brep joinedBrep in Breps)
                if (joinedBrep.IsSolid == true) {
                    if (joinedBrep.SolidOrientation == Rhino.Geometry.BrepSolidOrientation.Inward)
                        joinedBrep.Flip();

                    return joinedBrep;
                    //RhinoDoc.ActiveDoc.Objects.AddBrep(joinedBrep);
                }

            return null;
        }

        /// <summary>
        /// Check if edge of surface are connected
        /// 0 - surface is open, 1 - U connected, 10 - V Connected, 11 - torus surface,
        /// </summary>
        /// <param name="surface"></param>
        /// <returns></returns>
        public static int GetEdgeConnectivity(this Surface surface, int divisionsU = 10, int divisionsV = 10) {
            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            int flag0Sum = 0;
            int flag1Sum = 0;
            for (double i = 0.0; i < 1; i += 0.111) {
                if (surface.PointAt(0, i).DistanceToSquared(surface.PointAt(1, i)) < 0.001)
                    flag0Sum++;
                if (surface.PointAt(i, 0).DistanceToSquared(surface.PointAt(i, 1)) < 0.001)
                    flag1Sum++;
            }

            bool joinedEdge0 = flag0Sum == 10;
            bool joinedEdge1 = flag1Sum == 10;

            int value = Convert.ToInt32(joinedEdge0) + Convert.ToInt32(joinedEdge1) * 10;

            //Check divisions, divisions are used for intance when checkers tyling does not match
            if (divisionsU % 2 != 0 || divisionsV % 2 != 0)
                if (value == 11)
                    return 0;

            if (divisionsV % 2 != 0 && value == 10)
                return 0;

            if (divisionsU % 2 != 0 && value == 1)
                return 0;

            return value;
        }

        public static List<Polyline> SubdivideSurfaceHex(Surface surface, int num, int num2, double num3) {
            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            List<Polyline> list = new List<Polyline>();
            double num6 = 1.0 / (double)num;
            double num7 = 1.0 / (double)num2;
            double num8 = num3;

            for (int i = 0; i <= num; i++) {
                for (int j = 0; j <= num2; j++) {
                    if ((i + j) % 2 == 0) {
                        Point3d item;
                        Point3d item2;
                        Point3d item4;
                        Point3d item5;
                        Point3d item6;
                        Point3d item7;

                        if (i > 0) {
                            if ((double)j + num8 > (double)num2)
                                item = surface.PointAt((double)(checked(i - 1)) * num6, (double)j * num7);
                            else
                                item = surface.PointAt((double)(checked(i - 1)) * num6, ((double)j + num8) * num7);
                            if ((double)j - num8 < 0.0)
                                item2 = surface.PointAt((double)(checked(i - 1)) * num6, (double)j * num7);
                            else
                                item2 = surface.PointAt((double)(checked(i - 1)) * num6, ((double)j - num8) * num7);
                        } else {
                            item = surface.PointAt((double)i * num6, (double)j * num7);
                            item2 = surface.PointAt((double)i * num6, (double)j * num7);
                        }
                        if (j > 0)
                            if ((double)j - (1.0 - num8) < 0.0)
                                item4 = surface.PointAt((double)i * num6, (double)(checked(j - 1)) * num7);
                            else
                                item4 = surface.PointAt((double)i * num6, ((double)j - (1.0 - num8)) * num7);
                        else
                            item4 = surface.PointAt((double)i * num6, (double)j * num7);
                        if (i < num) {
                            if ((double)j + num8 > (double)num2)
                                item5 = surface.PointAt((double)(checked(i + 1)) * num6, (double)j * num7);
                            else
                                item5 = surface.PointAt((double)(checked(i + 1)) * num6, ((double)j + num8) * num7);

                            if ((double)j - num8 < 0.0)
                                item6 = surface.PointAt((double)(checked(i + 1)) * num6, (double)j * num7);
                            else
                                item6 = surface.PointAt((double)(checked(i + 1)) * num6, ((double)j - num8) * num7);
                        } else {
                            item5 = surface.PointAt((double)i * num6, (double)j * num7);
                            item6 = surface.PointAt((double)i * num6, (double)j * num7);
                        }
                        if (j <= checked(num2 - 1))

                            item7 = surface.PointAt((double)i * num6, ((double)j + (1.0 - num8)) * num7);
                        else

                            item7 = surface.PointAt((double)i * num6, (double)j * num7);

                        if (i > 0 & j > 0 & i < num & j <= num2 - 1)
                            list.Add(new Polyline(new List<Point3d> { item, item2, item4, item6, item5, item7, item }));
                        else {
                            if (i == 0 & j == 0)
                                list.Add(new Polyline(new List<Point3d> { item, item6, item5, item7, item }));
                            if ((i == 0 & j == num2) && num2 % 2 == 0)
                                list.Add(new Polyline(new List<Point3d> { item, item4, item6, item5, item }));

                            if ((i == num & j == 0) && num % 2 == 0)
                                list.Add(new Polyline(new List<Point3d> { item, item2, item6, item7, item }));

                            if (i == num & j == num2)
                                if (num % 2 == 0 & num2 % 2 == 0)
                                    list.Add(new Polyline(new List<Point3d> { item, item2, item4, item7, item }));
                                else
                                    list.Add(new Polyline(new List<Point3d> { item, item2, item4, item7, item }));

                            if (i == 0 & j > 0 & j < num2)
                                list.Add(new Polyline(new List<Point3d> { item, item4, item6, item5, item7, item }));

                            if (i == num & j > 0 & j < num2)
                                list.Add(new Polyline(new List<Point3d> { item, item2, item4, item6, item7, item }));

                            if (j == 0 & i > 0 & i < num)
                                list.Add(new Polyline(new List<Point3d> { item, item2, item6, item5, item7, item }));

                            if (j == num2 & i > 0 & i < num) {
                                list.Add(new Polyline(new List<Point3d> { item, item2, item4, item6, item5, item }));
                            }
                        }
                    }
                }
            }

            return list;
        }

        public static List<Polyline> SubdivideSurfaceQuad(Surface surface, int num, int num2) {
            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            List<Polyline> list = new List<Polyline>();
            double num5 = 1.0 / (double)num;
            double num6 = 1.0 / (double)num2;

            for (int i = 0; i < num; i++) {
                for (int j = 0; j < num2; j++) {
                    Point3d point3d = surface.PointAt((double)i * num5, (double)j * num6);
                    Point3d point3d2 = surface.PointAt((double)(checked(i + 1)) * num5, (double)j * num6);
                    Point3d point3d3 = surface.PointAt((double)(checked(i + 1)) * num5,
                        (double)(checked(j + 1)) * num6);
                    Point3d point3d4 = surface.PointAt((double)i * num5, (double)(checked(j + 1)) * num6);
                    list.Add(new Polyline(new Point3d[] { point3d, point3d2, point3d3, point3d4, point3d }));
                }
            }
            return list;
        }

        public static List<Polyline> SubdivideSurfaceDiamond(Surface surface, int num, int num2) {
            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            List<Polyline> list = new List<Polyline>();
            List<Polyline> list2 = new List<Polyline>();
            double num5 = 1.0 / (double)num;
            double num6 = 1.0 / (double)num2;

            for (int i = 0; i <= num; i++) {
                for (int j = 0; j <= num2; j++) {
                    if ((i + j) % 2 == 0) {
                        Point3d point3d;
                        Point3d point3d2;
                        Point3d point3d3;
                        Point3d point3d4;
                        unchecked {
                            if (i > 0) {
                                point3d = surface.PointAt((double)(checked(i - 1)) * num5, (double)j * num6);
                            } else {
                                point3d = surface.PointAt((double)i * num5, (double)j * num6);
                            }
                            if (j > 0) {
                                point3d2 = surface.PointAt((double)i * num5, (double)(checked(j - 1)) * num6);
                            } else {
                                point3d2 = surface.PointAt((double)i * num5, (double)j * num6);
                            }
                            if (i < num) {
                                point3d3 = surface.PointAt((double)(checked(i + 1)) * num5, (double)j * num6);
                            } else {
                                point3d3 = surface.PointAt((double)i * num5, (double)j * num6);
                            }
                            if (j <= checked(num2 - 1)) {
                                point3d4 = surface.PointAt((double)i * num5, (double)(checked(j + 1)) * num6);
                            } else {
                                point3d4 = surface.PointAt((double)i * num5, (double)j * num6);
                            }
                        }
                        if (i > 0 & j > 0 & i < num & j <= num2 - 1) {
                            Polyline item =
                                new Polyline(new Point3d[] { point3d, point3d2, point3d3, point3d4 });
                            list.Add(item);
                        } else {
                            if (i == 0 & j == 0) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d3, point3d4, point3d});
                                list2.Add(item);
                            }
                            if (i == 0 & j == num2) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d3, point3d4, point3d});
                                list2.Add(item);
                            }
                            if (i == num & j == 0) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d3, point3d4, point3d});
                                list2.Add(item);
                            }
                            if (i == num & j == num2) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d3, point3d4, point3d});
                                list2.Add(item);
                            }
                            if (i == 0 & j > 0 & j < num2) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d2, point3d3, point3d4, point3d2, point3d});
                                list2.Add(item);
                            }
                            if (i == num & j > 0 & j < num2) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d4, point3d, point3d});
                                list2.Add(item);
                            }
                            if (j == 0 & i > 0 & i < num) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d3, point3d4, point3d, point3d});
                                list2.Add(item);
                            }
                            if (j == num2 & i > 0 & i < num) {
                                Polyline item = new Polyline(new Point3d[]
                                    {point3d, point3d2, point3d3, point3d, point3d});
                                list2.Add(item);
                            }
                        }
                    }
                }
            }

            return list;
        }

        public static List<Curve> SubdivideSurfaceIntoIsocurves(Surface S, int n) {
            S.SetDomain(0, new Interval(0, 1));
            S.SetDomain(1, new Interval(0, 1));

            //Get isocurves
            List<Curve> curves = new List<Curve>();
            double step = 1.0 / n;
            Curve curve;

            int counter = 0;
            for (double i = 0.0; i < 1 - (step * 0.01) + step; i += step) {
                curve = S.IsoCurve(1, i);
                curves.Add(curve);
                counter++;
            }//for j

            return curves;
        }

        public static List<Curve> SubdivideSurfaceIntoIsocurves(Surface S, int x, int y) {
            //Subdivide surface to hexagons
            List<double> distanceA = new List<double>();
            List<double> distanceB = new List<double>();
            //S = S.Rebuild(3, 3, 10, 10);
            S.SetDomain(0, new Interval(0, 1));
            S.SetDomain(1, new Interval(0, 1));
            //S = S.Transpose();

            Curve centerIsoCurve = S.IsoCurve(1, 0.5);
            Point3d[] isoPoints;
            centerIsoCurve.DivideByCount(x, true, out isoPoints);

            Curve[] isoCurves = new Curve[x + 1];
            Point3d[][] isoCurvesDivisionPt = new Point3d[isoPoints.Length][];

            double u, v;
            double T = 0.55;
            Point3d[] isoPoints1;
            for (int i = 0; i < x + 1; i++) {
                S.ClosestPoint(isoPoints[i], out u, out v);
                isoCurves[i] = S.IsoCurve(0, v);
                double[] t = isoCurves[i].DivideByCount(y, true, out isoPoints1);
                isoCurvesDivisionPt[i] = new Point3d[y + 1];
                isoCurvesDivisionPt[i][0] = isoCurves[i].PointAtStart;

                double t_ = 0;
                for (int j = 1; j < y; j++) {
                    //Lerp between (0.25 and 0.75)
                    if (i % 2 == 0)
                        t_ = (j % 2 == 0) ? MathUtil.Lerp(t[j - 1], t[j + 1], T) : MathUtil.Lerp(t[j - 1], t[j + 1], 1 - T);
                    else
                        t_ = (j % 2 == 0) ? MathUtil.Lerp(t[j - 1], t[j + 1], 1 - T) : MathUtil.Lerp(t[j - 1], t[j + 1], T);

                    isoCurvesDivisionPt[i][j] = isoCurves[i].PointAt(t_);
                }
                isoCurvesDivisionPt[i][y] = isoCurves[i].PointAtEnd;

                // isoCurvesDivisionPt[i] = isoPoints1;
            }

            return isoCurves.ToList();
        }

        private class MultiObject {
            public double curveParameter { get; set; }
            public int type { get; set; }

            public MultiObject(double curveParameter_, int type_) {
                curveParameter = curveParameter_;
                type = type_;
            }

            public void Add(double curveParameter_, int type_) {
                curveParameter = curveParameter_;
                type = type_;
            }
        }

        [Obsolete]
        public static Mesh SubdivideSurfaceEqualDist(Surface surface, int n, int y, double distance, double lerp, ref List<Polyline> po, ref List<Polyline> po1) {
            Mesh mesh = new Mesh();

            //Get curves for divisions (works with surface iso curves or just a list of curves)
            List<Curve> curves = SubdivideSurfaceIntoIsocurves(surface, n, y);

            //Remap curves domain
            for (int i = 0; i < curves.Count; i++) {
                curves[i].Domain = new Interval(0, 1);
                //curves[i].Rebuild(100, 3, false);
            }

            //Declare sortedList
            //List<MultiObject> sl = new List<MultiObject>(curves.Count);

            List<MultiObject>[] sl = new List<MultiObject>[curves.Count];
            for (int i = 0; i < curves.Count; i++) {
                sl[i] = new List<MultiObject>();
            }

            List<int> divisionsAll = new List<int>();
            Random random = new Random();

            int divisions, divisions1 = 1;

            try {
                for (int i = 0; i < curves.Count - 1; i++) {
                    divisions = (int)(Math.Ceiling(curves[i].GetLength() / distance));
                    divisions1 = (int)(Math.Ceiling(curves[i + 1].GetLength() / distance));

                    divisions = Math.Max(divisions1, divisions);
                    divisions = Math.Max(1, divisions);
                    divisionsAll.Add(divisions);

                    double[] t = curves[i].DivideByCount(divisions, false);
                    double[] t1 = curves[i + 1].DivideByCount(divisions, false);

                    //      double[] t1 = new double[t.Length];
                    //      for(int j = 0; j < t.Length; j++){
                    //        double tempT = 0.001;
                    //        curves[i + 1].ClosestPoint(curves[i].PointAt(t[j]), out tempT);
                    //        Print(tempT.ToString());
                    //        t1[j] = tempT+0.001;
                    //      }

                    double shift = 0;
                    double shift1 = 0;
                    if (divisions != 1) {
                        shift = t[0] * 0.5;
                        shift1 = t1[0] * 0.5;
                    }
                    Polyline polyline = new Polyline();
                    Polyline polyline1 = new Polyline();
                    polyline.Add(curves[i].PointAtStart);
                    polyline1.Add(curves[i + 1].PointAtStart);

                    if (i % 2 == 0) {
                        sl[i].Add(new MultiObject(0.0, 2));
                        sl[i + 1].Add(new MultiObject(0.0, 2));

                        for (int j = 0; j < t.Length; j++) {
                            double r = 0;
                            //if (sl[i].ContainsKey(t[j] - shift)) {
                            //    r = random.NextDouble() * 0.001;
                            //    // Rhino.RhinoApp.WriteLine(i.ToString());
                            //}
                            sl[i].Add(new MultiObject(t[j], 0));

                            //if (sl[i].ContainsKey(t1[j])) {
                            //    r = random.NextDouble() * 0.001;
                            //    // Rhino.RhinoApp.WriteLine(i.ToString());
                            //}

                            sl[i + 1].Add(new MultiObject(t1[j], 1));
                            polyline.Add(curves[i].PointAt(t[j]));
                            polyline1.Add(curves[i + 1].PointAt(t1[j]));
                        }

                        sl[i].Add(new MultiObject(1.0, 2));
                        sl[i + 1].Add(new MultiObject(1.0, 2));
                    } else {
                        if (i == curves.Count - 2)
                            sl[i + 1].Add(new MultiObject(0.0, 2));

                        for (int j = 0; j < t.Length; j++) {
                            double r = 0;
                            //if (sl[i].ContainsKey(t[j] - shift)) {
                            //    r = random.NextDouble() * 0.001;
                            //    // Rhino.RhinoApp.WriteLine(i.ToString());
                            //}

                            sl[i].Add(new MultiObject(t[j] - shift, 0));

                            //if (sl[i].ContainsKey(t1[j] - shift)) {
                            //    r = random.NextDouble() * 0.001;
                            //    // Rhino.RhinoApp.WriteLine(i.ToString());
                            //}
                            sl[i + 1].Add(new MultiObject(t1[j] - shift + r, 1));
                            polyline.Add(curves[i].PointAt(t[j] - shift));
                            polyline1.Add(curves[i + 1].PointAt(t1[j] - shift));
                        }

                        if (divisions != 1) {
                            sl[i].Add(new MultiObject(t[t.Length - 1] + shift, 0));
                            sl[i + 1].Add(new MultiObject(t1[t1.Length - 1] + shift1, 1));
                            if (i == curves.Count - 2)
                                sl[i + 1].Add(new MultiObject(1.0, 2));
                            polyline.Add(curves[i].PointAt(t[t.Length - 1] + shift));
                            polyline1.Add(curves[i + 1].PointAt(t1[t1.Length - 1] + shift1));
                        } else {
                            if (i == curves.Count - 2)
                                sl[i + 1].Add(new MultiObject(1.0, 2));
                        }
                    }

                    polyline.Add(curves[i].PointAtEnd);
                    po.Add(polyline);
                    polyline1.Add(curves[i + 1].PointAtEnd);
                    po1.Add(polyline1);
                }

                //Rebuild sorted list for 0.0 0.25 1.0 to 0.0 0.5 1.0
                List<double>[] sl_ = new List<double>[curves.Count];

                for (int i = 0; i < sl.Length; i++) {
                    sl[i] = sl[i].OrderBy(t => t.curveParameter).ToList();
                    sl_[i] = MathUtil.Range(0, 1, sl[i].Count).ToList();
                }

                //Create Mesh

                List<double> key = new List<double>();

                //Add Vertices
                for (int i = 0; i < curves.Count; i++)
                    for (int j = 0; j < sl_[i].Count; j++) {
                        mesh.Vertices.Add(curves[i].PointAt(MathUtil.Lerp(sl[i][j].curveParameter, (float)sl_[i][j], (float)Math.Max(0, Math.Min(0.0001 + lerp, 1)))));
                    }

                //Add Faces

                List<Polyline> allP = new List<Polyline>();
                List<Polyline> allP1 = new List<Polyline>();

                List<List<int>> allID0 = new List<List<int>>();
                List<List<int>> allID1 = new List<List<int>>();

                int vc = 0;
                int vc1 = sl[0].Count;
                for (int i = 0; i < curves.Count - 1; i++) {
                    Polyline p = new Polyline();
                    Polyline p1 = new Polyline();

                    var id0 = new List<int>();
                    var id1 = new List<int>();

                    int pCount = sl[i].Count;

                    if (i % 2 == 0) {
                        int allPCount = allP.Count;

                        //Solve end parts that are very small
                        if (divisionsAll[i] == 1) {
                            //  Rhino.RhinoApp.WriteLine(i.ToString() + " 0 " + pCount.ToString());

                            for (int j = 0; j < pCount; j++) {
                                p.Add(mesh.Vertices[vc]);
                                id0.Add(vc);
                                vc++;
                            }

                            allP.Add(p);
                            allID0.Add(id0);
                            allP[allP.Count - 1].Reverse();
                            allID0[allP.Count - 1].Reverse();

                            for (int j = 0; j < sl[i + 1].Count; j++) {
                                allP[allPCount].Add(mesh.Vertices[vc]);
                                allID0[allPCount].Add(vc);
                                vc++;
                            }
                        } else {
                            for (int j = 0; j < pCount; j++) {
                                int id = sl[i][j].type;

                                p.Add(mesh.Vertices[vc]);
                                id0.Add(vc);

                                if (id != 2 && id != 1) {
                                    allP.Add(p);
                                    allID0.Add(id0);
                                    if (p.Count != 0) {
                                        allP[allP.Count - 1].Reverse();
                                        allID0[allP.Count - 1].Reverse();
                                        p = new Polyline(new Point3d[] { mesh.Vertices[vc] });
                                        id0 = new List<int>() { vc };
                                    }//if
                                }//if

                                vc++;
                            }// for j

                            allP.Add(p);
                            allID0.Add(id0);
                            allP[allP.Count - 1].Reverse();
                            allID0[allP.Count - 1].Reverse();

                            for (int j = 0; j < sl[i + 1].Count; j++) {//sl[i+1].Count
                                int id = sl[i + 1][j].type;

                                allP[allPCount].Add(mesh.Vertices[vc]);
                                allID0[allPCount].Add(vc);

                                if (id != 2 && id != 0 && p.Count != 0) {
                                    allP[allPCount + 1].Add(mesh.Vertices[vc]);
                                    allID0[allPCount + 1].Add(vc);
                                    allPCount++;
                                }//if

                                vc++;
                            }// for j
                        }//if not very short segments
                    } else {//if mod
                        int allPCount1 = allP1.Count;

                        if (divisionsAll[i] == 1) {
                            //   Rhino.RhinoApp.WriteLine("1 " + pCount.ToString());

                            for (int j = 0; j < pCount; j++) {
                                p1.Add(mesh.Vertices[vc1]);
                                id1.Add(vc1);
                                vc1++;
                            }

                            allP1.Add(p1);
                            allID1.Add(id1);
                            allP1[allP1.Count - 1].Reverse();
                            allID1[allP1.Count - 1].Reverse();

                            for (int j = 0; j < sl[i + 1].Count; j++) {
                                allP1[allPCount1].Add(mesh.Vertices[vc1]);
                                allID1[allPCount1].Add(vc1);
                                vc1++;
                            }
                        } else {
                            for (int j = 0; j < pCount; j++) {
                                int id = sl[i][j].type;
                                p1.Add(mesh.Vertices[vc1]);
                                id1.Add(vc1);

                                if (id != 2 && id != 1) {
                                    allP1.Add(p1);
                                    allID1.Add(id1);

                                    if (p1.Count != 0) {
                                        allP1[allP1.Count - 1].Reverse();
                                        allID1[allP1.Count - 1].Reverse();
                                        p1 = new Polyline(new Point3d[] { mesh.Vertices[vc1] });
                                        id1 = new List<int>() { vc1 };
                                    }
                                }
                                vc1++;
                            }// for j

                            allP1.Add(p1);
                            allID1.Add(id1);
                            allP1[allP1.Count - 1].Reverse();
                            allID1[allP1.Count - 1].Reverse();

                            for (int j = 0; j < sl[i + 1].Count; j++) {//sl[i+1].Count
                                int id = sl[i + 1][j].type;
                                allP1[allPCount1].Add(mesh.Vertices[vc1]);
                                allID1[allPCount1].Add(vc1);

                                if (id != 2 && id != 0 && p1.Count != 0) {
                                    allP1[allPCount1 + 1].Add(mesh.Vertices[vc1]);
                                    allID1[allPCount1 + 1].Add(vc1);
                                    allPCount1++;
                                }
                                vc1++;
                            }// for j
                        }
                    }//mod
                }// for i

                int nf = 0;
                for (int i = 0; i < allP.Count; i++) {
                    allP[i].Close();

                    MeshFace[] faces = allP[i].TriangulateClosedPolyline();
                    if (faces != null) {
                        int fl = faces.Length;
                        for (int j = 0; j < fl; j++) {
                            mesh.Faces.AddFace(allID0[i][faces[j].A], allID0[i][faces[j].B], allID0[i][faces[j].C]);
                        }

                        mesh.Ngons.AddNgon(MeshNgon.Create(allID0[i], Enumerable.Range(nf, fl).ToList()));
                        nf += fl;
                    }
                }

                for (int i = 0; i < allP1.Count; i++) {
                    allP1[i].Close();

                    MeshFace[] faces = allP1[i].TriangulateClosedPolyline();
                    if (faces != null) {
                        int fl = faces.Length;

                        for (int j = 0; j < fl; j++) {
                            mesh.Faces.AddFace(allID1[i][faces[j].A], allID1[i][faces[j].B], allID1[i][faces[j].C]);
                        }

                        mesh.Ngons.AddNgon(MeshNgon.Create(allID1[i], Enumerable.Range(nf, fl).ToList()));
                        nf += fl;
                    }
                }

                // List<double> key = new List<double>();
                List<double> val = new List<double>();
                foreach (MultiObject mo in sl[curves.Count - 1]) {
                    key.Add(mo.curveParameter);
                    val.Add(mo.type);
                }
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            mesh.WeldUsingRTree(0.0001);

            return mesh;
        }

        private static object Range(int v1, int v2, int count) {
            throw new NotImplementedException();
        }
    }
}