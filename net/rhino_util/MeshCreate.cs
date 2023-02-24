using System.Collections.Generic;
using Rhino.Geometry;
using System.Drawing;
using System;
using System.Linq;
using Grasshopper;

namespace rhino_util {

    public static class MeshCreate {

        public static Mesh SkewedQuadMesh(Surface S, double UDiv, double VDist, double T, double Sc, double ThresholdOfOverlap) {
            double Thres = ThresholdOfOverlap == 0 ? VDist * 2 : ThresholdOfOverlap;

            //Inputs to construct mesh
            var ct = 0;
            var quads = new List<MeshFace>();
            var verts = new List<Point3d>();
            //var lines = new List<Line>();

            //Get Surface Interval, divisionStep, baseStep, maxStep and Delta parameters
            Interval DomU = new Interval(S.Domain(0));
            Interval DomV = new Interval(S.Domain(1));
            double StepU = (DomU.T1 - DomU.T0) / UDiv;
            double baseStepV = VDist;
            double maxStepV = 2500;
            double Delta = maxStepV - baseStepV;

            double totalV = DomV[1] - DomV[0];
            double rem = (totalV / 2) - ((maxStepV / 2) + baseStepV + Delta);
            double stepV = rem / baseStepV;

            //Iterate number strips of the NURBS
            double ctU = 0;

            for (int j = 0; j < UDiv; j++) {
                double ctV = 0;
                double thresh = totalV / 2;
                double StepV1 = baseStepV;
                bool b = true;
                var ListV = new List<double>();

                Point3d p0, p1, p2, p6, p7, p8;
                Point3d savept6 = Point3d.Unset;
                Point3d savept7 = Point3d.Unset;
                Point3d savept8 = Point3d.Unset;
                while (b) {
                    ListV.Add(StepV1);

                    //Get quad points from the surface

                    if (ctV == 0) {
                        p0 = S.PointAt(ctU, ctV);
                        p1 = S.PointAt(ctU + StepU * 0.5, ctV);
                        p2 = S.PointAt(ctU + StepU, ctV);
                        p6 = S.PointAt(ctU, ctV + StepV1 * (1 - T / 2));
                        p7 = S.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + T / 2));
                        p8 = S.PointAt(ctU + StepU, ctV + StepV1 * (1 - T / 2));
                        savept6 = p6;
                        savept7 = p7;
                        savept8 = p8;
                    } else {
                        p0 = savept6;
                        p1 = savept7;
                        p2 = savept8;
                        p6 = S.PointAt(ctU, ctV + StepV1 * (1 - T / 2));
                        p7 = S.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + T / 2));
                        p8 = S.PointAt(ctU + StepU, ctV + StepV1 * (1 - T / 2));
                        savept6 = p6;
                        savept7 = p7;
                        savept8 = p8;
                    }

                    //Add mesh faces
                    verts.AddRange(new Point3d[] { p0, p6, p7, p1 });
                    quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                    ct += 4;

                    verts.AddRange(new Point3d[] { p1, p7, p8, p2 });
                    quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                    ct += 4;

                    //Increment V value
                    ctV += StepV1;
                    thresh -= StepV1;
                    StepV1 += StepV1 * Sc;

                    //stop and mirror
                    if (thresh < Thres) {
                        //  Rhino.RhinoApp.WriteLine(thresh.ToString()+ "     U         " +DomU.ToString() + "      V      "+DomV.ToString());
                        ListV.Add(thresh);
                        ListV.Reverse();
                        double revCt = totalV / 2; //rev counter

                        for (int i = 0; i < ListV.Count - 1; i++) {
                            revCt += ListV[i];

                            if (i == 0) {
                                p0 = S.PointAt(ctU, revCt - ListV[i + 1] * T / 2);
                                p1 = S.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * T / 2);
                                p2 = S.PointAt(ctU + StepU, revCt - ListV[i + 1] * T / 2);

                                verts.AddRange(new Point3d[] { p6, p0, p1, p7 });
                                quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                                ct += 4;

                                verts.AddRange(new Point3d[] { p7, p1, p2, p8 });
                                quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                                ct += 4;

                                p6 = S.PointAt(ctU, revCt + ListV[i + 1] * (1 - T / 2));
                                p7 = S.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + T / 2));
                                p8 = S.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - T / 2));
                                savept6 = p6;
                                savept7 = p7;
                                savept8 = p8;
                            } else if (i == ListV.Count - 2) {
                                p0 = savept6;
                                p1 = savept7;
                                p2 = savept8;
                                p6 = S.PointAt(ctU, revCt + ListV[i + 1]);
                                p7 = S.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1]);
                                p8 = S.PointAt(ctU + StepU, revCt + ListV[i + 1]);
                            } else {
                                p0 = savept6;
                                p1 = savept7;
                                p2 = savept8;
                                p6 = S.PointAt(ctU, revCt + ListV[i + 1] * (1 - T / 2));
                                p7 = S.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + T / 2));
                                p8 = S.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - T / 2));
                                savept6 = p6;
                                savept7 = p7;
                                savept8 = p8;
                            }

                            verts.AddRange(new Point3d[] { p0, p6, p7, p1 });
                            quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                            ct += 4;
                            //lines.Add(new Line(p0, p7));

                            verts.AddRange(new Point3d[] { p1, p7, p8, p2 });
                            quads.Add(new MeshFace(ct, ct + 1, ct + 2, ct + 3));
                            ct += 4;
                            //lines.Add(new Line(p1, p8));
                        }
                        b = false;
                    }
                }

                ctU += StepU;
            }

            //Create Mesh
            Mesh mesh = new Mesh();
            mesh.Vertices.AddVertices(verts);
            mesh.Faces.AddFaces(quads);
            mesh.Weld(100);
           
            // foreach(Line l in lines)
            //  Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(l);

            return mesh;
        }

        public static void HexOrDiamondGrid(Brep B, int U, int V, ref List<Polyline> PC, ref List<Polyline> PN, double t = 0.5, bool HexOrDiamond = true, int[][] feDivisions = null) {
            try {
                double tolerance = Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance;
                double toleranceSq = tolerance * tolerance;
                int u = U;
                int v = V;

                //Get:
                //Brep edges
                //edge-face connectivity
                //Face not duplicate curves
                Brep brep = B;
                int[][] bef = new int[brep.Edges.Count][];
                Curve[] brepCurves = new Curve[brep.Edges.Count];

                Dictionary<int, List<Curve>> dictFaceCurves = new Dictionary<int, List<Curve>>();
                Dictionary<int, List<int>> dictBrepEdgeIDInFace = new Dictionary<int, List<int>>();

                for (int i = 0; i < brep.Faces.Count; i++) {
                    dictFaceCurves.Add(i, new List<Curve>());
                    dictBrepEdgeIDInFace.Add(i, new List<int>());
                }

                for (int i = 0; i < brep.Edges.Count; i++) {
                    bef[i] = brep.Edges[i].AdjacentFaces();
                    brepCurves[i] = brep.Edges[i].EdgeCurve;

                    //Print( " ");

                    //Check if edge is not connecting the same face
                    int n = bef[i].Length;
                    if (n == 2) {
                        if (bef[i][0] == bef[i][1])
                            n = 1;
                    }

                    for (int j = 0; j < n; j++) {
                        dictFaceCurves[bef[i][j]].Add(brepCurves[i]);
                        dictBrepEdgeIDInFace[bef[i][j]].Add(i);
                        //Print(bef[i][j].ToString() );
                    }
                }

                //BrepCurves = brepCurves;

                //For Naked edge
                List<Tuple<double, string, int>>[] BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID = new List<Tuple<double, string, int>>[brep.Edges.Count];
                for (int i = 0; i < brep.Edges.Count; i++)
                    BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i] = new List<Tuple<double, string, int>>();

                Dictionary<string, Polyline> dict_NGonID_Polyline = new Dictionary<string, Polyline>();
                //For Clothed polylines
                List<Polyline> plinesClothed = new List<Polyline>();

                //Hex Subdivision

                Mesh[] meshes = new Mesh[brep.Faces.Count];
                // var display = new List<Polyline>();
                //var display1 = new List<Point3d>();

                for (int i = 0; i < brep.Faces.Count; i++) {
                    //Reparametricize
                    Surface surface = brep.Faces[i].ToNurbsSurface();
                    surface.SetDomain(0, new Interval(0, 1));
                    surface.SetDomain(1, new Interval(0, 1));

                    //Check if mesh is cylindrical or torus
                    bool closed0 = surface.IsClosed(0);
                    bool closed1 = surface.IsClosed(1);

                    //Get naked eddges base on surface topology
                    Curve[] curves = dictFaceCurves[i].ToArray();

                    //Get Hex mesh
                    List<double> distanceA = new List<double>();
                    List<double> distanceB = new List<double>();
                    Mesh mesh;
                    int I = 4;

                    int udiv = (feDivisions == null) ? u : feDivisions[i][0] + (feDivisions[i][0] % 2);
                    int vdiv = (feDivisions == null) ? v : feDivisions[i][1] + (feDivisions[i][1] % 2);
                    udiv = (int)MathUtil.Constrain(udiv, 2, 1000000000);
                    vdiv = (int)MathUtil.Constrain(vdiv, 2, 1000000000);

                    //if (t == 1)
                    //surface=surface.Reverse(0, true);

                    if (HexOrDiamond && t != 0) {
                        Rhino.RhinoApp.WriteLine("Here");
                        if (feDivisions == null)
                            mesh = MeshCreate.HexMesh(surface, udiv, vdiv, t, 0, ref distanceA, ref distanceB, true, false);
                        else {
                            mesh = MeshCreate.HexMesh(surface, udiv, vdiv, t, 0, ref distanceA, ref distanceB, true, false);
                        }
                        I = 6;
                    } else {
                        if (feDivisions == null)
                            mesh = MeshCreate.DiamondMesh(surface, udiv, vdiv, 0, false);
                        else
                            mesh = MeshCreate.DiamondMesh(surface, udiv, vdiv, 0, false);

                        if (!surface.IsClosed(0) && !surface.IsClosed(1)) {
                            mesh = mesh.SimplifyMesh(new List<Point3d>(){
                            surface.PointAt(0, 0),
                            surface.PointAt(0, 1),
                            surface.PointAt(1, 0),
                            surface.PointAt(1, 1),
                        });
                        }
                    }

                    meshes[i] = mesh;

                    Polyline[] plines = mesh.GetPolylines();
                    //if (t == 1)
                    //    plines = mesh.GetFacePolylines().ToArray();

                    //Iterate over faces and get naked ones
                    for (int j = 0; j < mesh.Ngons.Count; j++) {
                        long NGonID = MathUtil.GetKey(i, j);

                        if (mesh.Ngons[j].BoundaryVertexCount != I) {
                            //Check which polyline segment is naked
                            int[] me = mesh._fe(j);

                            for (int k = 0; k < me.Length; k++) {
                                Point3d p0 = plines[j].SegmentAt(k).PointAt(0);
                                Point3d p1 = plines[j].SegmentAt(k).PointAt(1);

                                for (int l = 0; l < curves.Length; l++) {
                                    double t0, t1;
                                    curves[l].ClosestPoint(p0, out t0);
                                    curves[l].ClosestPoint(p1, out t1);

                                    if (curves[l].PointAt(t0).DistanceToSquared(p0) < tolerance && curves[l].PointAt(t1).DistanceToSquared(p1) < tolerance) {
                                        int BrepEdge = dictBrepEdgeIDInFace[i][l];
                                        double BrepEdgeCurveT;
                                        curves[l].ClosestPoint(plines[j].SegmentAt(k).PointAt(0.5), out BrepEdgeCurveT);
                                        int NGonEID = k;

                                        BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[BrepEdge].Add(new Tuple<double, string, int>(BrepEdgeCurveT, NGonID.ToString(), NGonEID));
                                    }//if polygon is on edge within toleranceSq
                                }//for brep edge
                            }//for ngon edgs

                            dict_NGonID_Polyline.Add(NGonID.ToString(), plines[j]);
                        } else {
                            plinesClothed.Add(plines[j]);
                        }
                    }
                }

                //Sort elements by order
                for (int i = 0; i < BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID.Length; i++)
                    BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i] = BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i].OrderByDescending(x => x.Item1).ToList();

                //Construct graph to get connected components
                bool hasEdges = false;
                var g = new rhino_util.graphs.UndirectedWeightedGraph(dict_NGonID_Polyline.Count);
                foreach (var pair in dict_NGonID_Polyline)
                    g.InsertVertex(pair.Key.ToString());

                //Rhino.RhinoApp.WriteLine("");
                //var curveParameters = new DataTree<Polyline>();
                for (int i = 0; i < BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID.Length; i++) {
                    Dictionary<string, List<string>> edges = new Dictionary<string, List<string>>();
                    for (int j = 0; j < BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i].Count; j++) {
                        double BrepEdgeCurveT = BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i][j].Item1;
                        //Rhino.RhinoApp.WriteLine(BrepEdgeCurveT.ToString());
                        string NGonID = BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i][j].Item2;
                        int NGonEID = BrepEdgeArray_BrepEdgeCurveT_NGonID_NGonEID[i][j].Item3;

                        string key = Math.Round(BrepEdgeCurveT, 4).ToString();
                        if (edges.ContainsKey(key))
                            edges[key].Add(NGonID);
                        else
                            edges.Add(key, new List<string> { NGonID });
                    }

                    //Add vertices as NGonID
                    //Add edges if pair.Value is 2
                    //Get Connected components

                    //Add edges

                    foreach (var pair in edges) {
                        //curveParameters.AddRange(pair.Value, new GH_Path(this.Component.RunCount - 1, i, counter++));
                        if (pair.Value.Count == 2) {
                            g.InsertEdge(pair.Value[0], pair.Value[1], 1);
                            hasEdges = true;
                        }
                        //        for(int j = 0; j < pair.Value.Count;j++){
                        //          curveParameters.Add(dict_NGonID_Polyline[pair.Value[j]], new GH_Path(this.Component.RunCount - 1, i, counter));
                        //        }
                        //        counter++;
                    }
                }
                //Print(hasEdges.ToString());

                List<Polyline> plinesNaked = new List<Polyline>();
                if (hasEdges) {
                    List<rhino_util.graphs.UndirectedWeightedGraph> cc = g.ConnectedComponents(g, false);

                    List<Mesh> meshes_ = new List<Mesh>();

                    for (int i = 0; i < cc.Count; i++) {
                        List<Polyline> plines = new List<Polyline>();
                        Mesh mesh = new Mesh();

                        for (int j = 0; j < cc[i].VertexList.Length; j++) {
                            Polyline pline = dict_NGonID_Polyline[cc[i].VertexList[j].name];
                            // Mesh faceMesh = Mesh.CreateFromClosedPolyline(pline);//Error
                            Mesh faceMesh = MeshUtil.StarMesh(pline);
                            //pline.Bake();
                            // faceMesh.Bake();
                            mesh.Append(faceMesh);
                            plines.Add(pline);
                        }

                        mesh.Clean();
                        mesh.Compact();
                        mesh.Vertices.CombineIdentical(true, true);
                        mesh.Vertices.CullUnused();

                        plines = mesh.GetNakedPolylines(true);
                        if (plines.Count == 1)
                            plinesNaked.Add(plines[0]);
                        meshes_.Add(mesh);
                        //mesh.Bake();
                        //curveParameters.AddRange(plines, new GH_Path(this.Component.RunCount - 1, i));
                    }

                    //Meshes = meshes_;
                } else {
                    //Print(dict_NGonID_Polyline.Count.ToString());
                    foreach (var pair in dict_NGonID_Polyline) {
                        plinesNaked.Add(pair.Value);
                    }
                }
                //MeshesO = meshes;

                PC = plinesClothed;
                PN = plinesNaked;
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
        }

        public static List<Mesh> MeshLoftMultiple(List<Curve> C_, int U, int V, ref int divisions, double scale = 1E5, bool reorder = true) {
            List<Curve> C = new List<Curve>(C_.Count);
            for (int i = 0; i < C_.Count; i++) {
                Curve c = C_[i].DuplicateCurve();
                c.Transform(Transform.Scale(Point3d.Origin, scale));
                C.Add(c);
            }

            //Input set

            if (C.Count > 1) {
                //Reorder Curve

                Curve[] curves = new Curve[C.Count];
                curves[0] = C[0].DuplicateCurve();

                for (int i = 1; i < C.Count; i++) {
                    Curve curve = C[i].DuplicateCurve();

                    if (reorder) {
                        if ((curves[i - 1].PointAtStart.Z < 0.01 && curves[i - 1].PointAtStart.Z > -0.01) &&
                            (curves[i - 1].PointAtEnd.Z < 0.01 && curves[i - 1].PointAtEnd.Z > -0.01) &&
                            (curve.PointAtStart.Z < 0.01 && curve.PointAtStart.Z > -0.01) &&
                            (curve.PointAtEnd.Z < 0.01 && curve.PointAtEnd.Z > -0.01)
                                ) {
                            if (CheckDir(curves[i - 1].PointAtStart, curves[i - 1].PointAtEnd, curve.PointAtStart, curve.PointAtEnd)) {
                                curve.Reverse();
                            }
                        }
                    }

                    curves[i] = curve;
                }

                //Convert to polylines
                Polyline[] polylines = new Polyline[C.Count];

                //First try to convert first curve
                Polyline polyline0;
                bool IsPolyline = curves[0].TryGetPolyline(out polyline0);

                int divisionsBase = Math.Max(1, V);

                if (!IsPolyline || V != 0) {
                    Point3d[] pts0;
                    curves[0].DivideByCount(divisionsBase, true, out pts0);
                    polyline0 = new Polyline(pts0);
                }

                polylines[0] = polyline0;
                divisions = polyline0.Count;

                //Then the rest

                for (int i = 1; i < C.Count; i++) {
                    Polyline tempPol;
                    if (curves[i].TryGetPolyline(out tempPol)) {
                        if (tempPol.Count == polylines[0].Count) {
                            polylines[i] = new Polyline(tempPol);
                            continue;
                        }
                    }

                    Point3d[] pts1;
                    curves[i].DivideByCount(polyline0.Count - 1, true, out pts1);
                    polylines[i] = new Polyline(pts1);
                }

                Curve[][] linesSets = new Curve[C.Count - 1][];

                for (int i = 0; i < C.Count - 1; i++) {
                    linesSets[i] = new Curve[polyline0.Count];
                    for (int j = 0; j < polyline0.Count; j++) {
                        linesSets[i][j] = (new Line(polylines[i][j], polylines[(i + 1)][j])).ToNurbsCurve();
                    }
                }

                // M = linesSets;

                List<Mesh> meshes = new List<Mesh>();
                for (int i = 0; i < linesSets.Length; i++) {
                    Mesh mesh = MeshCreate.MeshLoft(linesSets[i].ToList(), U);
                    mesh.Transform(Transform.Scale(Point3d.Origin, 1 / scale));
                    meshes.Add(mesh);
                }
                return meshes;
            }

            return null;
        }

        public static Mesh MeshLoft(List<Curve> C, int D = 10) {
            Mesh mesh = new Mesh();
            int divisions = Math.Max(1, D);
            bool ngon = true;

            foreach (Curve c in C) {
                Point3d[] p;
                c.DivideByCount(divisions, true, out p);
                mesh.Vertices.AddVertices(p);
                if (c.PointAtStart.DistanceToSquared(c.PointAtEnd) < 0.01)
                    mesh.Vertices.Add(p[0]);
            }

            for (int i = 0; i < C.Count - 1; i++) {
                for (int j = 0; j < divisions; j++) {
                    mesh.Faces.AddFace(
                      i * (divisions + 1) + j,
                      i * (divisions + 1) + j + 1,
                      (i + 1) * (divisions + 1) + j + 1,
                      (i + 1) * (divisions + 1) + j

                      );

                    if (ngon) {
                        mesh.Ngons.AddNgon(MeshNgon.Create(
                          new int[]{
            i * (divisions + 1) + j,
            i * (divisions + 1) + j + 1,
            (i + 1) * (divisions + 1) + j + 1,
            (i + 1) * (divisions + 1) + j
                          },
                          new int[] { mesh.Faces.Count - 1 }

                        ));
                    }
                }
            }

            mesh.Clean();
            return mesh;
        }

        public static bool CheckDir(Point3d A, Point3d B, Point3d C, Point3d D) {
            //https://stackoverflow.com/questions/2122305/convex-hull-of-4-points
            double triangle_ABC = (A.Y - B.Y) * C.X + (B.X - A.X) * C.Y + (A.X * B.Y - B.X * A.Y);

            // double triangle_ABD = (A.Y - B.Y) * D.X + (B.X - A.X) * D.Y + (A.X * B.Y - B.X * A.Y);
            double triangle_BCD = (B.Y - C.Y) * D.X + (C.X - B.X) * D.Y + (B.X * C.Y - C.X * B.Y);
            //double triangle_CAD = (C.Y - A.Y) * D.X + (A.X - C.X) * D.Y + (C.X * A.Y - A.X * C.Y);
            return !((triangle_ABC > 0) ^ (triangle_BCD > 0));
        }

        /// <summary>
        /// Create Mesh from Polylines
        /// Type 0 - meshFromClosedPolyline , 1 - star mesh
        /// </summary>
        /// <param name="nurbsCurves"></param>
        /// <param name="type = 0 - RhinoCommonFunction = 1 - Star Mesh"></param>
        /// <param name="weld"></param>
        /// <returns></returns>
        public static Mesh MeshFromPolylines(IEnumerable<Polyline> nurbsCurves, double weld, int triangulation = 0) {
            //Create Mesh and get face
            Mesh mesh = new Mesh();
            int v = 0;
            int f = 0;

            foreach (Polyline polyline in nurbsCurves) {
                if (!polyline.IsValid || polyline == null || polyline.Count < 3)
                    continue;

                Polyline p = polyline.Duplicate();

                if (!(p[0].DistanceToSquared(p[p.Count - 1]) < 0.001))
                    p.Close();

                //p.CollapseShortSegments(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                int switchVar = (triangulation < 0) ? triangulation : (polyline.Count - 1) * triangulation;
                //switch ( (polyline.Count - 1) * triangulation ) {//polyline.Count - 1
                switch (switchVar) {
                    case (0)://fan subdivision

                        //Polyline p = new Polyline(P);
                        Point3d center = p.CenterPoint();

                        p.RemoveAt(p.Count - 1);

                        mesh.Vertices.AddVertices(p);
                        mesh.Vertices.Add(center);

                        for (int i = 0; i < p.Count; i++) {
                            mesh.Faces.AddFace(v + i, v + ((i + 1) % p.Count), v + p.Count);
                        }

                        int[] tempV_ = new int[p.Count];
                        for (int i = 0; i < p.Count; i++)
                            tempV_[i] = v + i;

                        int[] tempF_ = new int[p.Count];
                        for (int i = 0; i < p.Count; i++)
                            tempF_[i] = f + i;

                        mesh.Ngons.AddNgon(MeshNgon.Create(tempV_, tempF_));

                        v += p.Count + 1;
                        f += p.Count;

                        break;

                    //case (-6):
                    //p.RemoveAt(p.Count - 1);
                    //mesh.Vertices.AddVertices(p);
                    //mesh.Faces.AddFace(v + 2, v + 1, v + 0);
                    //mesh.Faces.AddFace(v + 4, v + 3, v + 2);
                    //mesh.Faces.AddFace(v + 5, v + 2, v + 0);
                    //mesh.Faces.AddFace(v + 4, v + 2, v + 5);
                    //mesh.Ngons.AddNgon(MeshNgon.Create(new[] { v, v + 1, v + 2, v + 3, v + 4, v + 5 },
                    //    new[] { f + 0, f + 1, f + 2, f + 3 }));
                    //v += 6;
                    //f += 4;
                    //break;

                    //case (-5):
                    //p.RemoveAt(p.Count - 1);
                    //mesh.Vertices.AddVertices(p);
                    //mesh.Faces.AddFace(v + 2, v + 1, v + 0);
                    //mesh.Faces.AddFace(v + 3, v + 2, v + 0);
                    //mesh.Faces.AddFace(v + 4, v + 3, v + 0);
                    //mesh.Ngons.AddNgon(MeshNgon.Create(new[] { v, v + 1, v + 2, v + 3, v + 4 },
                    //    new[] { f + 0, f + 1, f + 2 }));
                    //v += 5;
                    //f += 3;
                    //break;

                    case (4):

                        p.RemoveAt(p.Count - 1);
                        mesh.Vertices.AddVertices(p);
                        mesh.Faces.AddFace(3 + v, 2 + v, 1 + v, 0 + v);
                        mesh.Ngons.AddNgon(MeshNgon.Create(new[] { v, 1 + v, 2 + v, 3 + v }, new[] { 0 + f }));
                        v += 4;
                        f += 1;

                        break;

                    case (3):

                        p.RemoveAt(p.Count - 1);
                        mesh.Vertices.AddVertices(p);
                        mesh.Faces.AddFace(2 + v, 1 + v, 0 + v);
                        mesh.Ngons.AddNgon(MeshNgon.Create(new[] { v, 1 + v, 2 + v }, new[] { f }));
                        v += 3;
                        f += 1;

                        break;

                    default:
                        //Mesh temp = Mesh.CreateFromClosedPolyline(p);
                        bool reverse = false;//TODO You can skip the nakedVerticesID part by reversing the polyline
                        Mesh temp = switchVar < 0 ? MeshUtil.TriangulateOpenPolyline(p, out reverse).Item1 : Mesh.CreateFromClosedPolyline(p);
                        // temp.Bake();

                        int[] tempF = new int[temp.Faces.Count];
                        for (int i = 0; i < temp.Faces.Count; i++)
                            tempF[i] = f + i;

                        List<List<int>> nakedVertices = temp.GetNakedVerticesID();//winding issue

                        if (nakedVertices.Count == 1) {
                            int[] tempV = new int[temp.Vertices.Count];
                            for (int i = 0; i < temp.Vertices.Count; i++)
                                tempV[i] = v + nakedVertices[0][i];

                            mesh.Append(temp);
                            mesh.Ngons.AddNgon(MeshNgon.Create(tempV, tempF));

                            v += temp.Vertices.Count;
                            f += temp.Faces.Count;
                        }

                        break;
                }
            }

            mesh.Compact();
            mesh.Vertices.CombineIdentical(true, true);
            mesh.Vertices.CullUnused();

            if (mesh.Ngons.Count > 0)
                mesh.UnifyNormalsNGons();
            else
                mesh.UnifyNormals();

            mesh.Weld(3.14159265358979);
            mesh.FaceNormals.ComputeFaceNormals();
            mesh.Normals.ComputeNormals();

            if (mesh.SolidOrientation() == -1)
                mesh.Flip(true, true, true);

            if (weld > 0) {
                mesh = mesh.WeldUsingRTree(0.01, true);
            }

            return mesh;
        }

        public static Mesh MeshFromPolygonalBrep(this Brep brep, double weld, ref Polyline[] outlines) {
            int counter = 0;
            foreach (BrepFace f in brep.Faces) {
                BrepLoop bl = f.OuterLoop;
                Curve crv = bl.To3dCurve();

                PolylineCurve polylineCurve = crv.ToPolyline(0.001, 0.001, 0, 999999);
                Curve tempCrv = polylineCurve.Simplify(CurveSimplifyOptions.All, 0.001, 0.001);

                tempCrv.TryGetPolyline(out outlines[counter++]);
            }

            return MeshCreate.MeshFromPolylines(outlines, weld, -1);

            ////Get brep edges
            //for (int i = 0; i < brep.Faces.Count; i++) {
            //    Curve c = brep.Faces[i].OuterLoop.To3dCurve();
            //    c.TryGetPolyline(out outlines[i]);
            //}
        }

        public static Mesh MeshToNgonMesh(this Mesh m, int i = 0) {
            List<List<int>> nakedVertices = m.GetNakedVerticesID();
            List<Polyline> nakedPolylines = m.GetNakedPolylines(nakedVertices);
            Mesh mesh = m.DuplicateMesh();

            if (nakedVertices.Count != 0) {
                int check = mesh.Ngons.AddNgon(MeshNgon.Create(nakedVertices[i.Wrap(nakedVertices.Count)], Enumerable.Range(0, m.Faces.Count).ToList()));
            }
            return mesh;
        }

        //public static Mesh
        /// <summary>
        /// type 0 - Hex 1 - quad
        /// subdivision 0 - triangle, 1 - star 2 - quads
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="p0"></param>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <param name="p4"></param>
        /// <param name="p5"></param>
        /// <param name="type"></param>
        private static void AddHexSubdivision(this Mesh mesh, ref Point3d p0, ref Point3d p1, ref Point3d p2, ref Point3d p3, ref Point3d p4, ref Point3d p5, int type = 0, int subdivision = 0) {
        }

        //public static Mesh
        public static Mesh HexMesh2(Surface surface, int U, int V, double t) {
            //int U = u_;
            //int V = v_;
            U = Math.Max(U, 2);
            V = Math.Max(V, 4);

            //Retrieve surface and reparametrize it
            //Surface surface = P[qq];

            surface.SetDomain(0, new Interval(0, 1));
            surface.SetDomain(1, new Interval(0, 1));

            int edgeConnectivity = surface.GetEdgeConnectivity();

            if (edgeConnectivity == 11) {
                if (U % 2 == 1)
                    U++;
                if (V % 2 == 1)
                    V++;
            }

            if (edgeConnectivity == 10) {
                //if(U % 2 == 1)
                //U++;
                if (V % 2 == 1)
                    V++;
            }

            if (edgeConnectivity == 1) {
                if (U % 2 == 1)
                    U++;
                //if(V % 2 == 1)
                //  V++;
            }

            double divU = 1.0 / (double)U;
            double divV = 1.0 / (double)V;

            double t_ = t;
            t_ = MathUtil.Constrain(t, 0.01, 0.5);

            //Quad subdivision
            List<Tuple<int, Polyline>> tiles = new List<Tuple<int, Polyline>>();

            for (int i = 0; i < U; i++) {
                for (int j = 0; j < V; j++) {
                    int ii = i;
                    int jj = j;

                    Point3d a = surface.PointAt((double)ii * divU, (double)jj * divV);
                    Point3d b = surface.PointAt((double)(ii + 1) * divU, (double)jj * divV);
                    Point3d c = surface.PointAt((double)(ii + 1) * divU, (double)(jj + 1) * divV);
                    Point3d d = surface.PointAt((double)ii * divU, (double)(jj + 1) * divV);

                    double t0 = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, 1 - t_);
                    double t2 = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, 1 - t_);
                    Point3d e = surface.PointAt(t0, (double)jj * divV);
                    Point3d f = surface.PointAt(t2, (double)(jj + 1) * divV);

                    double t0_ = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, t_);
                    double t2_ = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, t_);
                    Point3d e_ = surface.PointAt(t0_, (double)jj * divV);
                    Point3d f_ = surface.PointAt(t2_, (double)(jj + 1) * divV);

                    // Print(t0 + " " +t2 + " " +t0_ + " " +t2_+ " " );

                    switch (edgeConnectivity) {
                        case (1):
                            if (i == (U - 1) && j != (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(-1 + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(-1 + 1) * divU, (double)(jj + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(jj + 1) * divV);
                            }

                            if (i == (U - 1) && j == (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(-1 + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(-1 + 1) * divU, (double)(jj + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(jj + 1) * divV);
                                // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(c);
                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(b);
                            }

                            break;

                        case (10):
                            if (i != (U - 1) && j == (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(ii + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(ii + 1) * divU, (double)(-1 + 1) * divV);//center of hexagon
                                d = surface.PointAt((double)ii * divU, (double)(-1 + 1) * divV);//

                                t0 = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, 1 - t_);
                                t2 = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, 1 - t_);
                                e = surface.PointAt(t0, (double)jj * divV);
                                f = surface.PointAt(t2, (double)(-1 + 1) * divV);

                                t0_ = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, t_);
                                t2_ = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, t_);
                                e_ = surface.PointAt(t0_, (double)jj * divV);
                                f_ = surface.PointAt(t2_, (double)(-1 + 1) * divV);

                                // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(f);
                                //   Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(b);

                                // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(d);
                                // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(c);
                            }

                            if (i == (U - 1) && j == (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(ii + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(ii + 1) * divU, (double)(-1 + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(-1 + 1) * divV);

                                t0 = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, 1 - t_);
                                t2 = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, 1 - t_);
                                e = surface.PointAt(t0, (double)jj * divV);
                                f = surface.PointAt(t2, (double)(-1 + 1) * divV);

                                t0_ = MathUtil.Lerp((double)ii * divU, (double)(ii + 1) * divU, t_);
                                t2_ = MathUtil.Lerp((double)(ii + 1) * divU, (double)ii * divU, t_);
                                e_ = surface.PointAt(t0_, (double)jj * divV);
                                f_ = surface.PointAt(t2_, (double)(-1 + 1) * divV);
                                // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(d);
                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(c);
                            }

                            break;

                        case (11):
                            //Side0 points
                            if (i == (U - 1) && j != (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(-1 + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(-1 + 1) * divU, (double)(jj + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(jj + 1) * divV);
                            }

                            //Side1 points
                            if (i != (U - 1) && j == (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(ii + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(ii + 1) * divU, (double)(-1 + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(-1 + 1) * divV);
                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(c);
                            }

                            //Corner points
                            if (i == (U - 1) && j == (V - 1)) {
                                a = surface.PointAt((double)ii * divU, (double)jj * divV);
                                b = surface.PointAt((double)(-1 + 1) * divU, (double)jj * divV);
                                c = surface.PointAt((double)(-1 + 1) * divU, (double)(-1 + 1) * divV);
                                d = surface.PointAt((double)ii * divU, (double)(-1 + 1) * divV);
                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(d);
                            }

                            break;
                    }

                    if (jj % 2 == 0 && ii % 2 == 0) {
                        Polyline polyline = new Polyline(new Point3d[]{
                a,
                e_,
                b,
                c,
                f_,
                d
                });

                        tiles.Add(new Tuple<int, Polyline>(0, polyline));
                    }//if

                    if (jj % 2 == 1 && ii % 2 == 0) {
                        Polyline polyline = new Polyline(new Point3d[]{
                d,
                f,
                c,
                b,
                e,
                a
                });
                        tiles.Add(new Tuple<int, Polyline>(1, polyline));
                    }//if

                    if (jj % 2 == 0 && ii % 2 == 1) {
                        Polyline polyline = new Polyline(new Point3d[]{
                b,
                e,
                a,
                d,
                f,
                c
                });
                        tiles.Add(new Tuple<int, Polyline>(2, polyline));
                    }//if

                    if (jj % 2 == 1 && ii % 2 == 1) {
                        Polyline polyline = new Polyline(new Point3d[]{
                c,
                f_,
                d,
                a,
                e_,
                b
                });

                        tiles.Add(new Tuple<int, Polyline>(3, polyline));
                    }//if
                }//for j
            }//for i

            //Mesh quads
            //int v = 0;
            //int f = 0;
            Mesh mesh = new Mesh();
            List<Polyline> p = new List<Polyline>();
            //mesh.TopologyVertices.ConnectedTopologyVertices(v, true);
            int n = 6;

            //Add mesh faces
            for (int i = 0; i < tiles.Count; i++) {
                p.Add(tiles[i].Item2);
                mesh.Vertices.AddVertices(tiles[i].Item2);
                switch (tiles[i].Item1) {
                    case (2):
                    case (1):
                        mesh.Faces.AddFace((i * n) + 1, (i * n) + 0, (i * n) + 5);
                        mesh.Faces.AddFace((i * n) + 5, (i * n) + 4, (i * n) + 1);
                        mesh.Faces.AddFace((i * n) + 4, (i * n) + 3, (i * n) + 2);
                        mesh.Faces.AddFace((i * n) + 2, (i * n) + 1, (i * n) + 4);

                        //mesh.Faces.AddFace((i * n) + 5, (i * n) + 4, (i * n) + 1, (i * n) + 0);
                        //mesh.Faces.AddFace((i * n) + 4, (i * n) + 3, (i * n) + 2, (i * n) + 1);
                        break;

                    case (0):
                    case (3):
                        mesh.Faces.AddFace((i * n) + 1, (i * n) + 4, (i * n) + 5);
                        mesh.Faces.AddFace((i * n) + 5, (i * n) + 0, (i * n) + 1);
                        mesh.Faces.AddFace((i * n) + 2, (i * n) + 3, (i * n) + 4);
                        mesh.Faces.AddFace((i * n) + 4, (i * n) + 1, (i * n) + 2);
                        //mesh.Faces.AddFace((i * n) + 0, (i * n) + 1, (i * n) + 4, (i * n) + 5);
                        //mesh.Faces.AddFace((i * n) + 1, (i * n) + 2, (i * n) + 3, (i * n) + 4);
                        break;
                }
            }

            //Add NGons around 2nd and 5th vertex
            HashSet<int> visitedVID = new HashSet<int>();

            List<int[]> connectedFaces = new List<int[]>();
            List<int[]> connectedVertices = new List<int[]>();
            for (int i = 0; i < tiles.Count; i++) {//tiles.Count
                int v0 = (i * n) + 2;
                int v1 = (i * n) + 5;
                v0 = mesh.TopologyVertices.TopologyVertexIndex(v0);
                v1 = mesh.TopologyVertices.TopologyVertexIndex(v1);

                //   Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(mesh.TopologyVertices[v0]);
                //  Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(mesh.TopologyVertices[v1]);

                int[] tv0 = mesh.Vertices.GetTopologicalIndenticalVertices(v0);
                int[] tv1 = mesh.Vertices.GetTopologicalIndenticalVertices(v1);

                //1st check if vertices are visited
                if (!visitedVID.Contains(v0)) {//if not collect face indices
                    visitedVID.Add(v0);

                    connectedFaces.Add(mesh.TopologyVertices.ConnectedFaces(v0));
                }

                if (!visitedVID.Contains(v1)) {
                    visitedVID.Add(v1);
                    connectedFaces.Add(mesh.TopologyVertices.ConnectedFaces(v1));
                }

                if (tv0 != null) {
                    foreach (int v in tv0) {
                        visitedVID.Add(mesh.TopologyVertices.TopologyVertexIndex(v));
                    }
                }

                if (tv1 != null) {
                    foreach (int v in tv1) {
                        visitedVID.Add(mesh.TopologyVertices.TopologyVertexIndex(v));
                    }
                }
            }

            List<int> visitedV = new List<int>();

            // Print(mesh.TopologyVertices.Count.ToString());
            //  Print(mesh.Vertices.Count.ToString());
            mesh.Vertices.CombineIdentical(true, true);
            mesh.Vertices.CullUnused();
            mesh.Weld(3.14159265358979);
            mesh.FaceNormals.ComputeFaceNormals();
            mesh.Normals.ComputeNormals();

            //if (mesh.SolidOrientation() == -1)
            //mesh.Flip(true, true, true);

            //From Collected
            //For constructing ngon we already know faces but we do not know outlines
            for (int i = 0; i < connectedFaces.Count; i++) {
                int[] check0 = MeshUtil.GetFaceArray(mesh, connectedFaces[i][0]);
                int[] check1 = MeshUtil.GetFaceArray(mesh, connectedFaces[i][1]);

                if (connectedFaces[i].Length == 2) {
                    int[] sharedVertices = check0.Intersect(check1).ToArray();
                    int distictVertices0 = check0.Except(check1).First();
                    int distictVertices1 = check1.Except(check0).First();
                    int[] boundary = new int[] { sharedVertices[0], distictVertices0, sharedVertices[1], distictVertices1 };

                    MeshNgon ngon = MeshNgon.Create(boundary, connectedFaces[i]);
                    mesh.Ngons.AddNgon(ngon);
                    //Rhino.RhinoApp.WriteLine(distictVertices0.ToString() + " " + distictVertices1.ToString() + " " + sharedVertices[0].ToString()+ " " + sharedVertices[1].ToString() );
                } else {
                    if (connectedFaces[i].Length > 2) {
                        int center = -1;

                        if (check0.Contains(mesh.Faces[connectedFaces[i][2]].A) && check1.Contains(mesh.Faces[connectedFaces[i][2]].A))
                            center = mesh.Faces[connectedFaces[i][2]].A;
                        else if (check0.Contains(mesh.Faces[connectedFaces[i][2]].B) && check1.Contains(mesh.Faces[connectedFaces[i][2]].B))
                            center = mesh.Faces[connectedFaces[i][2]].B;
                        else if (check0.Contains(mesh.Faces[connectedFaces[i][2]].C) && check1.Contains(mesh.Faces[connectedFaces[i][2]].C))
                            center = mesh.Faces[connectedFaces[i][2]].C;

                        center = mesh.TopologyVertices.TopologyVertexIndex(center);

                        List<int> connectedTPVertices = mesh.TopologyVertices.ConnectedTopologyVertices(center, true).ToList();
                        //for(int j = 0; j < connectedTPVertices.Length;j++)
                        //connectedTPVertices[j] = mesh.TopologyVertices.MeshVertexIndices(connectedTPVertices[j])[0];

                        if (connectedTPVertices.Count != 8)
                            connectedTPVertices.Add(center);

                        MeshNgon ngon = MeshNgon.Create(connectedTPVertices, connectedFaces[i]);
                        mesh.Ngons.AddNgon(ngon);
                    }
                }
            }
            mesh.Flip(true, true, true);

            //  A = p;

            //Print(mesh.TopologyVertices.Count.ToString());
            //  Print(mesh.Vertices.Count.ToString());

            return mesh;
        }

        /// <summary>
        /// Subdivide nurbs surface to hexagon ngon mesh
        /// </summary>
        /// <param name="surface"></param>
        /// <param name="num"></param>
        /// <param name="num2"></param>
        /// <param name="num3"></param>
        /// <param name="trim"></param>
        /// <returns></returns>
        public static Mesh HexMesh(Surface surface, int num, int num2, double num3, int trim, ref List<double> distA, ref List<double> distB, bool skipOneSide = true, bool quad = false) {
            //Create Mesh and get face
            Mesh mesh = new Mesh();

            distA = new List<double>();
            distB = new List<double>();

            double num6 = 1.0 / (double)num;
            double num7 = 1.0 / (double)num2;
            double num8 = num3;

            int k = 0;
            int f = 0;

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
                                item = surface.PointAt((double)(i - 1) * num6, (double)j * num7);
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

                        if (i > 0 & j > 0 & i < num & j <= num2 - 1) {
                            mesh.Vertices.Add(item);
                            mesh.Vertices.Add(item2);
                            mesh.Vertices.Add(item4);
                            mesh.Vertices.Add(item6);
                            mesh.Vertices.Add(item5);
                            mesh.Vertices.Add(item7);

                            //distA.Add(new Line(item4, item7));
                            //distB.Add(new Line((item + item2) * 0.5, (item6 + item5) * 0.5));
                            distA.Add(item4.DistanceTo(item7));
                            distB.Add(((item + item2) * 0.5).DistanceTo(((item6 + item5) * 0.5)));

                            if (quad) {
                                /*
                                mesh.Faces.AddFace(k + 2, k + 1, k + 0);
                                mesh.Faces.AddFace(k + 5, k + 2, k + 0);
                                mesh.Faces.AddFace(k + 4, k + 3, k + 2);
                                mesh.Faces.AddFace(k + 4, k + 2, k + 5);

                                MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0, f + 1 });
                                MeshNgon ngon1 = MeshNgon.Create(new[] {  k + 2, k + 3, k + 4, k + 5 }, new[] { f + 2, f + 3 });
                                mesh.Ngons.AddNgon(ngon0);
                                mesh.Ngons.AddNgon(ngon1);

                                k += 6;
                                f += 4;
                                 */

                                mesh.Faces.AddFace(k + 2, k + 1, k + 0, k + 5);
                                mesh.Faces.AddFace(k + 4, k + 3, k + 2, k + 5);

                                MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0 });
                                MeshNgon ngon1 = MeshNgon.Create(new[] { k + 2, k + 3, k + 4, k + 5 }, new[] { f + 1 });
                                mesh.Ngons.AddNgon(ngon0);
                                mesh.Ngons.AddNgon(ngon1);

                                k += 6;
                                f += 2;
                            } else {
                                mesh.Faces.AddFace(k + 2, k + 1, k + 0);
                                mesh.Faces.AddFace(k + 5, k + 2, k + 0);
                                mesh.Faces.AddFace(k + 4, k + 3, k + 2);
                                mesh.Faces.AddFace(k + 4, k + 2, k + 5);

                                MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3, k + 4, k + 5 }, new[] { f + 0, f + 1, f + 2, f + 3 });
                                mesh.Ngons.AddNgon(ngon);

                                k += 6;
                                f += 4;
                            }
                        } else if (trim == 0) {
                            if (skipOneSide) {
                                //Corners
                                if (i == 0 & j == 0) {
                                    mesh.Vertices.Add(item7);
                                    mesh.Vertices.Add(item);
                                    mesh.Vertices.Add(item6);
                                    mesh.Vertices.Add(item5);
                                    mesh.AddQuadNGons(ref k, ref f, quad);
                                }
                                //Corners
                                if ((i == 0 & j == num2) && num2 % 2 == 0) {
                                    mesh.Vertices.Add(item);
                                    mesh.Vertices.Add(item4);
                                    mesh.Vertices.Add(item6);
                                    mesh.Vertices.Add(item5);

                                    mesh.AddQuadNGons(ref k, ref f, quad);
                                }
                                //Corners

                                if ((i == num & j == 0) && num % 2 == 0) {
                                    mesh.Vertices.Add(item);
                                    mesh.Vertices.Add(item2);
                                    mesh.Vertices.Add(item6);
                                    mesh.Vertices.Add(item7);
                                    mesh.AddQuadNGons(ref k, ref f, quad);
                                }
                                //Corners
                                if (i == num & j == num2) {
                                    mesh.Vertices.Add(item);
                                    mesh.Vertices.Add(item2);
                                    mesh.Vertices.Add(item4);
                                    mesh.Vertices.Add(item7);
                                    mesh.AddQuadNGons(ref k, ref f, quad);
                                }
                            }
                            //Sides
                            if (i == 0 & j > 0 & j < num2) {
                                mesh.Vertices.Add(item7);
                                mesh.Vertices.Add(item4);
                                mesh.Vertices.Add(item6);
                                mesh.Vertices.Add(item5);
                                mesh.AddQuadNGons(ref k, ref f, quad);
                            }

                            //Sides
                            if (i == num & j > 0 & j < num2) {
                                mesh.Vertices.Add(item);
                                mesh.Vertices.Add(item2);
                                mesh.Vertices.Add(item4);
                                mesh.Vertices.Add(item7);
                                mesh.AddQuadNGons(ref k, ref f, quad);
                            }
                            if (skipOneSide) {
                                //Sides
                                if (j == 0 & i > 0 & i < num) {
                                    if (quad) {
                                        mesh.Vertices.Add(item);
                                        mesh.Vertices.Add(item2);
                                        mesh.Vertices.Add(0.5 * (item2 + item6));
                                        mesh.Vertices.Add(item6);
                                        mesh.Vertices.Add(item5);
                                        mesh.Vertices.Add(item7);

                                        /*
                                        mesh.Faces.AddFace(k + 2, k + 1, k + 0);
                                        mesh.Faces.AddFace(k + 5, k + 2, k + 0);
                                        mesh.Faces.AddFace(k + 4, k + 3, k + 2);
                                        mesh.Faces.AddFace(k + 4, k + 2, k + 5);

                                        //MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3, k + 4, k + 5 }, new[] { f + 0, f + 1, f + 2, f + 3 });
                                        //mesh.Ngons.AddNgon(ngon);

                                        MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0, f + 1 });
                                            MeshNgon ngon1 = MeshNgon.Create(new[] { k + 2, k + 3, k + 4, k + 5 }, new[] { f + 2, f + 3 });
                                            mesh.Ngons.AddNgon(ngon0);
                                            mesh.Ngons.AddNgon(ngon1);

                                        k += 6;
                                        f += 4;
                                        */

                                        mesh.Faces.AddFace(k + 2, k + 1, k + 0, k + 5);
                                        mesh.Faces.AddFace(k + 4, k + 3, k + 2, k + 5);

                                        MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0 });
                                        MeshNgon ngon1 = MeshNgon.Create(new[] { k + 2, k + 3, k + 4, k + 5 }, new[] { f + 1 });
                                        mesh.Ngons.AddNgon(ngon0);
                                        mesh.Ngons.AddNgon(ngon1);

                                        k += 6;
                                        f += 2;
                                    } else {
                                        mesh.Vertices.Add(item5);
                                        mesh.Vertices.Add(item7);
                                        mesh.Vertices.Add(item);
                                        mesh.Vertices.Add(item2);

                                        mesh.Vertices.Add(item6);

                                        mesh.Faces.AddFace(k + 4, k + 1, k + 0);
                                        mesh.Faces.AddFace(k + 3, k + 2, k + 1);
                                        mesh.Faces.AddFace(k + 3, k + 1, k + 4);

                                        MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3, k + 4 },
                                            new[] { f + 0, f + 1, f + 2 });
                                        mesh.Ngons.AddNgon(ngon);
                                        k += 5;
                                        f += 3;
                                    }
                                }

                                //Sides
                                if (j == num2 & i > 0 & i < num) {
                                    if (quad) {
                                        mesh.Vertices.Add(item);
                                        mesh.Vertices.Add(item2);
                                        mesh.Vertices.Add(item4);
                                        mesh.Vertices.Add(item6);
                                        mesh.Vertices.Add(item5);
                                        mesh.Vertices.Add(0.5 * (item5 + item));

                                        /*
                                        mesh.Faces.AddFace(k + 2, k + 1, k + 0);
                                        mesh.Faces.AddFace(k + 5, k + 2, k + 0);
                                        mesh.Faces.AddFace(k + 4, k + 3, k + 2);
                                        mesh.Faces.AddFace(k + 4, k + 2, k + 5);

                                        // MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3, k + 4, k + 5 }, new[] { f + 0, f + 1, f + 2, f + 3 });
                                        //mesh.Ngons.AddNgon(ngon);

                                        MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0, f + 1 });
                                        MeshNgon ngon1 = MeshNgon.Create(new[] { k + 2, k + 3, k + 4, k + 5 }, new[] { f + 2, f + 3 });
                                        mesh.Ngons.AddNgon(ngon0);
                                        mesh.Ngons.AddNgon(ngon1);

                                        k += 6;
                                        f += 4;
                                        */

                                        mesh.Faces.AddFace(k + 2, k + 1, k + 0, k + 5);
                                        mesh.Faces.AddFace(k + 4, k + 3, k + 2, k + 5);

                                        MeshNgon ngon0 = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 5 }, new[] { f + 0 });
                                        MeshNgon ngon1 = MeshNgon.Create(new[] { k + 2, k + 3, k + 4, k + 5 }, new[] { f + 1 });
                                        mesh.Ngons.AddNgon(ngon0);
                                        mesh.Ngons.AddNgon(ngon1);

                                        k += 6;
                                        f += 2;
                                    } else {
                                        mesh.Vertices.Add(item2);
                                        mesh.Vertices.Add(item4);
                                        mesh.Vertices.Add(item6);
                                        mesh.Vertices.Add(item5);
                                        mesh.Vertices.Add(item);

                                        mesh.Faces.AddFace(k + 4, k + 1, k + 0);
                                        mesh.Faces.AddFace(k + 3, k + 2, k + 1);
                                        mesh.Faces.AddFace(k + 3, k + 1, k + 4);

                                        MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3, k + 4 },
                                            new[] { f + 0, f + 1, f + 2 });
                                        mesh.Ngons.AddNgon(ngon);
                                        k += 5;
                                        f += 3;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //mesh.RebuildNormals();
            //mesh.Clean();
            if (num3 == 1) {
                var allPlines = mesh.GetPolylines();
                List<Polyline> plines = new List<Polyline>();
                foreach (Polyline x in allPlines) {
                    Polyline pline = new Polyline(x);
                    pline.CollapseShortSegments(0.01);

                    switch (pline.Count - 1) {
                        case (6):
                            plines.Add(new Polyline(new[] { pline[0], pline[5], pline[1], pline[0] }));

                            plines.Add(new Polyline(new[] { pline[2], pline[4], pline[3], pline[2] }));

                            break;

                        case (5):
                            plines.Add(new Polyline(new[] { pline[0], pline[4], pline[1], pline[0] }));
                            plines.Add(new Polyline(new[] { pline[3], pline[2], pline[1], pline[3] }));
                            break;

                        default:
                            pline.Reverse();
                            plines.Add(pline);
                            break;
                    }
                }
                mesh = MeshFromPolylines(plines, 0.01);
            }
            return mesh;
        }

        public static void AddQuadNGons(this Mesh mesh, ref int k, ref int f, bool quad = false) {
            if (quad) {
                mesh.Faces.AddFace(k + 3, k + 2, k + 1, k + 0);

                MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3 }, new[] { f + 0 });
                mesh.Ngons.AddNgon(ngon);
                k += 4;
                f += 1;
            } else {
                mesh.Faces.AddFace(k + 2, k + 1, k + 0);
                mesh.Faces.AddFace(k + 0, k + 3, k + 2);

                MeshNgon ngon = MeshNgon.Create(new[] { k + 0, k + 1, k + 2, k + 3 }, new[] { f + 0, f + 1 });
                mesh.Ngons.AddNgon(ngon);
                k += 4;
                f += 2;
            }
        }

        public static void AddTrianglesNGons(this Mesh mesh, Point3d p0, Point3d p1, Point3d p2, ref int v, ref int f) {
            // Corner
            mesh.Vertices.Add(p0);
            mesh.Vertices.Add(p1);
            mesh.Vertices.Add(p2);

            mesh.Faces.AddFace(v + 2, v + 1, v + 0);

            MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2 }, new[] { f + 0, });
            mesh.Ngons.AddNgon(ngon);
            v += 3;
            f += 1;
        }

        public static Mesh QuadMesh(Surface surface, int num, int num2, bool triangulate = false, bool stellate = false, bool clean = true) {
            Mesh mesh = new Mesh();

            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            double num5 = 1.0 / (double)num;
            double num6 = 1.0 / (double)num2;

            int v = 0;
            int f = 0;

            for (int i = 0; i < num; i++) {
                for (int j = 0; j < num2; j++) {
                    Point3d a = surface.PointAt((double)i * num5, (double)j * num6);
                    Point3d b = surface.PointAt((double)(i + 1) * num5, (double)j * num6);
                    Point3d c = surface.PointAt((double)(i + 1) * num5, (double)(j + 1) * num6);
                    Point3d d = surface.PointAt((double)i * num5, (double)(j + 1) * num6);

                    if (stellate) {
                        mesh.Vertices.Add(a);
                        mesh.Vertices.Add(b);
                        mesh.Vertices.Add(c);
                        mesh.Vertices.Add(d);
                        mesh.Vertices.Add(PointUtil.AveragePoint(new Point3d[] { a, b, c, d }));

                        mesh.Faces.AddFace(v + 4, v + 0, v + 1);
                        mesh.Faces.AddFace(v + 4, v + 3, v + 0);
                        mesh.Faces.AddFace(v + 4, v + 2, v + 3);
                        mesh.Faces.AddFace(v + 4, v + 1, v + 2);

                        MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2, v + 3 }, new[] { f + 0, f + 1, f + 2, f + 3 });
                        mesh.Ngons.AddNgon(ngon);
                        v += 5;
                        f += 4;
                    } else {
                        int flag = -1;

                        if (a.DistanceToSquared(b) < 0.0001)
                            flag = 0;
                        if (b.DistanceToSquared(c) < 0.0001)
                            flag = 1;
                        if (c.DistanceToSquared(d) < 0.0001)
                            flag = 2;
                        if (d.DistanceToSquared(a) < 0.0001)
                            flag = 3;

                        switch (flag) {
                            case (0):
                                mesh.Vertices.Add(b);
                                mesh.Vertices.Add(c);
                                mesh.Vertices.Add(d);
                                break;

                            case (1):
                                mesh.Vertices.Add(a);
                                mesh.Vertices.Add(c);
                                mesh.Vertices.Add(d);
                                break;

                            case (2):
                                mesh.Vertices.Add(a);
                                mesh.Vertices.Add(b);
                                mesh.Vertices.Add(d);
                                break;

                            case (3):
                                mesh.Vertices.Add(a);
                                mesh.Vertices.Add(b);
                                mesh.Vertices.Add(c);
                                break;

                            default:
                                mesh.Vertices.Add(a);
                                mesh.Vertices.Add(b);
                                mesh.Vertices.Add(c);
                                mesh.Vertices.Add(d);
                                break;
                        }

                        if (flag == -1) {
                            if (triangulate) {
                                int face0 = mesh.Faces.AddFace(v + 2, v + 1, v + 0);
                                int face1 = mesh.Faces.AddFace(v + 3, v + 2, v + 0);

                                MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2, v + 3 }, new[] { f + 0, f + 1 });
                                mesh.Ngons.AddNgon(ngon);
                                v += 4;
                                f += 2;
                            } else {
                                mesh.Faces.AddFace(v + 3, v + 2, v + 1, v + 0);

                                MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2, v + 3 }, new[] { f });
                                mesh.Ngons.AddNgon(ngon);
                                v += 4;
                                f++;
                            }
                        } else {
                            int face0 = mesh.Faces.AddFace(v + 2, v + 1, v + 0);

                            MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2 }, new[] { f + 0 });
                            mesh.Ngons.AddNgon(ngon);
                            v += 3;
                            f += 1;
                        }
                    }
                }
            }

            if (clean) {
                mesh.Clean();
                if (mesh.SolidOrientation() == -1)
                    mesh.Flip(true, true, true);
                mesh.WeldUsingRTree(0.001);
            }
            return mesh;
        }

        public static Mesh QuadMeshDistanceFromBrep(Surface surface, int num, int num2, Brep target, double min, double max, double shift, ref double[] distances) {
            Mesh mesh = new Mesh();

            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            double num5 = 1.0 / (double)num;
            double num6 = 1.0 / (double)num2;

            int v = 0;
            int f = 0;

            for (int i = 0; i < num; i++) {
                for (int j = 0; j < num2; j++) {
                    Point3d pa = surface.PointAt((double)i * num5, (double)j * num6);
                    Point3d pb = surface.PointAt((double)(i + 1) * num5, (double)j * num6);
                    Point3d pc = surface.PointAt((double)(i + 1) * num5, (double)(j + 1) * num6);
                    Point3d pd = surface.PointAt((double)i * num5, (double)(j + 1) * num6);

                    mesh.Vertices.Add(pa);
                    mesh.Vertices.Add(pb);
                    mesh.Vertices.Add(pc);
                    mesh.Vertices.Add(pd);

                    int test0 = mesh.Faces.AddFace(v + 2, v + 1, v + 0);
                    int test1 = mesh.Faces.AddFace(v + 3, v + 2, v + 0);
                    //int test1 = mesh.Faces.AddFace(v + 3, v + 2, v + 1, v + 0);

                    MeshNgon ngon = MeshNgon.Create(new[] { v + 0, v + 1, v + 2, v + 3 }, new[] { f + 0, f + 1 });//
                    mesh.Ngons.AddNgon(ngon);
                    v += 4;
                    f += 1;
                }
            }

            mesh.Vertices.CombineIdentical(true, true);
            mesh.FaceNormals.ComputeFaceNormals();
            mesh.Normals.ComputeNormals();
            mesh.Compact();

            //Assign colors

            distances = new double[mesh.Vertices.Count];

            //var gradient = ColorUtil.Gh2ColorsGradient(Color.White, Color.Black);
            var gradient = ColorUtil.Gh3ColorsGradient(Color.White, Color.Red, Color.Black, 0, 0.5, 1);

            for (int i = 0; i < mesh.Vertices.Count; i++) {
                double dist = target.ClosestPoint(mesh.Vertices[i]).DistanceTo(mesh.Vertices[i]);
                dist -= shift;
                distances[i] = dist;
                dist = MathUtil.Constrain(dist, min, max);

                // int temp = MathUtil.MapInt(dist, min, max, 0, 255);
                //mesh.VertexColors.Add(Color.FromArgb(255 - temp, 255 - temp, 255 - temp));

                double temp = MathUtil.Map(dist, min, max, 0, 1);
                mesh.VertexColors.Add(gradient.ColourAt(temp));
            }

            mesh.Clean();

            return mesh;
        }

        public static Mesh DiamondMesh(Surface surface, int num, int num2, int T, bool flip = false) {
            List<int> e0 = new List<int>();
            List<int> e1 = new List<int>();
            List<int> e2 = new List<int>();
            List<int> e3 = new List<int>();

            //Base Mesh
            Mesh mesh = new Mesh();

            //edge meshes and corner meshes
            Mesh em00 = new Mesh();
            Mesh em01 = new Mesh();
            Mesh em10 = new Mesh();
            Mesh em11 = new Mesh();

            List<Point3d> em00AdditionalPoint = new List<Point3d>();
            List<Point3d> em01AdditionalPoint = new List<Point3d>();
            List<Point3d> em10AdditionalPoint = new List<Point3d>();
            List<Point3d> em11AdditionalPoint = new List<Point3d>();

            Mesh cm00 = new Mesh();
            Mesh cm01 = new Mesh();
            Mesh cm10 = new Mesh();
            Mesh cm11 = new Mesh();

            surface.SetDomain(0, new Interval(0.0, 1.0));
            surface.SetDomain(1, new Interval(0.0, 1.0));

            double num5 = 1.0 / (double)num;
            double num6 = 1.0 / (double)num2;

            int v = 0;
            int f = 0;

            for (int i = 0; i <= num; i++) {
                for (int j = 0; j <= num2; j++) {
                    if ((i + j) % 2 == 0) {
                        Point3d point3d;
                        Point3d point3d2;
                        Point3d point3d3;
                        Point3d point3d4;

                        if (i > 0)
                            point3d = surface.PointAt((double)(checked(i - 1)) * num5, (double)j * num6);
                        else
                            point3d = surface.PointAt((double)i * num5, (double)j * num6);

                        if (j > 0)
                            point3d2 = surface.PointAt((double)i * num5, (double)(checked(j - 1)) * num6);
                        else
                            point3d2 = surface.PointAt((double)i * num5, (double)j * num6);

                        if (i < num)
                            point3d3 = surface.PointAt((double)(checked(i + 1)) * num5, (double)j * num6);
                        else
                            point3d3 = surface.PointAt((double)i * num5, (double)j * num6);

                        if (j <= checked(num2 - 1))
                            point3d4 = surface.PointAt((double)i * num5, (double)(checked(j + 1)) * num6);
                        else
                            point3d4 = surface.PointAt((double)i * num5, (double)j * num6);

                        if (i > 0 & j > 0 & i < num & j <= num2 - 1) {
                            //Inner faces
                            if (flip) {
                                mesh.Vertices.Add(point3d4);
                                mesh.Vertices.Add(point3d);
                                mesh.Vertices.Add(point3d2);
                                mesh.Vertices.Add(point3d3);
                            } else {
                                mesh.Vertices.Add(point3d);
                                mesh.Vertices.Add(point3d2);
                                mesh.Vertices.Add(point3d3);
                                mesh.Vertices.Add(point3d4);
                            }

                            mesh.Faces.AddFace(v + 2, v + 1, v + 0);
                            mesh.Faces.AddFace(v + 3, v + 2, v + 0);

                            MeshNgon ngon = MeshNgon.Create(new[] { v + 1, v + 0, v + 3, v + 2, }, new[] { f + 0, f + 1 });
                            mesh.Ngons.AddNgon(ngon);
                            v += 4;
                            f += 2;
                        } else if (T == 0) {
                            if (i == 0 & j == 0) {
                                //Corner
                                cm00.Vertices.Add(point3d2);
                                cm00.Vertices.Add(point3d3);
                                cm00.Vertices.Add(point3d4);
                                //e0.Add(mesh.Ngons.Count());
                                //e2.Add(mesh.Ngons.Count());
                                // Rhino.RhinoApp.WriteLine("e2 " + e2.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d2, point3d3, point3d4, ref v, ref f);
                            }
                            if (i == 0 & j == num2) {
                                //Corner
                                cm01.Vertices.Add(point3d2);
                                cm01.Vertices.Add(point3d3);
                                cm01.Vertices.Add(point3d4);
                                //e0.Add(mesh.Ngons.Count());
                                //e3.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e3 " + e3.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d2, point3d3, point3d4, ref v, ref f);
                            }
                            if (i == num & j == 0) {
                                //Corner
                                cm10.Vertices.Add(point3d);
                                cm10.Vertices.Add(point3d3);
                                cm10.Vertices.Add(point3d4);
                                //e1.Add(mesh.Ngons.Count());
                                //e2.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e2 " + e2.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d, point3d3, point3d4, ref v, ref f);
                            }
                            if (i == num & j == num2) {
                                //Corner
                                cm11.Vertices.Add(point3d);
                                cm11.Vertices.Add(point3d2);
                                cm11.Vertices.Add(point3d4);
                                //e1.Add(mesh.Ngons.Count());
                                //e3.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e3 " + e3.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d, point3d2, point3d4, ref v, ref f);
                            }
                            if (i == 0 & j > 0 & j < num2) {
                                //Side
                                em00AdditionalPoint.Add(point3d);
                                em00.Vertices.Add(point3d4);
                                em00.Vertices.Add(point3d2);
                                em00.Vertices.Add(point3d3);

                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(new Polyline(new Point3d[] { point3d3, point3d4, point3d2 }));

                                //e0.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e0 " + e0.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d2, point3d3, point3d4, ref v, ref f);
                            }
                            if (i == num & j > 0 & j < num2) {
                                //Side
                                em01AdditionalPoint.Add(point3d3);
                                em01.Vertices.Add(point3d4);
                                em01.Vertices.Add(point3d);
                                em01.Vertices.Add(point3d2);

                                //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(new Polyline(new Point3d[] { point3d4, point3d2, point3d }));
                                //e1.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e1 " + e1.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d, point3d2, point3d4, ref v, ref f);
                            }
                            if (j == 0 & i > 0 & i < num) {
                                //Side
                                em10AdditionalPoint.Add(point3d2);
                                em10.Vertices.Add(point3d);
                                em10.Vertices.Add(point3d3);
                                em10.Vertices.Add(point3d4);
                                //e2.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e2 " + e2.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d, point3d3, point3d4, ref v, ref f);
                            }
                            if (j == num2 & i > 0 & i < num) {
                                //Side
                                em11AdditionalPoint.Add(point3d4);
                                em11.Vertices.Add(point3d);
                                em11.Vertices.Add(point3d2);
                                em11.Vertices.Add(point3d3);
                                //e3.Add(mesh.Ngons.Count());
                                //Rhino.RhinoApp.WriteLine("e3 " + e3.Last().ToString());
                                //AddTrianglesNGons(mesh, point3d, point3d2, point3d3, ref v, ref f);
                            }
                        }
                    }
                }
            }

            //Deal with edge and corners
            int edgeConnection = surface.GetEdgeConnectivity(num, num2);
            //if (num % 2 != 0 || num2 % 2 != 0)
            //edgeConnection = 0;
            //Rhino.RhinoApp.WriteLine("econn" + edgeConnection.ToString());
            int n = 0;

            if (edgeConnection == 1 || edgeConnection == 11) {
                n = em00.Vertices.Count;
                em00.Append(em01);

                for (int i = 0; i < n; i += 3) {
                    if (!flip) {
                        em00.Faces.AddFace(i + 2, i + 1, i + 1 + n);
                        em00.Faces.AddFace(i + 1 + n, i, i + 2);
                        em00.Ngons.AddNgon(MeshNgon.Create((new int[] { i, i + 2, i + 1, i + 1 + n }).Shift(2), new int[] { em00.Faces.Count - 2, em00.Faces.Count - 1 }));
                    } else {
                        em00.Faces.AddFace(i + 2, i + 1, i);
                        em00.Faces.AddFace(i + 1, i + 1 + n, i);
                        em00.Ngons.AddNgon(MeshNgon.Create(new int[] { i, i + 2, i + 1, i + 1 + n }.Shift(1), new int[] { em00.Faces.Count - 2, em00.Faces.Count - 1 }));
                    }
                }

                mesh.Append(em00);
            }
            if (edgeConnection == 10 || edgeConnection == 11) {
                n = em10.Vertices.Count;
                em10.Append(em11);

                for (int i = 0; i < n; i += 3) {
                    if (flip) {
                        em10.Faces.AddFace(i + 1, i + 1 + n, i + 2);
                        em10.Faces.AddFace(i + 1 + n, i, i + 2);
                        em10.Ngons.AddNgon(MeshNgon.Create(new int[] { i, i + 2, i + 1, i + 1 + n }, new int[] { em10.Faces.Count - 2, em10.Faces.Count - 1 }));
                    } else {
                        em10.Faces.AddFace(i + 2, i + 1, i);
                        em10.Faces.AddFace(i + 1, i + 1 + n, i);
                        em10.Ngons.AddNgon(MeshNgon.Create(new int[] { i, i + 2, i + 1, i + 1 + n }, new int[] { em10.Faces.Count - 2, em10.Faces.Count - 1 }));
                    }
                }
                mesh.Append(em10);
            }
            if (edgeConnection == 10 || edgeConnection == 0) {
                n = em00.Vertices.Count;
                bool flag = n != em01.Vertices.Count;
                em00.Append(em01);

                int j = 0;
                for (int i = 0; i < n + (Convert.ToInt32(flag)); i += 3) {//+(Convert.ToInt32(flag))
                    if (flip) {
                        if (i < n) {
                            em00.Faces.AddFace(i + 2, i + 1, i + 0);
                            em00.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 0, i + 1, i + 2 }), new int[] { em00.Faces.Count - 1 }));
                        }
                        em00.Faces.AddFace(i + 2 + n, i + 1 + n, i + 0 + n);
                        em00.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 2 + n, i + 1 + n, i + 0 + n }), new int[] { em00.Faces.Count - 1 }));
                    } else {
                        if (i < n) {
                            em00.Vertices.Add(em00AdditionalPoint[j]);
                            em00.Faces.AddFace(i + 2, i + 1, em00.Vertices.Count - 1);
                            em00.Faces.AddFace(i + 0, i + 2, em00.Vertices.Count - 1);
                            em00.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 1, em00.Vertices.Count - 1, i + 0, i + 2 }), new int[] { em00.Faces.Count - 1, em00.Faces.Count - 2 }));
                        }

                        em00.Vertices.Add(em01AdditionalPoint[j]);
                        em00.Faces.AddFace(i + 2 + n, i + 1 + n, em00.Vertices.Count - 1);
                        em00.Faces.AddFace(i + 0 + n, em00.Vertices.Count - 1, i + 1 + n);
                        em00.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 2 + n, i + 1 + n, i + 0 + n, em00.Vertices.Count - 1 }), new int[] { em00.Faces.Count - 1, em00.Faces.Count - 2 }));

                        j++;
                    }
                }
                mesh.Append(em00);
            }
            if (edgeConnection == 1 || edgeConnection == 0) {
                n = em10.Vertices.Count;
                em10.Append(em11);
                bool flag = n != em11.Vertices.Count;

                int j = 0;
                for (int i = 0; i < n + (Convert.ToInt32(flag)); i += 3) {//+(Convert.ToInt32(flag))
                    if (!flip) {
                        if (i < n) {
                            em10.Faces.AddFace(i + 2, i + 1, i + 0);
                            em10.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 2, i + 1, i + 0 }), new int[] { em10.Faces.Count - 1 }));
                        }
                        em10.Faces.AddFace(i + 2 + n, i + 1 + n, i + 0 + n);
                        em10.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 2 + n, i + 1 + n, i + 0 + n }), new int[] { em10.Faces.Count - 1 }));
                    } else {
                        if (i < n) {
                            em10.Vertices.Add(em10AdditionalPoint[j]);
                            em10.Faces.AddFace(i + 1, em10.Vertices.Count - 1, i + 2);
                            em10.Faces.AddFace(em10.Vertices.Count - 1, i + 0, i + 2);
                            em10.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 0, i + 2, i + 1, em10.Vertices.Count - 1, }), new int[] { em10.Faces.Count - 1, em10.Faces.Count - 2 }));
                        }

                        em10.Vertices.Add(em11AdditionalPoint[j]);
                        em10.Faces.AddFace(i + 2 + n, i + 1 + n, em10.Vertices.Count - 1);
                        em10.Faces.AddFace(i + 1 + n, i + 0 + n, em10.Vertices.Count - 1);
                        em10.Ngons.AddNgon(MeshNgon.Create((new int[] { i + 0 + n, em10.Vertices.Count - 1, i + 2 + n, i + 1 + n }), new int[] { em10.Faces.Count - 1, em10.Faces.Count - 2 }));

                        j++;
                    }
                }
                mesh.Append(em10);
            }

            //Corners

            if (edgeConnection == 11) {
                if (cm00.Vertices.Count == 3 && cm11.Vertices.Count == 3)
                    if (flip) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm00.Vertices[1]);
                        mesh.Vertices.Add(cm00.Vertices[2]);
                        mesh.Vertices.Add(cm11.Vertices[0]);
                        mesh.Vertices.Add(cm11.Vertices[1]);
                        mesh.Faces.AddFace(n + 0, n + 3, n + 1);
                        mesh.Faces.AddFace(n + 3, n + 2, n + 1);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 2, n + 1, n + 0, n + 3 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    } else {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm00.Vertices[2]);
                        mesh.Vertices.Add(cm11.Vertices[0]);
                        mesh.Vertices.Add(cm11.Vertices[1]);
                        mesh.Vertices.Add(cm00.Vertices[1]);
                        mesh.Faces.AddFace(n + 0, n + 3, n + 1);
                        mesh.Faces.AddFace(n + 3, n + 2, n + 1);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 2, n + 1, n + 0, n + 3 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    }
            } else if (edgeConnection == 1) {
                if (flip) {
                    if (cm00.Vertices.Count > 0 && cm10.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm00.Vertices[0]);
                        mesh.Vertices.Add(cm00.Vertices[1]);
                        mesh.Vertices.Add(cm10.Vertices[0]);
                        mesh.Vertices.Add(cm10.Vertices[2]);
                        mesh.Faces.AddFace(n + 1, n + 0, n + 3);
                        mesh.Faces.AddFace(n + 0, n + 2, n + 3);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 2, n + 3, n + 1, n + 0 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    }
                    if (cm01.Vertices.Count > 0 && cm11.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm01.Vertices[0]);
                        mesh.Vertices.Add(cm01.Vertices[1]);
                        mesh.Vertices.Add(cm11.Vertices[0]);
                        mesh.Vertices.Add(cm11.Vertices[2]);

                        mesh.Faces.AddFace(n + 0, n + 2, n + 3);
                        mesh.Faces.AddFace(n + 1, n + 0, n + 3);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 2, n + 3, n + 1, n + 0 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    }
                } else {
                    if (cm00.Vertices.Count > 0 && cm10.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        //mesh.Vertices.Add(cm00.Vertices[0]);
                        mesh.Vertices.Add(cm00.Vertices[1]);
                        mesh.Vertices.Add(cm10.Vertices[0]);
                        mesh.Vertices.Add(cm10.Vertices[2]);
                        mesh.Faces.AddFace(n + 2, n + 0, n + 1);

                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 2, n + 0, n + 1 }), new int[] { mesh.Faces.Count - 1 }));
                    }
                    if (cm01.Vertices.Count > 0 && cm11.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm01.Vertices[0]);
                        mesh.Vertices.Add(cm01.Vertices[1]);
                        mesh.Vertices.Add(cm11.Vertices[0]);
                        //mesh.Vertices.Add(cm11.Vertices[2]);
                        //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(cm01.Vertices[0]);

                        mesh.Faces.AddFace(n + 1, n + 0, n + 2);

                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 1, n + 0, n + 2 }), new int[] { mesh.Faces.Count - 1 }));
                    }
                }
            } else if (edgeConnection == 10) {
                if (flip) {
                    if (cm00.Vertices.Count > 0 && cm01.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm00.Vertices[0]);
                        mesh.Vertices.Add(cm00.Vertices[2]);
                        mesh.Vertices.Add(cm01.Vertices[0]);
                        mesh.Vertices.Add(cm01.Vertices[1]);
                        //mesh.Faces.AddFace(n + 2, n + 0, n + 1);
                        mesh.Faces.AddFace(n + 3, n + 2, n + 1);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 1, n + 3, n + 2 }), new int[] { mesh.Faces.Count - 1 }));
                    }
                    if (cm10.Vertices.Count > 0 && cm11.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm10.Vertices[0]);
                        mesh.Vertices.Add(cm10.Vertices[2]);
                        mesh.Vertices.Add(cm11.Vertices[1]);
                        mesh.Vertices.Add(cm11.Vertices[2]);
                        //mesh.Faces.AddFace(n + 3, n + 2, n + 0);
                        mesh.Faces.AddFace(n + 2, n + 0, n + 1);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 1, n + 2 }), new int[] { mesh.Faces.Count - 1 }));
                    }
                } else {
                    if (cm00.Vertices.Count > 0 && cm01.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm00.Vertices[0]);
                        mesh.Vertices.Add(cm00.Vertices[2]);
                        mesh.Vertices.Add(cm01.Vertices[0]);
                        mesh.Vertices.Add(cm01.Vertices[1]);
                        mesh.Faces.AddFace(n + 3, n + 2, n + 0);
                        mesh.Faces.AddFace(n + 1, n + 3, n + 0);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 1, n + 3, n + 2 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    }

                    if (cm10.Vertices.Count > 0 && cm11.Vertices.Count > 0) {
                        n = mesh.Vertices.Count;
                        mesh.Vertices.Add(cm10.Vertices[0]);
                        mesh.Vertices.Add(cm10.Vertices[2]);
                        mesh.Vertices.Add(cm11.Vertices[1]);
                        mesh.Vertices.Add(cm11.Vertices[2]);

                        mesh.Faces.AddFace(n + 3, n + 2, n + 0);
                        mesh.Faces.AddFace(n + 1, n + 3, n + 0);
                        mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 1, n + 3, n + 2 }), new int[] { mesh.Faces.Count - 1, mesh.Faces.Count - 2 }));
                    }
                }
            } else if (edgeConnection == 0) {
                if (cm00.Vertices.Count > 0) {
                    n = mesh.Vertices.Count;
                    mesh.Vertices.AddVertices(cm00.Vertices);
                    mesh.Faces.AddFace(n + 2, n + 1, n + 0);
                    mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 2, n + 1 }), new int[] { mesh.Faces.Count - 1 }));
                }
                if (cm01.Vertices.Count > 0) {
                    n = mesh.Vertices.Count;
                    mesh.Vertices.AddVertices(cm01.Vertices);
                    mesh.Faces.AddFace(n + 1, n + 0, n + 2);
                    mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 2, n + 1 }), new int[] { mesh.Faces.Count - 1 }));
                }
                if (cm10.Vertices.Count > 0) {
                    n = mesh.Vertices.Count;
                    mesh.Vertices.AddVertices(cm10.Vertices);
                    mesh.Faces.AddFace(n + 2, n + 1, n + 0);
                    mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 2, n + 1 }), new int[] { mesh.Faces.Count - 1 }));
                }
                if (cm11.Vertices.Count > 0) {
                    n = mesh.Vertices.Count;
                    mesh.Vertices.AddVertices(cm11.Vertices);
                    mesh.Faces.AddFace(n + 1, n + 0, n + 2);
                    mesh.Ngons.AddNgon(MeshNgon.Create((new int[] { n + 0, n + 2, n + 1 }), new int[] { mesh.Faces.Count - 1 }));
                }
            }

            mesh.Clean();

            return mesh;
        }

        /// <summary>
        /// uses the simplest Triangle .NET function of 2D triangulation.
        /// First polyline is taken as main curves, other polylines are holes.
        /// all curves are projected to 2D plane, triangulated and projected back to 3D
        /// </summary>
        /// <param name="polylines"></param>
        /// <returns></returns>
        //public static Rhino.Geometry.Mesh MeshFromClosedPolylineWithHoles(Polyline[] _polylines) {
        //    Rhino.Geometry.Mesh m = new Rhino.Geometry.Mesh();
        //    TriangleNet.Mesh tMesh;
        //    List<int> elements = new List<int>();
        //    List<Point3d> allPt = new List<Point3d>();
        //    Plane plane;
        //    List<int> holes = new List<int>();
        //    Polyline[] polylines = new Polyline[_polylines.Length];
        //    Polygon polygon = new Polygon();

        //    for (int i = 0; i < _polylines.Length; i++) {
        //        polylines[i] = new Polyline(_polylines[i]);
        //        polylines[i].Transform(Transform.Scale(Point3d.Origin, 1000));
        //    }

        //    if (polylines.Length == 0) return new Rhino.Geometry.Mesh();
        //    //Rhino.RhinoApp.WriteLine(_polylines.Length.ToString());

        //    int counter = 0;
        //    foreach (Polyline poly in polylines) {
        //        if (!poly.IsValid)
        //            return new Rhino.Geometry.Mesh();
        //        else if (counter > 0)
        //            holes.Add(counter);
        //        counter++;
        //    }

        //    Plane.FitPlaneToPoints(polylines[0], out plane);
        //    for (int i = 0; i < polylines.Length; i++) {
        //        allPt.AddRange(polylines[i].GetRange(0, polylines[i].Count - 1));
        //        if (i == 0) {
        //            Point3d center = PointUtil.AveragePoint(polylines[i]);
        //            plane.Origin = center;
        //        }
        //        polylines[i].Transform(Transform.plane_to_plane(plane, Plane.WorldXY));
        //    }

        //    for (int i = 0; i < polylines.Length; i++) {
        //        TriangleNet.Geometry.Vertex[] points = new TriangleNet.Geometry.Vertex[polylines[i].Count - 1];

        //        for (int j = 0; j < polylines[i].Count - 1; j++) {
        //            Point3d item = polylines[i][j];
        //            points[j] = new TriangleNet.Geometry.Vertex(item.X, item.Y);
        //        }

        //        if (!holes.Contains(i)) {
        //            polygon.Add(new Contour(points, i, true), false);
        //            polygon.Regions.Add(new RegionPointer(2.5, 0, i));
        //        } else {
        //            polygon.Add(new Contour(points, i, false), true);
        //            polygon.Regions.Add(new RegionPointer(1.5, 0, i));
        //        }

        //    }

        //    tMesh = (TriangleNet.Mesh)polygon.Triangulate(); //bug

        //    m.Vertices.AddVertices(allPt);

        //    // foreach (TriangleNet.Geometry.Vertex v in tMesh.Vertices) {
        //    // m.Vertices.Add(v.X, v.Y, 0);
        //    //}

        //    foreach (Triangle t in tMesh.Triangles) {
        //        m.Faces.AddFace(t.GetVertexID(0), t.GetVertexID(1), t.GetVertexID(2));
        //    }

        //    //m.Clean();
        //    m.Transform(Transform.Scale(Point3d.Origin,0.001));
        //    //m.Bake();
        //    return m;

        //}
        /*
        public static Rhino.Geometry.Mesh MeshFromClosedPolylineWithHoles(Polyline[] _polylines) {
            Polyline[] polylines = new Polyline[_polylines.Length];
            for (int i = 0; i < _polylines.Length; i++)
                polylines[i] = new Polyline(_polylines[i]);

            TriangleNet.Mesh tMesh;
            //Polyline p = null;
            //List<Curve> curves = new List<Curve>();
            //if (!DA.GetDataList<Curve>(0, curves)) {
            //return;
            //}

            if (polylines.Length == 0)
                return new Rhino.Geometry.Mesh();

            List<int> holes = new List<int>();
            //DA.GetDataList<int>(1, holes);

            int counter = 0;
            foreach (Polyline poly in polylines) {
                if (!poly.IsValid)
                    return new Rhino.Geometry.Mesh();
                else if (counter > 0)
                    holes.Add(counter);
                counter++;
            }

            List<Point3d> allPt = new List<Point3d>();

            Plane plane;
            Plane.FitPlaneToPoints(polylines[0], out plane);
            for (int i = 0; i < polylines.Length; i++) {
                allPt.AddRange(polylines[i].GetRange(0, polylines[i].Count-1));
                polylines[i].Transform(Transform.plane_to_plane(plane, Plane.WorldXY));
            }

            bool remesh = false;
            // DA.GetData<bool>(2, ref remesh);
            double minAngle = 25;
            //DA.GetData<double>(3, ref minAngle);
            double resolution = 10;
            //DA.GetData<double>(4, ref resolution);
            bool refine = false;
            //DA.GetData<bool>(5, ref refine);
            List<int> elements = new List<int>();
            //DA.GetDataList<int>(6, elements);
            double scale = 1;
            //DA.GetData<double>(7, ref scale);

            //List<Polyline> polylines = new List<Polyline>();
            //foreach (Curve curve in curves) {
            //    curve.TryGetPolyline(ref p);
            //    polylines.Add(p);
            //}
            Polygon polygon = new Polygon();
            for (int i = 0; i < polylines.Length; i++) {
                TriangleNet.Geometry.Vertex[] points = new TriangleNet.Geometry.Vertex[polylines[i].Count];
                for (int j = 0; j < polylines[i].Count; j++) {
                    Point3d item = polylines[i][j];
                    double x = item.X;
                    item = polylines[i][j];
                    points[j] = new TriangleNet.Geometry.Vertex(x, item.Y);
                }
                if (!holes.Contains(i)) {
                    polygon.Add(new Contour(points, i, true), false);
                    polygon.Regions.Add(new RegionPointer(2.5, 0, i));
                } else {
                    polygon.Add(new Contour(points, i, false), true);
                    polygon.Regions.Add(new RegionPointer(1.5, 0, i));
                }
            }
            ConstraintOptions options = new ConstraintOptions() {
                ConformingDelaunay = true,
                Convex = false,
                SegmentSplitting = 0
            };
            QualityOptions quality = new QualityOptions() {
                MinimumAngle = Math.Min(minAngle, 35),
                MaximumAngle = 0,
                MaximumArea = resolution * 2,
                VariableArea = true,
                SteinerPoints = 0
            };
            tMesh = (!remesh ? (TriangleNet.Mesh)polygon.Triangulate() : (TriangleNet.Mesh)polygon.Triangulate(options, quality));
            if (refine & remesh) {
                SimpleSmoother smoother = new SimpleSmoother();
                smoother.Smooth(tMesh);
                tMesh.Refine(quality, false);
                (new QualityMeasure()).Update(tMesh);
                double area = scale;
                foreach (Triangle t in tMesh.Triangles) {
                    if (!elements.Contains(t.Label)) {
                        continue;
                    }
                    t.Area = area;
                }
                quality.VariableArea = true;
                tMesh.Refine(quality, false);
                smoother.Smooth(tMesh);
            }
            //    ICollection<SubSegment> segments = tMesh.Segments;
            //    List<Line> lines = new List<Line>();
            //    foreach (SubSegment s in segments) {
            //      lines.Add(new Line(tMesh.Vertices.ElementAt<TriangleNet.Geometry.Vertex>(s.P0).X, tMesh.Vertices.ElementAt<TriangleNet.Geometry.Vertex>(s.P0).Y, 0, tMesh.Vertices.ElementAt<TriangleNet.Geometry.Vertex>(s.P1).X, tMesh.Vertices.ElementAt<TriangleNet.Geometry.Vertex>(s.P1).Y, 0));
            //    }

            Rhino.Geometry.Mesh m = new Rhino.Geometry.Mesh();
               // foreach (TriangleNet.Geometry.Vertex v in tMesh.Vertices) {
                 // m.Vertices.Add(v.X, v.Y, 0);
                //}
           m.Vertices.AddVertices(allPt);

            foreach (Triangle t in tMesh.Triangles) {
                m.Faces.AddFace(t.GetVertexID(0), t.GetVertexID(1), t.GetVertexID(2));
            }

           //   m.Transform(Transform.plane_to_plane(Plane.WorldXY, plane));

            m.Clean();

            return m;
            //DA.SetData(0, m);
            //DA.SetDataList(1, lines);
        }
        */

        public static Rhino.Geometry.Mesh LoftPolylineWithHoles(List<Polyline> outlines0, List<Polyline> outlines1, bool addNGons = false) {
            //if (outlines0[0].plane().IsSamePosition(outlines1[0].plane())) return null;

            try {
                List<Curve> curves0 = new List<Curve>();
                List<Curve> curves1 = new List<Curve>();

                for (int i = 0; i < outlines0.Count; i++) {
                    curves0.Add(outlines0[i].ToNurbsCurve());
                    curves1.Add(outlines1[i].ToNurbsCurve());
                }

                double len = -1;
                int id = -1;

                int counter = 0;
                double[] lens = new double[curves0.Count];
                foreach (Curve c in curves0) {
                    double tempLen = c.GetBoundingBox(true).Diagonal.Length;

                    if (tempLen > len) {
                        len = tempLen;
                        id = counter;
                    }
                    counter++;
                }

                Polyline pL0 = curves0[id].ToPolylineFromCP();
                Polyline pL1 = curves1[id].ToPolylineFromCP();

                List<Curve> crv0 = new List<Curve>();
                List<Curve> crv1 = new List<Curve>();
                for (int i = 0; i < curves0.Count; i++) {
                    if (i != id) {
                        crv0.Add(curves0[i]);
                        crv1.Add(curves1[i]);
                    }
                }

                Mesh mesh0 = Rhino.Geometry.Mesh.CreatePatch(pL0, 0.01, null, crv0, null, null, true, 1);

                //Math mesh vertices to polylines points
                Point3d[] ptsMesh = mesh0.Vertices.ToPoint3dArray();

                Point3d[] ptsPlines0 = new Point3d[ptsMesh.Length];
                Point3d[] ptsPlines1 = new Point3d[ptsMesh.Length];
                int count = 0;
                for (int i = 0; i < pL0.Count - 1; i++) {
                    ptsPlines0[count] = pL0[i];
                    ptsPlines1[count++] = pL1[i];
                }

                for (int i = 0; i < crv0.Count; i++) {
                    Polyline crv0Plines = crv0[i].ToPolyline();
                    Polyline crv1Plines = crv1[i].ToPolyline();
                    for (int j = 0; j < crv0Plines.Count - 1; j++) {
                        ptsPlines0[count] = crv0Plines[j];
                        ptsPlines1[count++] = crv1Plines[j];
                    }
                }

                //Search ids to match mesh v to crv v
                int[] cpID = ClosestPointUtil.RTreeSearchIDOnlyOne(ptsPlines0, ptsMesh, 0.01); //use dictionary search instead

                Mesh mesh1 = mesh0.DuplicateMesh();
                for (int i = 0; i < cpID.Length; i++) {
                    mesh1.Vertices.SetVertex(i, ptsPlines1[cpID[i]]);
                }

                //Create walls
                Mesh mesh = new Mesh();
                mesh.Append(mesh0);
                mesh.Append(mesh1);

                int n = mesh0.Vertices.Count;
                for (int i = 0; i < mesh0.TopologyEdges.Count; i++) {//(mesh0.TopologyEdges.Count * 0.5)
                    if (mesh0.TopologyEdges.GetConnectedFaces(i).Length == 1) {
                        Rhino.IndexPair pair = mesh0.TopologyEdges.GetTopologyVertices(i);
                        mesh.Faces.AddFace(pair.I, pair.J, pair.J + n, pair.I + n);
                    }
                }

                mesh.UnifyNormals();

                if (addNGons) {
                    mesh.Ngons.AddPlanarNgons(0.01, 3, 3, true);
                    //mesh.Bake();

                    Mesh explodedMesh = new Mesh();
                    IEnumerable<MeshNgon> ngonFaces = mesh.GetNgonAndFacesEnumerable();

                    foreach (MeshNgon ngon in ngonFaces) {
                        int[] meshFaceList = System.Linq.Enumerable.ToArray(System.Linq.Enumerable.Select(ngon.FaceIndexList(), j => unchecked((int)j)));
                        Mesh NGonFaceMesh = mesh.DuplicateMesh().Faces.ExtractFaces(meshFaceList);
                        explodedMesh.Append(NGonFaceMesh);
                    }
                    explodedMesh.RebuildNormals();

                    return explodedMesh;
                }

                return mesh;
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
                return new Mesh();
            }
        }

        //public static Rhino.Geometry.Mesh LoftPolylineWithHoles(Polyline[] polylines0, Polyline[] polylines1) {
        //    Mesh mesh0 = MeshFromClosedPolylineWithHoles(polylines0);
        //    Mesh mesh1 = MeshFromClosedPolylineWithHoles(polylines1);

        //    Mesh m = new Mesh();
        //    m.Append(mesh0);
        //    m.Append(mesh1);

        //    return m;
        //}

        public static Tuple<DataTree<Polyline>, DataTree<Polyline>> Stellate(Mesh m, List<double> d0, List<double> d1) {
            Polyline[] plines = m.GetPolylines();
            Plane[] planes = m._FPlanes();

            var trianglesAll = new DataTree<Polyline>();
            var centerPartAll = new DataTree<Polyline>();

            for (int i = 0; i < plines.Length; i++) {
                //Move pyramid center
                Plane pl = planes[i];
                Point3d c = pl.Origin + pl.Normal * d0[i];
                Line line = new Line(pl.Origin, c);

                Plane plane = pl.ChangeOrigin(line.PointAt(d1[i]));

                double d = double.MaxValue;
                Point3d s = Point3d.Unset;
                for (int j = 0; j < plines[i].Count - 1; j++) {
                    Point3d p = PlaneUtil.LinePlane(line, plane.ChangeOrigin(plines[i][j]));

                    double dist = p.DistanceToSquared(c);
                    if (dist < d) {
                        d = dist;
                        s = p;
                    }
                }
                line = new Line(s + (c - s) * 0.00001, c);
                plane = pl.ChangeOrigin(line.PointAt(d1[i]));

                Line[] lines = new Line[plines[i].Count - 1];
                Line[] linesCut = new Line[plines[i].Count - 1];

                for (int j = 0; j < plines[i].Count - 1; j++) {
                    lines[j] = new Line(plines[i][j], c);
                    Point3d p = PlaneUtil.LinePlane(lines[j], plane);
                    linesCut[j] = new Line(plines[i][j], p);
                }

                Polyline[] triangles = new Polyline[plines[i].Count - 1];

                Polyline centerPart = new Polyline();
                for (int j = 0; j < plines[i].Count - 1; j++) {
                    if (d1[i] > 0.999999) {
                        triangles[j] = new Polyline { plines[i][j], plines[i][j + 1], c, plines[i][j] };
                    } else {
                        triangles[j] = new Polyline { linesCut[j].From, linesCut[(j + 1) % lines.Length].From, linesCut[(j + 1) % lines.Length].To, linesCut[j].To, linesCut[j].From };
                        centerPart.Add(linesCut[j].To);
                    }
                }
                if (d1[i] < 0.999999) {
                    centerPart.Close();
                    centerPartAll.Add(centerPart, new Grasshopper.Kernel.Data.GH_Path(i));
                }

                trianglesAll.AddRange(triangles, new Grasshopper.Kernel.Data.GH_Path(i));

                //Create triangles
            }
            return Tuple.Create(trianglesAll, centerPartAll);
            //A = trianglesAll;
            //B = centerPartAll;
        }
    }
}
