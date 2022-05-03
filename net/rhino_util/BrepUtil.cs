using Grasshopper;
using Grasshopper.Kernel.Data;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class BrepUtil {

        public static Brep[] BooleanUnion(this Brep[] breps0) {
            return BooleanUnion(breps0.ToList());
        }

        public static Brep[] BooleanUnion(this List<Brep> breps0) {
            List<Brep> unionedBreps = new List<Brep>();

            Brep unionBrep0 = breps0[0];
            for (int i = 1; i < breps0.Count; i++) {
                Brep[] temp = Brep.CreateBooleanUnion(new Brep[] { unionBrep0, breps0[i] }, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                //Rhino.RhinoApp.WriteLine(temp.Length.ToString());

                if (temp.Length == 0) {
                    unionedBreps.Add(breps0[i]);
                } else if (temp.Length == 1) {
                    temp[0].MergeCoplanarFaces(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                    unionBrep0 = temp[0];
                } else {
                    unionedBreps.AddRange(temp);

                    //unionBrep0 = Brep.CreateBooleanUnion(new Brep[] { unionBrep0, breps0[i] }, 0.01)[0];
                    //unionBrep0.MergeCoplanarFaces(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                }
            }
            unionedBreps.Add(unionBrep0);
            unionedBreps = new List<Brep> { Brep.CreateBooleanUnion(unionedBreps, 0.01)[0] };
            return unionedBreps.ToArray();
        }

        public static Brep[] XForm(this IEnumerable<Brep> polylines, Transform t) {
            Brep[] pp = new Brep[polylines.Count()];

            int count = 0;
            foreach (Brep p in polylines) {
                Brep pline = p.DuplicateBrep();
                p.Transform(t);
                pp[count++] = p;
            }
            return pp;
        }

        public static Brep CreatePipe(Curve centreCurve, List<Tuple<double, Plane>> frames, List<double> radii) {
            List<double> t = new List<double>();
            foreach (var f in frames)
                t.Add(f.Item1);

            Brep loft = Brep.CreatePipe(centreCurve, t, radii, true, PipeCapMode.None, false, 0.01, 0.01)[0];
            return loft;
        }

        public static Brep XForm(this Brep B, Rhino.Geometry.Transform xform) {
            Brep b = B.DuplicateBrep();
            b.Transform(xform);
            return b;
        }

        public static List<Curve> Get2ValenceCurves(this Brep brep) {
            List<Curve> selected = new List<Curve>(brep.Curves3D.Count);

            for (int i = 0; i < brep.Edges.Count; i++) {
                int[] ff = brep.Edges[i].AdjacentFaces();
                if (ff.Length == 2) {
                    if (ff[0] != ff[1])
                        selected.Add(brep.Curves3D[i]);
                } else {
                    selected.Add(brep.Curves3D[i]);
                }
            }

            if (selected.Count == 2) {
                selected[0].Reverse();
            }

            return selected;
        }

        public static Brep OrientBrep(this Brep b) {
            Brep bCopy = b.DuplicateBrep();
            try {
                int[][] ef = b.GetEdgeFaces();

                //Get Edges by facepair
                Dictionary<string, int> dictFToE = new Dictionary<string, int>();
                for (int i = 0; i < ef.Length; i++)
                    if (ef[i].Length == 2) {
                        dictFToE.Add(ef[i][0].ToString() + " " + ef[i][1].ToString(), i);
                        dictFToE.Add(ef[i][1].ToString() + " " + ef[i][0].ToString(), i);
                    }

                //Get brep edge and their flags
                //flag will be shifted based on bfs
                int[][] fe = b.GetFaceEdges();

                bool[][] feF = new bool[fe.Length][];

                for (int i = 0; i < fe.Length; i++)
                    feF[i] = new bool[] { true, false, true, false };

                Curve[][] feC = b.GetFaceCurves();

                //Iterate BFS
                Tuple<List<List<int>>, List<List<int[]>>> brepBFS = graphs.UndirectedGraphBfsRhino.BrepBFS(b);

                for (int i = 0; i < brepBFS.Item2[0].Count; i++) {// brepBFS.Item2[0].Count
                    int[] faces = brepBFS.Item2[0][i];

                    string pair = faces[0].ToString() + " " + faces[1].ToString();
                    //Print(pair);

                    int e = dictFToE[pair];
                    int[] efCurrent = ef[e];

                    int eLocal0 = Array.IndexOf(fe[faces[0]], e);
                    int eLocal1 = Array.IndexOf(fe[faces[1]], e);

                    //Print(feF[faces[0]][eLocal0].ToString() + " " + feF[faces[1]][eLocal1].ToString());
                    if (feF[faces[0]][eLocal0] != feF[faces[1]][eLocal1]) {
                        //Print("Yes");
                        feF[faces[1]] = feF[faces[1]].shiftRight();
                    } else {
                        //Print("No");
                    }
                    //Print(feF[faces[0]][eLocal0].ToString() + " " + feF[faces[1]][eLocal1].ToString());
                }

                for (int i = 0; i < fe.Length; i++)
                    if (!feF[i][0]) {
                        feC[i] = feC[i].Shift(1);
                        bCopy.Faces[i].Transpose(true);
                    }

                //FEC = feC;
                //FEF = feF;
                //BCopy = bCopy;
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return bCopy;
        }

        public static int[] EdgeDivisions(this Brep B, double D, ref int[][] FEDivisions) {
            //Brep data
            int[] faces = Enumerable.Range(0, B.Faces.Count).ToArray();
            int[][] fe = GetFaceEdges(B);
            bool[][] feF = new bool[fe.Length][];
            for (int i = 0; i < fe.Length; i++)
                feF[i] = Enumerable.Repeat(true, fe[i].Length).ToArray();
            int[][] ef = GetEdgeFaces(B);
            Curve[] ec = GetEdgeCurves(B);
            Curve[][] fc = GetFaceCurves(B);

            //Sequencing
            HashSet<int> fvis = new HashSet<int>();
            HashSet<int> evis = new HashSet<int>();

            List<List<int>> fSequences = new List<List<int>>();
            List<List<int>> eSequences = new List<List<int>>();
            List<List<int>> beSequences = new List<List<int>>();

            //Iterate faces and collect not visited faces and edges

            List<int> fStart = new List<int>();
            List<int> eStart = new List<int>();

            for (int i = 0; i < faces.Length * 2; i++) {//faces.Length*2
                //for(int j = 0; j < fe.Length; j++)
                //feF[j] = Enumerable.Repeat(true, fe[j].Length).ToArray();

                int f = i % faces.Length;

                //1.0 Find starting edges
                //Get not visited Edges
                bool foundUEdges = i >= faces.Length;
                if (foundUEdges) {
                    for (int j = 0; j < fe[f].Length; j += 2) {
                        if (fe[f][j] >= 0 && feF[f][j]) {
                            fStart.Add(f);
                            eStart.Add(j);
                            fvis.Add(f);
                            evis.Add(fe[f][j]);
                            feF[f][j] = false;
                        }
                    }
                }

                //Get not visited Edges
                if (!foundUEdges) {
                    for (int j = 1; j < fe[f].Length; j += 2) {
                        if (fe[f][j] >= 0 && feF[f][j]) {
                            fStart.Add(f);
                            eStart.Add(j);
                            fvis.Add(f);
                            evis.Add(fe[f][j]);
                            feF[f][j] = false;
                        }
                    }
                }

                //Print("Start" + eStart.Count.ToString());

                //Iterate to find one strip
                List<int> fSequence = new List<int>();
                List<int> eSequence = new List<int>();
                List<int> beSequence = new List<int>();
                int iterations = 0;
                while (eStart.Count != 0 && iterations < 500) {
                    List<int> fStartNext = new List<int>();
                    List<int> eStartNext = new List<int>();

                    //Iterate over found face edges

                    for (int j = 0; j < fStart.Count; j++) {
                        // Print(fStart[j].ToString() + " " + eStart[j].ToString() );

                        int be = (int)(Math.Abs(fe[fStart[j]][eStart[j]]));
                        int[] bf = ef[be];

                        if (bf.Length < 2)
                            continue;
                        int bfNext = (bf[0] == fStart[j]) ? bf[1] : bf[0];
                        int beNext = Array.IndexOf(fe[bfNext], be);
                        if (feF[bfNext][beNext] == false)
                            continue;
                        feF[bfNext][beNext] = false;
                        beNext = (beNext + 2) % 4;
                        feF[bfNext][beNext] = false;
                        fStartNext.Add(bfNext);
                        eStartNext.Add(beNext);
                    }

                    fSequence.AddRange(fStart);
                    eSequence.AddRange(eStart);

                    fStart = fStartNext;
                    eStart = eStartNext;

                    iterations++;
                }//while

                if (eSequence.Count > 0) {
                    for (int j = 0; j < eSequence.Count; j++) {
                        //feF[fSequence[j]][eSequence[j]] = false;
                        beSequence.Add(fe[fSequence[j]][eSequence[j]]);
                    }
                    eSequences.Add(eSequence);
                    fSequences.Add(fSequence);
                    beSequences.Add(beSequence);
                }
            }

            //Assign divisions for each edge

            int[] eDivisions = new int[B.Edges.Count];
            for (int i = 0; i < beSequences.Count; i++) {
                //Options
                //Average
                //Min
                //Max
                double[] lengths = new double[beSequences[i].Count];

                for (int j = 0; j < beSequences[i].Count; j++) {
                    lengths[j] = ec[beSequences[i][j]].GetLength();
                }

                double average = lengths.Average();
                double min = lengths.Min();
                double max = lengths.Max();
                //int division = Math.Max(1, (int)(average / D));
                double d = D;

                int division = Math.Max(1, (int)(Math.Ceiling(d)));
                //division = (int) (max / D);
                //Print(average.ToString());

                for (int j = 0; j < beSequences[i].Count; j++) {
                    int be = beSequences[i][j];
                    eDivisions[be] = division;
                }
            }

            int[][] feDivisions = new int[faces.Length][];
            for (int i = 0; i < faces.Length; i++) {
                feDivisions[i] = new int[fe[i].Length];
                for (int j = 0; j < fe[i].Length; j++) {
                    int be = Math.Abs(fe[i][j]);
                    feDivisions[i][j] = eDivisions[be];
                }
            }

            //Swap surfaces
            //bool[] swap = new bool[B.Faces.Count];
            FEDivisions = feDivisions;
            return eDivisions;

            //Output
            //FE = fe;
            //EF = ef;
            //EC = ec;
            //FC = fc;
            //FSeq = fSequences;
            //ESeq = eSequences;
            //BESeq = beSequences;
            //EDivisions = eDivisions;
        }

        public static int[][] GetFaceEdges(this Brep brep) {
            int[][] fe = new int[brep.Faces.Count][];

            for (int i = 0; i < brep.Faces.Count; i++) {
                int[] e = brep.Faces[i].AdjacentEdges();
                fe[i] = e;
            }//for
            return fe;
        }

        public static Curve[] GetEdgeCurves(this Brep brep) {
            Curve[] c = new Curve[brep.Edges.Count];
            for (int i = 0; i < brep.Edges.Count; i++) {
                c[i] = brep.Edges[i].EdgeCurve;
            }
            return c;
        }

        public static Curve[][] GetFaceCurves(this Brep brep) {
            Curve[][] fc = new Curve[brep.Faces.Count][];

            for (int i = 0; i < brep.Faces.Count; i++) {
                int[] e = brep.Faces[i].AdjacentEdges();
                Curve[] c = new Curve[e.Length];
                for (int j = 0; j < e.Length; j++) {
                    c[j] = brep.Edges[e[j]].EdgeCurve;
                }
                fc[i] = c;
            }//for
            return fc;
        }

        public static int[][] GetEdgeFaces(this Brep brep) {
            int[][] ef = new int[brep.Edges.Count][];

            for (int i = 0; i < brep.Edges.Count; i++) {
                int[] f = brep.Edges[i].AdjacentFaces();

                if (f.Length == 2)
                    if (f[0] == f[1])
                        f = new int[] { f[0] };

                ef[i] = f;
            }//for
            return ef;
        }

        public static DataTree<Curve> SectionBrep(this Brep M, Plane P, double D, double tolerance = 0.01) {
            //GetBoundingBox
            Box box = Box.Unset;
            BoundingBox bbox = M.GetBoundingBox(P, out box);
            int n = (int)Math.Ceiling(box.Z.Length / D);

            //Store Planes
            var plns = new List<Plane>();

            //Output cuts
            DataTree<Curve> cuts = new DataTree<Curve>();

            //Mesh Plane intersction
            double docTol = Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance;
            for (int i = 0; i < n; i++) {
                Plane plane = new Plane(box.PointAt(0, 0, 0) + P.ZAxis * i * D + P.ZAxis * tolerance, P.XAxis, P.YAxis);
                plns.Add(plane);
                Curve[] crvs; Point3d[] pts;
                Rhino.Geometry.Intersect.Intersection.BrepPlane(M, plane, docTol, out crvs, out pts);
                if (crvs != null) {
                    cuts.AddRange(crvs, new GH_Path(i));
                }
            }
            return cuts;
        }

        public static Brep Loft(Polyline c0, Polyline c1, bool closed = false) {
            return Loft(c0.ToNurbsCurve(), c1.ToNurbsCurve(), closed);
        }

        public static Brep Loft(Curve c0, Curve c1, bool closed = false) {
            Curve oriented = CurveUtil.GuideCurveDirection(c0, c1);

            Brep[] lofts = Brep.CreateFromLoft(new Curve[] { c0, c1 }, Point3d.Unset, Point3d.Unset, LoftType.Normal, false);

            //Test if loft is good
            if (lofts == null)
                return new Brep();
            if (lofts.Length == 0)
                return new Brep();

            //Take first one and split
            Brep loft = lofts[0];
            loft.Faces.SplitKinkyFaces();

            if (closed) {
                Brep loftCap = loft.CapPlanarHoles(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);

                if (loftCap != null) {
                    if (BrepSolidOrientation.Inward == loftCap.SolidOrientation)
                        loftCap.Flip();
                    //Rhino.RhinoApp.WriteLine((loftCap.IsValid).ToString());
                    //loftCap.Bake();
                    return loftCap;
                } else {
                    //loftCap.Bake();
                    return loft;
                }
            }

            return loft;
        }

        public static void IntersectBreps(DataTree<Brep> panels, DataTree<Brep> foundations, ref DataTree<Polyline> panelsCuts, ref DataTree<Polyline> foundationsCuts) {
            panelsCuts = new DataTree<Polyline>();
            foundationsCuts = new DataTree<Polyline>();

            // try {
            for (int i = 0; i < panels.BranchCount; i++) {
                for (int j = 0; j < foundations.BranchCount; j++) {
                    Polyline[] intersection0 = new Polyline[0];
                    Polyline[] intersection1 = new Polyline[0];

                    bool isIntersecting = IntersectBreps2x2(panels.Branch(i), foundations.Branch(j), ref intersection0, ref intersection1);

                    if (isIntersecting && intersection0.Length != 0 && intersection1.Length != 0) {
                        Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(intersection0[0]);
                        panelsCuts.AddRange(intersection0, new GH_Path(i));
                        foundationsCuts.AddRange(intersection1, new GH_Path(j));
                    }
                }
            }

            //} catch (Exception e) {
            //    Print(e.ToString());
            //}

            //A = panelsCuts;
            //B = foundationsCuts;
        }

        public static bool IntersectBreps2x2(List<Polyline> panels_, List<Polyline> foundation_, ref Polyline[] intersection0, ref Polyline[] intersection1) {
            try {
                List<Brep> panels = new List<Brep>();
                List<Brep> foundation = new List<Brep>();

                foreach (Polyline p in panels_) {
                    panels.Add(Brep.CreatePlanarBreps(p.ToNurbsCurve(), 0.01)[0]);
                }

                foreach (Polyline p in foundation_) {
                    foundation.Add(Brep.CreatePlanarBreps(p.ToNurbsCurve(), 0.01)[0]);
                }

                return IntersectBreps2x2(panels, foundation, ref intersection0, ref intersection1);
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }

            return false;
        }

        public static bool IntersectBreps2x2(List<Brep> panels, List<Brep> foundations, ref Polyline[] intersection0, ref Polyline[] intersection1) {
            try {
                Line c0 = BrepBrep(panels[0], foundations[0]);

                if (c0 != Line.Unset) {
                    Line c1 = BrepBrep(panels[0], foundations[1]);
                    Line c2 = BrepBrep(panels[1], foundations[0]);
                    Line c3 = BrepBrep(panels[1], foundations[1]);

                    Point3d[] pts = new Point3d[] { c0.PointAt(0.5), c1.PointAt(0.5), c2.PointAt(0.5), c3.PointAt(0.5) };
                    Plane plane;
                    Plane.FitPlaneToPoints(pts, out plane);
                    Point3d c = (c0.PointAt(0.5) + c1.PointAt(0.5) + c2.PointAt(0.5) + c3.PointAt(0.5)) * 0.25;
                    plane.Origin = c;
                    //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(c);
                    pts = new Point3d[] { c0.ClosestPoint(c, false), c1.ClosestPoint(c, false), c2.ClosestPoint(c, false), c3.ClosestPoint(c, false) };
                    Plane.FitPlaneToPoints(pts, out plane);

                    Point3d pm0 = PlaneUtil.LinePlane(c0, plane);
                    Point3d pm1 = PlaneUtil.LinePlane(c1, plane);
                    Point3d pm2 = PlaneUtil.LinePlane(c2, plane);
                    Point3d pm3 = PlaneUtil.LinePlane(c3, plane);

                    Polyline polyline0 = new Polyline(new Point3d[] { c0.From, pm0, pm1, c1.From, });
                    Polyline polyline1 = new Polyline(new Point3d[] { c2.From, pm2, pm3, c3.From, });

                    Polyline polyline2 = new Polyline(new Point3d[] { c0.To, pm0, pm2, c2.To, });
                    Polyline polyline3 = new Polyline(new Point3d[] { c1.To, pm1, pm3, c3.To, });

                    intersection0 = new Polyline[] { polyline0, polyline1 };
                    intersection1 = new Polyline[] { polyline2, polyline3 };

                    return true;
                }

                return false;
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }

            return false;
        }

        public static Line BrepBrep(Brep b0, Brep b1) {
            try {
                Curve[] crv;
                Point3d[] pts;
                if (Rhino.Geometry.Intersect.Intersection.BrepBrep(b0, b1, 0.01, out crv, out pts)) {
                    if (crv.Length > 0) {
                        return new Line(crv[0].PointAtStart, crv[0].PointAtEnd);
                    } else {
                        return Line.Unset;
                    }
                }
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return Line.Unset;
        }
    }
}