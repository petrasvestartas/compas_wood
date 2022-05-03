using Grasshopper;
using Grasshopper.Kernel.Data;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;

namespace rhino_util {

    public static class NGonUtil {

        public static void GetOverlap(Mesh m0, Mesh m1, double tolerance = 0.1) {
            //Check planes or polylines
            Polyline[] plines0 = m0.GetPolylines();
            Polyline[] plines1 = m1.GetPolylines();

            Plane[] pl0 = m0.GetNgonPlanes();
            Plane[] pl1 = m1.GetNgonPlanes();

            //1.Check if planes are coplanar
            //2. Get mesh pline bounding box aligned to plane and check both planes bbox collision
        }

        public static List<List<Polyline>> GetFlaps(this Mesh M, double D, bool F, bool T, bool chamfer, ref List<Mesh> meshes) {
            List<List<Polyline>> flapsAll = new List<List<Polyline>>();
            meshes = new List<Mesh>();

            try {
                Polyline[] plines = M.GetPolylines();

                int[][] tv = M.GetNGonsTopoBoundaries();
                int[][] fe = M.GetNGonFacesEdges(tv);
                HashSet<int> allE = M.GetAllNGonEdges(tv);
                int[][] ef = M.GetNgonsConnectedToNGonsEdges(allE, true);
                uint[][] finf = M.GetFacesInNGons();

                int[] allEArray = allE.ToArray();

                Dictionary<int, int> meNgonE = new Dictionary<int, int>();

                for (int i = 0; i < allEArray.Length; i++) {
                    meNgonE.Add(allEArray[i], i);
                }

                for (int i = 0; i < plines.Length; i++) {
                    List<Polyline> flaps = new List<Polyline>() { plines[i] };
                    Mesh meshTemp = Mesh.CreateFromClosedPolyline(plines[i]);

                    if (D != 0) {
                        Point3d center = M._Plane(i).Origin;

                        for (int j = 0; j < plines[i].Count - 1; j++) {
                            int e = fe[i][j];

                            int[] faces = M.TopologyEdges.GetConnectedFaces(e);
                            //M.TopologyEdges.EdgeLine(e).Bake();
                            //Print(faces.Length.ToString());

                            int neiFace = faces[0];
                            Vector3d normalDir = Vector3d.Zero;

                            if (faces.Length == 2) {
                                bool flag = false;
                                foreach (var ff in finf[i]) {
                                    if (faces[0] == (int)ff) {
                                        flag = true;
                                        break;
                                    }
                                }
                                neiFace = flag ? faces[1] : faces[0];

                                normalDir = (M.FaceNormals[faces[1]] + M.FaceNormals[faces[0]]);
                                normalDir.Unitize();
                            } else {
                                normalDir = (M.FaceNormals[faces[0]]);
                                normalDir.Unitize();
                            }

                            Line l = plines[i].SegmentAt(j);
                            Vector3d dir = Vector3d.CrossProduct(M.FaceNormals[neiFace], l.Direction);
                            dir.Unitize();
                            Point3d p0 = l.PointAt(0.5);

                            if (T)
                                dir = -normalDir;

                            //Point3d centerNext = M._Plane(i).Origin;
                            //if( (p0 + dir).DistanceTo(center) < (p0 - dir).DistanceTo(center) )
                            //dir.Reverse();

                            int sign = faces.Length == 2 ? -1 : -1;
                            if (F)
                                sign *= -1;
                            double distance = D * sign;
                            Vector3d chamferV = chamfer ? l.Direction.UnitVector() * distance : Vector3d.Zero;
                            Polyline polyline = new Polyline() { l.From, l.To, l.To + dir * distance + chamferV, l.From + dir * distance - chamferV, l.From };
                            meshTemp.Append(Mesh.CreateFromClosedPolyline(polyline));
                            flaps.Add(polyline);
                        }//for j
                    }
                    meshTemp.Compact();
                    meshTemp.Vertices.CombineIdentical(true, true);
                    meshTemp.Vertices.CullUnused();
                    meshTemp.UnifyNormals();

                    meshTemp.Weld(3.14159265358979);
                    meshTemp.FaceNormals.ComputeFaceNormals();
                    meshTemp.Normals.ComputeNormals();
                    meshes.Add(meshTemp);
                    flapsAll.Add(flaps);
                }

                //A = flapsAll;
                //B = meshes;
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return flapsAll;
        }

        public static List<Polyline> SplitNGonsIn3(this Mesh m, int maxSize = 6, int iterations = 2, int smooth = 1) {
            if (iterations <= 0)
                return m.GetPolylines().ToList();

            List<Polyline> splitPlines = new List<Polyline>(m.Ngons.Count * 2);

            try {
                bool run = true;

                Mesh m_ = m.DuplicateMesh();

                List<int>[] adj = m_.GetNGonFaceAdjacency();
                Polyline[] plines = m_.GetPolylines();

                int counter = 0;
                while (run && counter < iterations) {
                    bool flag = true;

                    List<int> ids = new List<int>();
                    List<int> numOfVertices = new List<int>();

                    for (int i = 0; i < plines.Length; i++) {
                        if (plines[i].Count - 1 > (maxSize) && flag) {
                            ids.Add(i);
                            numOfVertices.Add(plines[i].Count - 1);
                        }
                    }

                    int ib = -1;
                    if (ids.Count > 0) {
                        int[] idsArray = ids.ToArray();
                        int[] numOfVerticesArray = numOfVertices.ToArray();
                        Array.Sort(numOfVerticesArray, idsArray);
                        ib = idsArray.Last();
                        int ibV = numOfVerticesArray.Last();

                        List<int> sameID = new List<int>();
                        List<double> sameIDLength = new List<double>();
                        for (int i = idsArray.Length - 1; i >= 0; i--) {
                            if (ibV == plines[idsArray[i]].Count - 1) {
                                sameID.Add(idsArray[i]);
                                sameIDLength.Add(plines[idsArray[i]].Length);
                            } else {
                                break;
                            }
                        }
                        int[] sameIDArray = sameID.ToArray();
                        double[] sameIDLengthArray = sameIDLength.ToArray();
                        Array.Sort(sameIDLengthArray, sameIDArray);
                        ib = sameIDArray.Last();
                    }

                    //Rhino.RhinoApp.WriteLine(ib.ToString());

                    for (int i = 0; i < plines.Length; i++) {
                        Point3d center = plines[i].CenterPoint();

                        if (i == ib && ib != -1) {
                            plines[i].RemoveAt(plines[i].Count - 1);
                            var points = GrasshopperUtil.PartitionByN(plines[i], 3);
                            var IDs = GrasshopperUtil.PartitionByN(Enumerable.Range(0, plines[i].Count).ToList(), 3).ToArray();

                            //Get partitions points into list of polylines
                            List<Polyline> plines3 = new List<Polyline>(3);

                            foreach (var p in points)
                                plines3.Add(new Polyline(p.ToList()));

                            //Add last point, center and close

                            //center.Bake();

                            List<Point3d> pointsToInsert = new List<Point3d>();
                            for (int j = 0; j < plines3.Count; j++) {
                                if (j > 0)
                                    plines3[j].Insert(0, plines3[j - 1].Last());

                                //Rhino.RhinoApp.WriteLine(IDs[j].First().ToString());
                                Point3d p0 = plines[i][MathUtil.Wrap(IDs[j].Last() + 0, plines[i].Count)];
                                Point3d p1 = plines[i][MathUtil.Wrap(IDs[j].Last() + 1, plines[i].Count)];
                                Point3d pMid = 0.5 * (p0 + p1);
                                pointsToInsert.Add(pMid);
                                plines3[j].Add(pMid);
                                //plines3[j].Add(p1);

                                //plines3[j].Add(center);
                                //plines3[j].Close();
                            }
                            plines3[0].Insert(0, plines3[plines3.Count - 1].Last());

                            for (int j = 0; j < plines3.Count; j++) {
                                plines3[j].Add(center);
                                plines3[j].Close();
                            }

                            //Insert to neighbour faces
                            //Rhino.RhinoApp.WriteLine(counter.ToString() + " adj " + adj[i].Count.ToString() + " Pt to insert " + pointsToInsert.Count());
                            for (int j = 0; j < adj[i].Count; j++) {
                                foreach (Point3d pt in pointsToInsert) {
                                    //pt.Bake();

                                    double t0 = plines[adj[i][j]].ClosestParameter(pt);
                                    //double t0 = plines[adj[i][j]].closes(pt);
                                    Point3d cp = plines[adj[i][j]].PointAt(t0);
                                    if (counter == 9) {
                                        //plines[adj[i][j]].Bake();
                                    }

                                    if (cp.DistanceTo(pt) < 0.01) {
                                        int n = plines[adj[i][j]].Count - 1;
                                        int insertID = (int)Math.Ceiling(t0) == plines[adj[i][j]].Count - 1 ? (int)Math.Ceiling(t0) : (int)Math.Ceiling(t0);
                                        //plines[adj[i][j]].Insert(MathUtil.Wrap((int)Math.Ceiling(t0), plines[adj[i][j]].Count - 1), cp);
                                        plines[adj[i][j]].Insert(insertID, cp);
                                        if (counter == 9) {
                                            //cp.Bake();
                                            //plines[adj[i][j]].Bake();
                                        }
                                        //x.InsertRange(, poly);
                                    }
                                }
                            }

                            //Add to global list
                            splitPlines.AddRange(plines3);
                            flag = false;
                        } else {
                            //Add to global list
                            splitPlines.Add(plines[i]);
                        }
                    }

                    m_ = MeshCreate.MeshFromPolylines(splitPlines, -1, 0);
                    if (!m_.IsValid) {
                        Rhino.RhinoApp.WriteLine(m_.IsValid.ToString());
                        //splitPlines.Bake();
                        break;
                    }
                    m_ = m_.WeldUsingRTree(0.01, true);

                    //if(counter == 12)
                    //m_.Bake();
                    adj = m_.GetNgonFaceAdjacencyOrdered();
                    plines = m_.GetPolylines();
                    run = !flag;
                    counter++;
                    if ((run && counter < iterations) == true)
                        splitPlines.Clear();
                    //Rhino.RhinoApp.WriteLine(m_.IsValid.ToString());
                }
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            if (smooth > 0) {
                Mesh meshFinal = MeshCreate.MeshFromPolylines(splitPlines, -1, 0);
                for (int i = 0; i < smooth; i++) {
                    meshFinal.Smooth(1, true, true, true, true, SmoothingCoordinateSystem.Object);
                }
                splitPlines = meshFinal.GetPolylines().ToList();
            }
            return splitPlines;
        }

        public static int[] SelectNGons(this Mesh m, List<Point3d> p) {
            Point3d[] c = m.GetNGonCenters();

            int[] cpID = new int[p.Count];

            for (int i = 0; i < p.Count; i++) {
                cpID[i] = PointUtil.ClosestPoint(p[i], c);
            }

            return cpID;
        }

        public static HashSet<int> SelectNGonsHash(this Mesh m, List<Point3d> p) {
            Point3d[] c = m.GetNGonCenters();

            HashSet<int> cpID = new HashSet<int>();

            for (int i = 0; i < p.Count; i++) {
                cpID.Add(PointUtil.ClosestPoint(p[i], c));
            }

            return cpID;
        }

        public static HashSet<int> SelectEdges(this Mesh m, List<Line> lines) {
            var cloud = m.GetNGonEdgesMidPoints();
            HashSet<int> closestEdges = new HashSet<int>();

            for (int i = 0; i < lines.Count; i++) {
                int id = cloud.ClosestPoint(lines[i].PointAt(0.5));

                closestEdges.Add((int)cloud[id].Normal.X);
            }
            return closestEdges;
        }

        public static HashSet<int> SelectEdges(this Mesh m, List<Point3d> p) {
            var cloud = m.GetNGonEdgesMidPoints();
            HashSet<int> closestEdges = new HashSet<int>();

            for (int i = 0; i < p.Count; i++) {
                int id = cloud.ClosestPoint(p[i]);

                closestEdges.Add((int)cloud[id].Normal.X);
            }
            return closestEdges;
        }

        public static PointCloud GetNGonEdgesMidPoints(this Mesh m) {
            PointCloud cloud = new PointCloud();
            HashSet<int> e = m.GetAllNGonEdges(m.GetNGonsTopoBoundaries());
            Line[] lines = m.GetAllNGonEdgesLines(e);

            foreach (int id in e) {
                cloud.Add(m.TopologyEdges.EdgeLine(id).PointAt(0.5), new Vector3d(id, 0, 0));
            }

            return cloud;
        }

        public static PointCloud GetNGonEdgesMidPoints(this Mesh m, List<int> e) {
            PointCloud cloud = new PointCloud();

            Line[] lines = m.GetAllNGonEdgesLines(m.GetAllNGonEdges(m.GetNGonsTopoBoundaries()));

            foreach (int edge in e)
                cloud.Add(m.TopologyEdges.EdgeLine(edge).PointAt(0.5));

            return cloud;
        }

        public static Mesh ReoderMeshNgons(this Mesh m, List<int> s) {
            if (s.Count == m.Ngons.Count) {
                Polyline[] p0 = m.GetPolylines();
                Polyline[] p1 = new Polyline[p0.Length];

                for (int i = 0; i < p0.Length; i++) {
                    p1[i] = new Polyline(p0[s[i]]);
                }

                return MeshCreate.MeshFromPolylines(p1, 0.01);
            }
            return m;
        }

        public static Mesh ReoderMeshNgons(this Mesh m, Curve c) {
            Polyline[] p0 = m.GetPolylines();
            double[] t0 = new double[p0.Length];

            for (int i = 0; i < p0.Length; i++) {
                c.ClosestPoint(p0[i].CenterPoint(), out double t);
                t0[i] = t;
            }

            Array.Sort(t0, p0);
            return MeshCreate.MeshFromPolylines(p0, 0.01);
        }

        public static Vector3d GetMeshEdgePerpDir(this Mesh mesh, int e) {
            int[] ngons = mesh.GetEdgeNgons(e);

            Vector3d vec = Vector3d.Zero;

            for (int i = 0; i < ngons.Length; i++) {
                vec += mesh.GetNGonNormal(ngons[i]);
            }

            vec.Unitize();
            return vec;
        }

        public static int[] GetEdgeNgons(this Mesh mesh, int e) {
            int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

            int[] ngons = new int[0];

            if (connectedFaces.Length == 1)
                ngons = new int[] { mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[0]) };
            else {
                ngons = new int[connectedFaces.Length];
                for (int i = 0; i < connectedFaces.Length; i++)
                    ngons[i] = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
            }

            return ngons;
        }

        public static int GetOppositeNgon(this Mesh mesh, int e, int NGonID) {
            int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

            int[] ngons = new int[0];

            if (connectedFaces.Length == 1)
                ngons = new int[] { mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[0]) };
            else {
                ngons = new int[connectedFaces.Length];
                for (int i = 0; i < connectedFaces.Length; i++)
                    ngons[i] = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
            }

            if (ngons.Length == 2) {
                int oppositeNGon = (ngons[0] == NGonID) ? ngons[1] : ngons[0];
                return oppositeNGon;
            }

            return -1;
        }

        public static DataTree<Vector3d> insertionVectors(this Mesh M, bool Center = false, DataTree<Vector3d> EV_ = null) {
            DataTree<Vector3d> EV = new DataTree<Vector3d>();
            //DataTree<Vector3d> EV_ = new DataTree<Vector3d>();

            Point3d[] centers = M.GetNGonCenters();

            int[][] tv = M.GetNGonsTopoBoundaries();
            HashSet<int> e = M.GetAllNGonEdges(tv);
            Dictionary<int, int[]> efDict = M.GetFE(e, false);

            foreach (int ee in e) {
                if (M.TopologyEdges.GetConnectedFaces(ee).Length != 1) {
                    if (Center) {
                        int[] facePair = efDict[ee];
                        Vector3d insertionVec = centers[facePair[0]] - centers[facePair[1]];
                        insertionVec.Unitize();
                        EV.Add(insertionVec, new GH_Path(ee));
                    } else {
                        EV.Add(VectorUtil.BisectorVector(M.TopologyEdges.EdgeLine(ee), M.GetNGonNormal(efDict[ee][0])), new GH_Path(ee));
                    }
                }
            }

            //EV_ = new DataTree<Vector3d>(EV);
            if (EV_ != null)
                if (EV_.BranchCount == EV.BranchCount)
                    return EV_;

            return EV;
        }

        public static void GetEdgeAndBisectorPlanes(this Mesh M, out Plane[][] bisePlanes, out Plane[][] edgePlanes) {
            int[][] fv = M.GetNGonsTopoBoundaries();
            bisePlanes = new Plane[fv.Length][];
            edgePlanes = new Plane[fv.Length][];

            for (int i = 0; i < fv.Length; i++) {
                int n = fv[i].Length;
                bisePlanes[i] = new Plane[n];
                edgePlanes[i] = new Plane[n];

                for (int j = 0; j < fv[i].Length; j++) {
                    int v0 = fv[i][MathUtil.Wrap(j - 1, n)];
                    int v1 = fv[i][(j + 0) % n];
                    int v2 = fv[i][(j + 1) % n];

                    int e = M.TopologyEdges.GetEdgeIndex(v0, v1);
                    int[] cf = M.TopologyEdges.GetConnectedFaces(e);
                    Vector3d sum = Vector3d.Zero;

                    for (int k = 0; k < cf.Length; k++) {
                        sum += M.FaceNormals[cf[k]];
                    }
                    sum /= cf.Length;

                    //if (cf.Length == 1)
                    //    sum = Vector3d.ZAxis;

                    Plane edgePlane = new Plane((Point3d)(M.Vertices[v0] + M.Vertices[v1]) * 0.5, M.Vertices[v0] - M.Vertices[v1], sum);//M.Normals[v0] + M.Normals[v1]
                    Plane bisePlane = new Plane(
                      M.Vertices[v1],
                      VectorUtil.BisectorVector(new Line(M.Vertices[v0], M.Vertices[v1]), new Line(M.Vertices[v1], M.Vertices[v2]), M.Normals[v1]),
                      M.Normals[v1]
                      );

                    bisePlanes[i][j] = bisePlane;
                    edgePlanes[i][j] = edgePlane;
                }//for j
            }//for i

            //BisePlanes = bisePlanes;
            //EdgePlanes = edgePlanes;
        }

        public static Polyline[] ProjectedPolylinesToAveragePlane(this Mesh mesh, bool averagePlane = true) {
            Polyline[] polylines = mesh.GetPolylines();

            for (int i = 0; i < polylines.Length; i++) {
                polylines[i] = polylines[i].ProjectPolyline(averagePlane);
            }

            return polylines;
        }

        public static List<Polyline> UnifyWinding(this Mesh m, Grasshopper.DataTree<int> FP) {
            //Convert DataTree to Nested Array
            int[][] fp = new int[FP.BranchCount][];
            for (int i = 0; i < FP.BranchCount; i++) {
                fp[i] = FP.Branch(i).ToArray();
            }
            return UnifyWinding(m, fp);
        }

        public static List<Polyline> UnifyWinding(this Mesh m, int[][] fp) {
            //m - mesh
            //fp - face pairs

            //Inputs
            Polyline[] polylines = m.GetPolylines();

            int[][] boundaries = m.GetNGonsTopoBoundaries();
            int[][] fe = m.GetNGonFacesEdges(boundaries);

            //Solution inputs
            List<Polyline> orderedPolylines = new List<Polyline>();
            HashSet<int> historyHash = new HashSet<int>();

            //Solution iterations
            for (int i = 0; i < fp.Length; i++) {
                //Select face id and ngon faces
                int a = fp[i][0];
                int b = fp[i][1];
                Polyline p0 = new Polyline(polylines[a]);
                Polyline p1 = new Polyline(polylines[b]);

                //Get face edges
                int[] e0 = fe[a];
                int[] e1 = fe[b];

                //Get common edge
                int commonE = e0.Intersect(e1).First();

                //Get local edge id and line
                int e0Local = Array.IndexOf(e0, commonE);
                int e1Local = Array.IndexOf(e1, commonE);
                Line e0LocalLine = polylines[a].SegmentAt(e0Local);
                Line e1LocalLine = polylines[b].SegmentAt(e1Local);

                //Check if edges are the same direction
                if (e0LocalLine.From.DistanceToSquared(e1LocalLine.From) < 0.001) {
                    p1.Reverse();
                    fe[b] = fe[b].Reverse().ToArray();
                }

                if (historyHash.Add(a)) orderedPolylines.Add(p0);
                if (historyHash.Add(b)) orderedPolylines.Add(p1);
            }

            //output
            return orderedPolylines;
        }

        public static Mesh LoftMeshFastOpen(this List<Polyline> x) {
            Mesh joinedMesh = new Mesh();

            for (int i = 0; i < x.Count; i += 2) {
                Mesh m = new Mesh();
                m.Vertices.AddVertices(x[i]);
                m.Vertices.AddVertices(x[i + 1]);

                for (int j = 0; j < x[i].Count - 1; j++) {
                    m.Faces.AddFace(j, j + 1, x[i].Count + j + 1, x[i].Count + j);
                }

                joinedMesh.Append(m);
            }

            return joinedMesh;
        }

        /*
        public static Mesh RemoveNgon(this Mesh M, int id) {
            //Copy mesh
            Mesh mesh = M.DuplicateMesh();

            if (mesh.IsValid) {
                    //Item to remove
                    int removeID = id;

                    //Get ngon mesh faces
                    int[] ngonFaces = BoundaryVertexIndexArrayInt(mesh.Ngons[removeID]).Item2;

                    mesh.Ngons.RemoveAt(removeID);

                    //Copy face array
                    int[] faces = NGonsCore.MathUtil.CreateSeries(mesh.Faces.Count);
                    int[] vertices = NGonsCore.MathUtil.CreateSeries(mesh.Vertices.Count);

                    //Remove faces in order
                    var ordered = ngonFaces.OrderByDescending(x => x);
                    mesh.Faces.DeleteFaces(ordered);//remove faces ordered so that it not return error

                    //Copy face array
                    int[] facesUpdated = NGonsCore.MathUtil.CreateSeries(faces.Length);

                    int k = 0;
                    for (int i = 0; i < faces.Length; i++) {
                        if (ordered.Contains(i)) {
                            facesUpdated[i] = (-1);
                        } else {
                            facesUpdated[i] = (k++);
                        }
                    }

                    //Copy all ngon list later they will be changed
                    List<MeshNgon> ngonlist = new List<MeshNgon>();
                    foreach (MeshNgon ng in mesh.Ngons)
                        ngonlist.Add(ng);

                    //Clear ngons from mesh
                    mesh.Ngons.Clear();

                    for (int i = 0; i < ngonlist.Count; i++) {
                        var ngonsInputs = BoundaryVertexIndexArrayInt(ngonlist[i]);

                        for (int j = 0; j < ngonsInputs.Item2.Length; j++) {
                            ngonsInputs.Item2[j] = facesUpdated[ngonsInputs.Item2[j]];
                        }

                        mesh.Ngons.AddNgon(MeshNgon.Create(ngonsInputs.Item1, ngonsInputs.Item2));
                    }

                    //mesh = mesh.DuplicateMesh();

                return mesh;
            }
            return mesh;
        }

        public static Mesh RemoveNgons(this Mesh M, List<int> id) {
            //Copy mesh
            Mesh mesh = M.DuplicateMesh();
            List<int> idOrder = id.OrderByDescending(x => x).ToList();

            if (mesh.IsValid) {
                for (int q = 0; q < idOrder.Count; q++) {
                    //Item to remove
                    int removeID = idOrder[q];

                    //Get ngon mesh faces
                    int[] ngonFaces = BoundaryVertexIndexArrayInt(mesh.Ngons[removeID]).Item2;

                    mesh.Ngons.RemoveAt(removeID);

                    //Copy face array
                    int[] faces = NGonsCore.MathUtil.CreateSeries(mesh.Faces.Count);
                    int[] vertices = NGonsCore.MathUtil.CreateSeries(mesh.Vertices.Count);

                    //Remove faces in order
                    var ordered = ngonFaces.OrderByDescending(x => x);
                    mesh.Faces.DeleteFaces(ordered);//remove faces ordered so that it not return error

                    //Copy face array
                    int[] facesUpdated = NGonsCore.MathUtil.CreateSeries(faces.Length);

                    int k = 0;
                    for (int i = 0; i < faces.Length; i++) {
                        if (ordered.Contains(i)) {
                            facesUpdated[i] = (-1);
                        } else {
                            facesUpdated[i] = (k++);
                        }
                    }

                    //Copy all ngon list later they will be changed
                    List<MeshNgon> ngonlist = new List<MeshNgon>();
                    foreach (MeshNgon ng in mesh.Ngons)
                        ngonlist.Add(ng);

                    //Clear ngons from mesh
                    mesh.Ngons.Clear();

                    for (int i = 0; i < ngonlist.Count; i++) {
                        var ngonsInputs = BoundaryVertexIndexArrayInt(ngonlist[i]);

                        for (int j = 0; j < ngonsInputs.Item2.Length; j++) {
                            ngonsInputs.Item2[j] = facesUpdated[ngonsInputs.Item2[j]];
                        }

                        mesh.Ngons.AddNgon(MeshNgon.Create(ngonsInputs.Item1, ngonsInputs.Item2));
                    }

                    //mesh = mesh.DuplicateMesh();
                }

                return mesh;
            }
            return mesh;
        }

        public static Tuple<int[], int[]> BoundaryVertexIndexArrayInt(MeshNgon ngon) {
            uint[] vertices = ngon.BoundaryVertexIndexList();
            int[] vertexIntArray = new int[vertices.Length];

            for (int i = 0; i < vertices.Length; i++)
                vertexIntArray[i] = (int)vertices[i];

            uint[] faces = ngon.FaceIndexList();
            int[] facesIntArray = new int[faces.Length];

            for (int i = 0; i < faces.Length; i++)
                facesIntArray[i] = (int)faces[i];

            return new Tuple<int[], int[]>(vertexIntArray, facesIntArray);
        }
        */

        public static Mesh LoftMeshFast(this Polyline C0, Polyline C1, bool B) {
            Polyline p0 = C0;
            Polyline p1 = C1;

            //////////////////////////////
            //Treat all polylines as OPEN
            //////////////////////////////
            bool closed = p0.IsClosed && p1.IsClosed;

            if (closed && false)//unless there is viggly joint geometry
            {
                p0[p0.Count - 1] = p0.PointAt(p0.Count - 1 - 0.1);
                p1[p1.Count - 1] = p1.PointAt(p1.Count - 1 - 0.1);
                //p0.Bake();
            } else {
                p0.RemoveAt(p0.Count - 1);
                p1.RemoveAt(p1.Count - 1);
            }
            int n = p0.Count;
            //////////////////////////////

            //////////////////////////////
            //Triangulate p0 and check if polyline is flipped after triangulation
            //////////////////////////////
            bool reverse = false;

            var meshData = MeshUtil.TriangulateOpenPolyline(p0, out reverse);

            if (reverse) {
                p0.Reverse();
                p1.Reverse();
            }

            Mesh loft = meshData.Item1;
            List<MeshFace> mfList = meshData.Item2;
            //////////////////////////////

            //////////////////////////////
            //Add Points from the bottom polylines
            //////////////////////////////
            for (int j = 0; j < n; j++)
                loft.Vertices.Add(p1[j]);
            //////////////////////////////

            //////////////////////////////
            //Add Bottom faces
            //////////////////////////////
            MeshFace[] mf = new MeshFace[mfList.Count];
            for (int j = 0; j < mfList.Count; j++) {
                int a = mfList[j].A + n;
                int b = mfList[j].B + n;
                int c = mfList[j].C + n;

                mf[j] = new MeshFace(c, b, a);
            }
            loft.Faces.AddFaces(mf);
            //////////////////////////////

            //////////////////////////////
            //Add Side faces
            //////////////////////////////
            for (int j = 0; j < n; j++) {
                MeshFace mf_side = new MeshFace(
                  j,
                  j + n,
                  n + (j + 1) % (n),
                 (j + 1) % (n)

                  );
                loft.Faces.AddFace(mf_side);
            }
            //////////////////////////////

            if (B) {
                return loft;
            } else {
                loft.RebuildNormals();

                if (loft.SolidOrientation() == -1)
                    loft.Flip(true, true, true);
                loft.Unweld(0.01, true);
                loft.RebuildNormals();
                return loft;
            }

            return loft;
        }

        public static Grasshopper.DataTree<Mesh> LoftMeshFast(Grasshopper.DataTree<Polyline> C0, Grasshopper.DataTree<Polyline> C1, bool B) {
            Mesh mesh = new Mesh();
            Grasshopper.DataTree<Mesh> meshes = new Grasshopper.DataTree<Mesh>();

            if (C0.BranchCount == C1.BranchCount) {
                for (int i = 0; i < C0.BranchCount; i++) {
                    if (C0.Branch(i).Count == 1 && C1.Branch(i).Count == 1) {
                        Polyline p0 = C0.Branch(i)[0];
                        Polyline p1 = C1.Branch(i)[0];

                        //////////////////////////////
                        //Treat all polylines as OPEN
                        //////////////////////////////
                        bool closed = p0.IsClosed && p1.IsClosed;

                        if (closed && false)//unless there is viggly joint geometry
                        {
                            p0[p0.Count - 1] = p0.PointAt(p0.Count - 1 - 0.1);
                            p1[p1.Count - 1] = p1.PointAt(p1.Count - 1 - 0.1);
                            //p0.Bake();
                        } else {
                            p0.RemoveAt(p0.Count - 1);
                            p1.RemoveAt(p1.Count - 1);
                        }
                        int n = p0.Count;
                        //////////////////////////////

                        //////////////////////////////
                        //Triangulate p0 and check if polyline is flipped after triangulation
                        //////////////////////////////
                        bool reverse = false;

                        var meshData = MeshUtil.TriangulateOpenPolyline(p0, out reverse);

                        if (reverse) {
                            p0.Reverse();
                            p1.Reverse();
                        }

                        Mesh loft = meshData.Item1;
                        List<MeshFace> mfList = meshData.Item2;
                        //////////////////////////////

                        //////////////////////////////
                        //Add Points from the bottom polylines
                        //////////////////////////////
                        for (int j = 0; j < n; j++)
                            loft.Vertices.Add(p1[j]);
                        //////////////////////////////

                        //////////////////////////////
                        //Add Bottom faces
                        //////////////////////////////
                        MeshFace[] mf = new MeshFace[mfList.Count];
                        for (int j = 0; j < mfList.Count; j++) {
                            int a = mfList[j].A + n;
                            int b = mfList[j].B + n;
                            int c = mfList[j].C + n;

                            mf[j] = new MeshFace(c, b, a);
                        }
                        loft.Faces.AddFaces(mf);
                        //////////////////////////////

                        //////////////////////////////
                        //Add Side faces
                        //////////////////////////////
                        for (int j = 0; j < n; j++) {
                            MeshFace mf_side = new MeshFace(
                              j,
                              j + n,
                              n + (j + 1) % (n),
                             (j + 1) % (n)

                              );
                            loft.Faces.AddFace(mf_side);
                        }
                        //////////////////////////////

                        if (B) {
                            mesh.Append(loft);
                        } else {
                            loft.RebuildNormals();

                            if (loft.SolidOrientation() == -1)
                                loft.Flip(true, true, true);
                            loft.Unweld(0.01, true);
                            loft.RebuildNormals();
                            meshes.Add(loft, C0.Paths[i]);
                        }
                    }//if
                }//for i
            }//if

            if (B) {
                //mesh.RebuildNormals();
                return new Grasshopper.DataTree<Mesh>(new Mesh[] { mesh }, new Grasshopper.Kernel.Data.GH_Path(0));
            }

            return meshes;
        }

        public static List<Point3d>[] GetConnectedNgonPoints(this Mesh mesh, List<int>[] connectedNgonVertices) {
            List<Point3d>[] connectedNgonPoints = new List<Point3d>[connectedNgonVertices.Length];

            for (int i = 0; i < connectedNgonVertices.Length; i++) {
                connectedNgonPoints[i] = new List<Point3d>();
                foreach (int j in connectedNgonVertices[i])
                    connectedNgonPoints[i].Add(mesh.TopologyVertices[j]);
            }

            return connectedNgonPoints;
        }

        public static List<int>[] GetConnectedNgonVertices(this Mesh mesh, int[] allEArray, int[] allvArray) {
            // List < Point3d > centers = new List<Point3d>();
            //DataTree < Point3d > neighbours = new DataTree<Point3d>();

            List<int>[] connectedNgonVertices = new List<int>[allvArray.Length];

            for (int i = 0; i < allvArray.Length; i++) {
                int[] connectedVertices = mesh.TopologyVertices.ConnectedTopologyVertices(allvArray[i], true);
                connectedNgonVertices[i] = new List<int>();

                foreach (int j in connectedVertices) {
                    int edge = mesh.TopologyEdges.GetEdgeIndex(allvArray[i], j);
                    int localID = Array.IndexOf(allEArray, edge);//if edge present in boundaries
                    if (localID != -1)
                        connectedNgonVertices[i].Add(j);
                }

                //if(ngonV.Count == 3){
                //foreach(int j in  connectedNgonVertices[i]){
                // neighbours.Add(mesh.TopologyVertices[j], new GH_Path(i));
                //}

                //centers.Add(mesh.TopologyVertices[allvArray[i]]);
                // }
            }

            return connectedNgonVertices;
        }

        /// <summary>
        /// Convert mesh vertex ID array to mesh topological vertex ID array
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="vertexIDs"></param>
        /// <returns></returns>
        public static int[] VertexTopoVertex(this Mesh mesh, uint[] vertexIDs) {
            int[] topoVertices = new int[vertexIDs.Length];

            for (int i = 0; i < vertexIDs.Length; i++)
                topoVertices[i] = mesh.TopologyVertices.TopologyVertexIndex((int)vertexIDs[i]);

            return topoVertices;
        }

        /// <summary>
        /// Get NGons Boundaries as vertex ID array
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static uint[][] GetNGonsBoundaries(this Mesh mesh) {
            uint[][] boundaries = new uint[mesh.Ngons.Count][];

            for (int i = 0; i < mesh.Ngons.Count; i++)
                boundaries[i] = mesh.Ngons[i].BoundaryVertexIndexList();

            return boundaries;
        }

        /// <summary>
        /// Get NGons Boundaries as topological vertex ID array
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static int[][] GetNGonsTopoBoundaries(this Mesh mesh) {
            int[][] boundaries = new int[mesh.Ngons.Count][];

            for (int i = 0; i < mesh.Ngons.Count; i++)
                boundaries[i] = VertexTopoVertex(mesh, mesh.Ngons[i].BoundaryVertexIndexList());

            return boundaries;
        }

        /// <summary>
        /// Get NGons Boundaries as vertex (Point3f) array
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="verticesIDs"></param>
        /// <returns></returns>
        public static Point3f[][] GetNGonsBoundariesPoint3F(this Mesh mesh, uint[][] verticesIDs) {
            Point3f[][] pts = new Point3f[verticesIDs.Length][];

            for (int i = 0; i < verticesIDs.Length; i++) {
                pts[i] = new Point3f[verticesIDs[i].Length];
                for (int j = 0; j < verticesIDs[i].Length; j++)
                    pts[i][j] = mesh.Vertices[(int)verticesIDs[i][j]];
            }

            return pts;
        }

        /// <summary>
        /// Get NGons Boundaries as vertex (Point3d) array
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="verticesIDs"></param>
        /// <returns></returns>
        public static Point3d[][] GetNGonsBoundariesPoint3d(this Mesh mesh, uint[][] verticesIDs) {
            Point3d[][] pts = new Point3d[verticesIDs.Length][];

            for (int i = 0; i < verticesIDs.Length; i++) {
                pts[i] = new Point3d[verticesIDs[i].Length];
                for (int j = 0; j < verticesIDs[i].Length; j++)
                    pts[i][j] = mesh.Vertices[(int)verticesIDs[i][j]];
            }

            return pts;
        }

        /// <summary>
        /// Get NGons Boundaries as topological vertex (Point3f) array
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="topologyVerticesIDs"></param>
        /// <returns></returns>
        public static Point3f[][] GetNGonsTopoBoundariesPoint3F(this Mesh mesh, int[][] topologyVerticesIDs) {
            Point3f[][] pts = new Point3f[topologyVerticesIDs.Length][];

            for (int i = 0; i < topologyVerticesIDs.Length; i++) {
                pts[i] = new Point3f[topologyVerticesIDs[i].Length];
                for (int j = 0; j < topologyVerticesIDs[i].Length; j++)
                    pts[i][j] = mesh.TopologyVertices[topologyVerticesIDs[i][j]];
            }

            return pts;
        }

        /// <summary>
        /// Get Mesh Topology vertices in all NGons (HashSet)
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static HashSet<int> GetAllNGonsTopoVertices(this Mesh mesh) {
            HashSet<int> allNGonTV = new HashSet<int>();

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                uint[] meshV = mesh.Ngons[i].BoundaryVertexIndexList();
                int[] meshTv = VertexTopoVertex(mesh, meshV);

                foreach (int j in meshTv)
                    allNGonTV.Add(j);
            }
            return allNGonTV;
        }

        /// <summary>
        /// Get Mesh vertices in all NGons (HashSet)
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static HashSet<int> GetAllNGonsVertices(this Mesh mesh) {
            HashSet<int> allNGonV = new HashSet<int>();

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                uint[] meshV = mesh.Ngons[i].BoundaryVertexIndexList();
                foreach (uint j in meshV)
                    allNGonV.Add((int)j);
            }

            return allNGonV;
        }

        /// <summary>
        /// Get Mesh Topology vertices (Point3f) in all NGons (HashSet)
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonTv"></param>
        /// <returns></returns>
        public static Point3f[] GetAllNGonsTopoVerticesPoint3F(this Mesh mesh, HashSet<int> allNGonTv) {
            Point3f[] pts = new Point3f[allNGonTv.Count];
            int i = 0;
            foreach (var id in allNGonTv) {
                pts[i] = mesh.TopologyVertices[id];
                i++;
            }

            return pts;
        }

        /// <summary>
        /// Get Mesh  vertices (Point3f) in all NGons (HashSet)
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonV"></param>
        /// <returns></returns>
        public static Point3f[] GetAllNGonsVerticesPoint3F(this Mesh mesh, HashSet<int> allNGonV) {
            Point3f[] pts = new Point3f[allNGonV.Count];
            int i = 0;
            foreach (var id in allNGonV) {
                pts[i] = mesh.Vertices[id];
                i++;
            }

            return pts;
        }

        /// <summary>
        /// Get NGons as Polylines from Mesh
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Polyline[] GetPolylines(this Mesh mesh) {
            Polyline[] polylines = new Polyline[mesh.Ngons.Count];
            //Rhino.RhinoApp.WriteLine(mesh.Ngons.Count.ToString());
            uint[][] boundaries = GetNGonsBoundaries(mesh);

            for (int i = 0; i < boundaries.Length; i++) {
                Polyline p = new Polyline();

                for (int j = 0; j < boundaries[i].Length; j++)
                    p.Add(mesh.Vertices[(int)boundaries[i][j]]);

                p.Add(mesh.Vertices[(int)boundaries[i][0]]);
                polylines[i] = p;
                //p.Bake();
            }

            return polylines;
        }

        /// <summary>
        /// Get NGon Centers
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Point3d[] GetNGonCenters(this Mesh mesh) {
            Point3d[] centers = new Point3d[mesh.Ngons.Count];

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                centers[i] = mesh.GetNgonCenter(i);
            }

            return centers;
        }

        public static Point3d GetNgonCenter(this Mesh mesh, int ngon) {
            uint[] vertices = mesh.Ngons.GetNgon(ngon).BoundaryVertexIndexList();

            Polyline outline = new Polyline();
            foreach (int v in vertices) {
                outline.Add(mesh.Vertices[v]);
            }

            return CenterPoint(outline);
        }

        public static Point3f[] GetNGonCentersPoint3f(this Mesh mesh) {
            Point3f[] centers = new Point3f[mesh.Ngons.Count];
            Polyline[] polylines = mesh.GetPolylines();

            for (int i = 0; i < mesh.Ngons.Count; i++)
                centers[i] = (Point3f)mesh.GetNgonCenter(i);

            return centers;
        }

        public static Plane[] GetNgonPlanes(this Mesh mesh) {
            Plane[] p = new Plane[mesh.Ngons.Count];

            uint[][] b = mesh.GetNGonsBoundaries();
            Point3d[][] pts = mesh.GetNGonsBoundariesPoint3d(b);

            for (int i = 0; i < mesh.Ngons.Count; i++)
                //Plane.FitPlaneToPoints(pts[i], out p[i]);
                p[i] = new Plane(mesh.GetNgonCenter(i), mesh.GetNGonNormal(i));
            return p;
        }

        public static Vector3d[] GetNgonNormals(this Mesh mesh) {
            Vector3d[] p = new Vector3d[mesh.Ngons.Count];

            uint[][] b = mesh.GetNGonsBoundaries();
            Point3d[][] pts = mesh.GetNGonsBoundariesPoint3d(b);

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                //Plane pp;
                //Plane.FitPlaneToPoints(pts[i], out pp);
                p[i] = mesh.GetNGonNormal(i);
                //p[i] = pp.ZAxis;
            }

            return p;
        }

        public static Vector3f[] GetNgonNormals3f(this Mesh mesh) {
            Vector3f[] p = new Vector3f[mesh.Ngons.Count];

            uint[][] b = mesh.GetNGonsBoundaries();
            Point3d[][] pts = mesh.GetNGonsBoundariesPoint3d(b);

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                //Plane pp;
                //Plane.FitPlaneToPoints(pts[i], out pp);
                p[i] = (Vector3f)mesh.GetNGonNormal(i);
                //p[i] = pp.ZAxis;
            }

            return p;
        }

        //public static Vector3d GetNGonNormal(this Mesh mesh, uint[] boundaryVertices)
        //{
        //    //PolyFace item = this[index];
        //    Vector3d vector3d = new Vector3d();
        //    int count = checked(boundaryVertices.Length - 1);
        //    for (int i = 0; i <= count; i++)
        //    {
        //        int num = (int)boundaryVertices[(checked(checked(i - 1) + boundaryVertices.Length)) % boundaryVertices.Length];
        //        int item1 = (int)boundaryVertices[(checked(checked(i + 1) + boundaryVertices.Length)) % boundaryVertices.Length];
        //        Point3d point3d = mesh.Vertices[num];
        //        Point3d point3d1 = mesh.Vertices[item1];
        //        Point3d item2 = mesh.Vertices[(int)boundaryVertices[i]];
        //        vector3d = vector3d + Vector3d.CrossProduct(new Vector3d(item2 - point3d), new Vector3d(point3d1 - item2));
        //    }

        //    if (vector3d.X == 0 & vector3d.Y == 0 & vector3d.Z == 0)
        //        vector3d.Unitize();

        //    return vector3d;
        //}

        public static Vector3d GetNGonNormal(this Mesh mesh, int n) {
            Vector3d vector3d = new Vector3d();

            uint[] faces = mesh.Ngons[n].FaceIndexList();

            for (int i = 0; i < faces.Length; i++)
                vector3d += mesh.FaceNormals[(int)faces[i]];

            vector3d.Unitize();

            return vector3d;
        }

        /// <summary>
        /// Project ngon polyline to average plane from its corner points
        /// Projection direction corresponds with mesh vertex normals
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Polyline[] ProjectNGonsToPlanes(this Mesh mesh, double dist) {
            if (dist != 0) {
                Polyline[] polylines = new Polyline[mesh.Ngons.Count * 2];
                Polyline[] ngonsOutlines = mesh.GetPolylines();
                //Get ngon boundaries
                uint[][] b = mesh.GetNGonsBoundaries();
                Point3f[][] pts = mesh.GetNGonsBoundariesPoint3F(b);
                Vector3d[] normals = mesh.GetNgonNormals();

                for (int i = 0; i < mesh.Ngons.Count; i++) {
                    //1. Average normal and center to get ngon plane
                    Vector3f temp = new Vector3f();

                    //for (int j = 0; j < b[i].Length; j++) {
                    //    temp += mesh.Normals[(int)b[i][j]];
                    //}

                    //if (b[i].Length > 0)
                    //    temp /= b[i].Length;

                    Plane.FitPlaneToPoints(ngonsOutlines[i], out Plane plane);
                    temp = (Vector3f)plane.ZAxis;//wrong
                    temp = (Vector3f)normals[i];

                    Plane p = new Plane(mesh.Ngons.GetNgonCenter(i), temp);
                    p.Translate(p.ZAxis * dist * -0.5);

                    Plane p2 = new Plane(mesh.Ngons.GetNgonCenter(i), temp);
                    p2.Translate(p2.ZAxis * dist * 0.5);

                    //2. Project boundary vertices - Point3d to plane
                    Polyline po = new Polyline();
                    Polyline po2 = new Polyline();

                    for (int j = 0; j < b[i].Length; j++) {
                        Point3f ptA = new Point3f(pts[i][j].X, pts[i][j].Y, pts[i][j].Z);
                        Point3f ptB = new Point3f(pts[i][j].X, pts[i][j].Y, pts[i][j].Z);

                        ptA.Transform(Transform.Translation(-1000000 * mesh.Normals[(int)b[i][j]]));
                        ptB.Transform(Transform.Translation(1000000 * mesh.Normals[(int)b[i][j]]));

                        Line line = new Line(ptA, ptB);

                        bool flag = Rhino.Geometry.Intersect.Intersection.LinePlane(line, p, out double t);
                        bool flag2 = Rhino.Geometry.Intersect.Intersection.LinePlane(line, p2, out double t2);

                        if (!flag || !flag2)
                            Rhino.RhinoApp.WriteLine("missed");

                        po.Add(line.PointAt(t));
                        po2.Add(line.PointAt(t2));
                    }

                    po.Add(po[0]); //add end points
                    po2.Add(po2[0]); //add end points

                    //3. Add to polyline array
                    polylines[i] = po;
                    polylines[i + mesh.Ngons.Count] = po2;
                }

                return polylines;
            } else {
                Polyline[] polylines = new Polyline[mesh.Ngons.Count];
                //Get ngon boundaries
                uint[][] b = mesh.GetNGonsBoundaries();
                Point3f[][] pts = mesh.GetNGonsBoundariesPoint3F(b);
                Polyline[] ngonsOutlines = mesh.GetPolylines();

                for (int i = 0; i < mesh.Ngons.Count; i++) {
                    //1. Average normal and center to get ngon plane
                    Vector3f temp = new Vector3f();

                    //for (int j = 0; j < b[i].Length; j++) {
                    //    temp += mesh.Normals[(int)b[i][j]];
                    //}

                    //if (b[i].Length > 0)
                    //    temp /= b[i].Length;

                    Plane.FitPlaneToPoints(ngonsOutlines[i], out Plane plane);
                    temp = (Vector3f)plane.ZAxis;

                    Plane p = new Plane(mesh.Ngons.GetNgonCenter(i), temp);
                    p = plane;
                    //p.Translate(p.ZAxis * dist * -0.5);

                    //2. Project boundary vertices - Point3d to plane
                    Polyline po = new Polyline();

                    for (int j = 0; j < b[i].Length; j++) {
                        Point3f ptA = new Point3f(pts[i][j].X, pts[i][j].Y, pts[i][j].Z);
                        Point3f ptB = new Point3f(pts[i][j].X, pts[i][j].Y, pts[i][j].Z);
                        ptA.Transform(Transform.Translation(-1000000 * mesh.Normals[(int)b[i][j]]));
                        ptB.Transform(Transform.Translation(1000000 * mesh.Normals[(int)b[i][j]]));

                        Line line = new Line(ptA, ptB);

                        bool flag = Rhino.Geometry.Intersect.Intersection.LinePlane(line, p, out double t);

                        if (!flag)
                            Rhino.RhinoApp.WriteLine("missed " + t.ToString());

                        po.Add(line.PointAt(t));
                    }

                    po.Add(po[0]); //add end points

                    //3. Add to polyline array
                    polylines[i] = po;
                }

                return polylines;
            }
        }

        /// <summary>
        /// Face-Face Adjacency
        ///Iterate over mesh faces in ngon and get connected faces
        ///if that face does not belong to the same ngon add to adjacency list
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static List<int>[] GetNGonFaceAdjacency(this Mesh mesh) {
            List<int>[] faceAdj = new List<int>[mesh.Ngons.Count];

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                List<int> faceAdjTemp = new List<int>();
                uint[] meshFaceIdInCurrentNGon = mesh.Ngons[i].FaceIndexList();

                for (int j = 0; j < meshFaceIdInCurrentNGon.Length; j++) {
                    int[] id = mesh.Faces.AdjacentFaces((int)meshFaceIdInCurrentNGon[j]);

                    for (int k = 0; k < id.Length; k++) {
                        int ngonID = mesh.Ngons.NgonIndexFromFaceIndex(id[k]);

                        if (ngonID != i && !faceAdjTemp.Contains(ngonID))
                            faceAdjTemp.Add(ngonID);
                    }
                }
                faceAdj[i] = faceAdjTemp;
            }
            return faceAdj;
        }

        public static List<int>[] GetNgonFaceAdjacencyOrdered(this Mesh mesh) {
            int[][] boundaries = mesh.GetNGonsTopoBoundaries();
            HashSet<int> allE = mesh.GetAllNGonEdges(boundaries);
            int[] allEArray = allE.ToArray();

            //Flavour 1 - edge ngons
            int[][] edgeAdj = mesh.GetNgonsConnectedToNGonsEdges(allE);
            //Flovour 2 - ngon edges
            int[][] nGonTV = mesh.GetNGonsTopoBoundaries();
            int[][] ngonE = mesh.GetNGonFacesEdges(nGonTV);
            //Flavour 3 - plane array
            List<int>[] adj = new List<int>[mesh.Ngons.Count];

            //Get Edge Bisectors
            for (int i = 0; i < mesh.Ngons.Count; i++) {
                adj[i] = new List<int>();
                for (int j = 0; j < ngonE[i].Length; j++) {
                    int mappedEdgeID = Array.IndexOf(allEArray, ngonE[i][j]);
                    int neighborFaceID = (edgeAdj[mappedEdgeID][0] == i) ? edgeAdj[mappedEdgeID][1] : edgeAdj[mappedEdgeID][0];

                    if (neighborFaceID > -1)
                        adj[i].Add(neighborFaceID);
                }
            }

            return adj;
        }

        /// <summary>
        /// Mesh faces in Ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static uint[][] GetFacesInNGons(this Mesh mesh) {
            uint[][] facesInNgons = new uint[mesh.Ngons.Count][];

            for (int i = 0; i < mesh.Ngons.Count; i++)
                facesInNgons[i] = mesh.Ngons[i].FaceIndexList().ToArray();

            return facesInNgons;
        }

        /// <summary>
        /// Mesh faces as Polylines in Ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="facesInNgons"></param>
        /// <returns></returns>
        public static Polyline[][] GetFacePolylinesInNgons(this Mesh mesh, uint[][] facesInNgons) {
            Polyline[][] polylines = new Polyline[facesInNgons.Length][];

            for (int i = 0; i < facesInNgons.Length; i++) {
                polylines[i] = new Polyline[facesInNgons[i].Length];
                for (int j = 0; j < facesInNgons[i].Length; j++) {
                    int faceID = (int)facesInNgons[i][j];
                    MeshFace mf = mesh.Faces[faceID];

                    if (mf.IsQuad)
                        polylines[i][j] = new Polyline(new Point3d[] { mesh.Vertices[mf.A], mesh.Vertices[mf.B], mesh.Vertices[mf.C], mesh.Vertices[mf.D], mesh.Vertices[mf.A] });
                    else
                        polylines[i][j] = new Polyline(new Point3d[] { mesh.Vertices[mf.A], mesh.Vertices[mf.B], mesh.Vertices[mf.C], mesh.Vertices[mf.A] });
                }
            }

            return polylines;
        }

        /// <summary>
        /// Get mesh edge indices in ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="nGonTV"></param>
        /// <returns></returns>
        public static int[][] GetNGonFacesEdges(this Mesh mesh, int[][] nGonTV) {
            int[][] nGonE = new int[mesh.Ngons.Count][];

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                int n = mesh.Ngons[i].BoundaryVertexCount;
                nGonE[i] = new int[n];

                for (int j = 0; j < n - 1; j++)
                    nGonE[i][j] = mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][j], nGonTV[i][j + 1]);

                int tempEId = mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][0], nGonTV[i][n - 1]);
                nGonE[i][n - 1] = tempEId;
            }

            return nGonE;
        }

        /// <summary>
        /// Get flatten array of mesh edge indices in ngons(hashset)
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="nGonTV"></param>
        /// <returns></returns>
        public static HashSet<int> GetAllNGonEdges(this Mesh mesh, int[][] nGonTV) {
            HashSet<int> allNGonsE = new HashSet<int>();
            try {
                for (int i = 0; i < mesh.Ngons.Count; i++) {
                    int n = mesh.Ngons[i].BoundaryVertexCount;

                    for (int j = 0; j < n - 1; j++)
                        allNGonsE.Add(mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][j], nGonTV[i][j + 1]));

                    allNGonsE.Add(mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][0], nGonTV[i][n - 1]));
                }
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return allNGonsE;
        }

        public static Tuple<int[][], HashSet<int>> GetNGonEdgesIDAndHashSet(this Mesh mesh, int[][] nGonTV) {
            int[][] nGonE = new int[mesh.Ngons.Count][];
            HashSet<int> allNGonsE = new HashSet<int>();

            for (int i = 0; i < mesh.Ngons.Count; i++) {
                int n = mesh.Ngons[i].BoundaryVertexCount;
                nGonE[i] = new int[n];

                for (int j = 0; j < n - 1; j++) {
                    int temp = mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][j], nGonTV[i][j + 1]);
                    allNGonsE.Add(temp);
                    nGonE[i][j] = temp;
                }

                int tempEId = mesh.TopologyEdges.GetEdgeIndex(nGonTV[i][0], nGonTV[i][n - 1]);
                allNGonsE.Add(tempEId);
                nGonE[i][n - 1] = tempEId;
            }

            return new Tuple<int[][], HashSet<int>>(nGonE, allNGonsE);
        }

        /// <summary>
        /// Lines Get mesh edge indices in ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="nGonEdgeId"></param>
        /// <returns></returns>
        public static Line[][] GetNGonFacesEdgesLines(this Mesh mesh, int[][] nGonEdgeId) {
            Line[][] lines = new Line[nGonEdgeId.Length][];

            for (int i = 0; i < nGonEdgeId.Length; i++) {
                lines[i] = new Line[nGonEdgeId[i].Length];

                for (int j = 0; j < nGonEdgeId[i].Length; j++)
                    lines[i][j] = mesh.TopologyEdges.EdgeLine(nGonEdgeId[i][j]);
            }

            return lines;
        }

        public static Point3d[][][] GetNGonFacesEdgesLinesPoints(this Mesh mesh, int[][] nGonEdgeId, double dist) {
            Point3d[][] points = new Point3d[mesh.TopologyEdges.Count][];
            for (int i = 0; i < mesh.TopologyEdges.Count; i++) {
                Line line = mesh.TopologyEdges.EdgeLine(i);
                int divisions = (int)Math.Round(line.Length / Math.Abs(dist), 0);
                points[i] = PointUtil.InterpolateLine(line, divisions, dist < 0);
            }

            Line[][] lines = new Line[nGonEdgeId.Length][];
            Point3d[][][] pointsEdges = new Point3d[nGonEdgeId.Length][][];

            for (int i = 0; i < nGonEdgeId.Length; i++) {
                pointsEdges[i] = new Point3d[nGonEdgeId[i].Length][];

                for (int j = 0; j < nGonEdgeId[i].Length; j++)
                    pointsEdges[i][j] = points[nGonEdgeId[i][j]];
            }

            return pointsEdges;
        }

        /// <summary>
        /// Lines - Get flatten array of mesh edge indices in ngons(hashset)
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonsEdgeId"></param>
        /// <returns></returns>
        public static Line[] GetAllNGonEdgesLines(this Mesh mesh, HashSet<int> allNGonsEdgeId) {
            Line[] lines = new Line[allNGonsEdgeId.Count];

            int n = 0;
            foreach (int i in allNGonsEdgeId) {
                lines[n] = mesh.TopologyEdges.EdgeLine(i);
                n++;
            }

            return lines;
        }

        /// <summary>
        /// Ngons connected to edge - Important the order is according to ngon all edges ID
        /// Always remap to mesh edge ID -> int currentEdge = Array.IndexOf(allEArrayofNGons, currentMeshEdge);
        /// If there is only one ngon connected, -1 add to the beggining indicating naked edge
        /// Flag is used to indicate if edge is naked
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonsEdges"></param>
        /// <param name="flag"></param>
        /// <returns></returns>
        public static int[][] GetNgonsConnectedToNGonsEdges(this Mesh mesh, HashSet<int> allNGonsEdges, bool flag = true) {
            int[][] ngonsConnectedToEdge = new int[allNGonsEdges.Count][];

            int it = 0;

            if (flag) {
                foreach (int e in allNGonsEdges) {
                    int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

                    if (connectedFaces.Length == 1)
                        ngonsConnectedToEdge[it] = new int[2] { -1, mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[0]) };
                    else {
                        ngonsConnectedToEdge[it] = new int[connectedFaces.Length];
                        for (int i = 0; i < connectedFaces.Length; i++)
                            ngonsConnectedToEdge[it][i] = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
                    }
                    it++;
                }
            } else {
                foreach (int e in allNGonsEdges) {
                    int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

                    List<int> temp = new List<int>();

                    for (int i = 0; i < connectedFaces.Length; i++) {
                        int f = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
                        if (f > -1)
                            temp.Add(f);
                    }

                    ngonsConnectedToEdge[it] = temp.ToArray();

                    it++;
                }
            }
            //Rhino.RhinoApp.WriteLine(ngonsConnectedToEdge.Length.ToString());
            return ngonsConnectedToEdge;
        }

        /// <summary>
        ///Returns connected faces to an edge and edge local index in that face
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonsEdges"></param>
        /// <param name="flag"></param>
        /// <returns></returns>
        public static Tuple<int, int>[][] GetEF_LocalID(this Mesh mesh, HashSet<int> allNGonsEdges) {
            Tuple<int, int>[][] ngonsConnectedToEdge = new Tuple<int, int>[allNGonsEdges.Count][];

            int[][] fe = mesh.GetNGonFacesEdges(mesh.GetNGonsTopoBoundaries());

            int it = 0;

            //foreach (int e in allNGonsEdges) {
            //    int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

            //    List<Tuple<int, int>> temp = new List<Tuple<int, int>>();

            //    for (int i = 0; i < connectedFaces.Length; i++) {
            //        int f = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
            //        if (f > -1)
            //            temp.Add(new Tuple<int,int>(f,f));
            //    }

            //    ngonsConnectedToEdge[it] = temp.ToArray();

            //    it++;
            //}

            foreach (int e in allNGonsEdges) {
                int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

                List<Tuple<int, int>> temp = new List<Tuple<int, int>>();

                for (int i = 0; i < connectedFaces.Length; i++) {
                    int f = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);

                    if (f > -1) {
                        int[] faceEdges = fe[f];
                        int localEdgeID = Array.IndexOf(faceEdges, e);
                        temp.Add(new Tuple<int, int>(f, localEdgeID));
                    }
                }

                ngonsConnectedToEdge[it] = temp.ToArray();

                it++;
            }

            //Rhino.RhinoApp.WriteLine(ngonsConnectedToEdge.Length.ToString());
            return ngonsConnectedToEdge;
        }

        /// <summary>
        /// flag false return one element for naked edge true -1
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonsEdges"></param>
        /// <param name="flag"></param>
        /// <returns></returns>
        public static Dictionary<int, int[]> GetFE(this Mesh mesh, HashSet<int> allNGonsEdges, bool flag = true) {
            //int[][] ngonsConnectedToEdge = new int[allNGonsEdges.Count][];
            Dictionary<int, int[]> ngonsConnectedToEdge = new Dictionary<int, int[]>();

            int it = 0;

            if (flag) {
                foreach (int e in allNGonsEdges) {
                    int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);

                    if (connectedFaces.Length == 1)
                        ngonsConnectedToEdge.Add(e, new int[2] { -1, mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[0]) });
                    else {
                        int[] cf = new int[connectedFaces.Length];
                        for (int i = 0; i < connectedFaces.Length; i++)
                            ngonsConnectedToEdge[it][i] = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
                        ngonsConnectedToEdge.Add(e, cf);
                    }
                    it++;
                }
            } else {
                foreach (int e in allNGonsEdges) {
                    int[] connectedFaces = mesh.TopologyEdges.GetConnectedFaces(e);
                    List<int> temp = new List<int>();

                    for (int i = 0; i < connectedFaces.Length; i++) {
                        int f = mesh.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);
                        if (f > -1)
                            temp.Add(f);
                    }

                    //ngonsConnectedToEdge[it] = temp.ToArray();
                    ngonsConnectedToEdge.Add(e, temp.ToArray());
                    it++;
                }
            }
            //Rhino.RhinoApp.WriteLine(ngonsConnectedToEdge.Length.ToString());
            return ngonsConnectedToEdge;
        }

        ///  <summary>
        /// Get connected ngons to vertices
        /// Also works by looping through ngons topology vertices
        /// Get their connected faces and map to Ngons
        ///  </summary>
        ///  <param name="mesh"></param>
        ///  <param name="allNGonTv"></param>
        /// <param name="flag"></param>
        /// <returns></returns>
        public static int[][] GetNGonsConnectedToNGonTopologyVertices(this Mesh mesh, HashSet<int> allNGonTv, bool flag = true) {
            int[][] nGonsConnectedToTv = new int[allNGonTv.Count][];

            bool[] nakedNGonVertices = mesh.GetNakedNGonTopologyPointStatus(allNGonTv); //this is to test if vertex is naked

            int iv = 0;
            foreach (int i in allNGonTv) {
                int[] connectedFaces = mesh.TopologyVertices.ConnectedFaces(i);

                HashSet<int> connectedNGons = new HashSet<int>();

                foreach (int j in connectedFaces)
                    connectedNGons.Add(mesh.Ngons.NgonIndexFromFaceIndex(j));

                if (flag && nakedNGonVertices[iv])
                    nGonsConnectedToTv[iv] = null;
                else
                    nGonsConnectedToTv[iv] = connectedNGons.ToArray();

                iv++;
            }

            return nGonsConnectedToTv;
        }

        ///  <summary>
        /// Get connected ngons to vertices
        /// Also works by looping through ngons topology vertices
        /// Get their connected faces and map to Ngons
        ///  </summary>
        ///  <param name="mesh"></param>
        ///  <param name="allNGonTv"></param>
        /// <param name="flag"></param>
        /// <returns></returns>
        public static Dictionary<int, int[]> GetNGonsConnectedToNGonTopologyVerticesDictionary(this Mesh mesh, HashSet<int> allNGonTv, bool flag = true) {
            Dictionary<int, int[]> nGonsConnectedToTv = new Dictionary<int, int[]>(allNGonTv.Count);
            //int[][] nGonsConnectedToTv = new int[allNGonTv.Count][];

            bool[] nakedNGonVertices = mesh.GetNakedNGonTopologyPointStatus(allNGonTv); //this is to test if vertex is naked

            int iv = 0;
            foreach (int i in allNGonTv) {
                int[] connectedFaces = mesh.TopologyVertices.ConnectedFaces(i);

                HashSet<int> connectedNGons = new HashSet<int>();

                foreach (int j in connectedFaces)
                    connectedNGons.Add(mesh.Ngons.NgonIndexFromFaceIndex(j));

                //if (flag && nakedNGonVertices[iv])
                // nGonsConnectedToTv[iv] = null;
                // else
                //nGonsConnectedToTv[iv] = connectedNGons.ToArray();
                nGonsConnectedToTv.Add(i, connectedNGons.ToArray());

                iv++;
            }

            return nGonsConnectedToTv;
        }

        /// <summary>
        ///Get connected ngons to vertices
        ///Also works by looping through ngons vertices
        ///Get their connected faces and map to Ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonV"></param>
        /// <returns></returns>
        public static int[][] GetNGonsConnectedToNGonVertices(this Mesh mesh, HashSet<int> allNGonV) {
            int[][] nGonsConnectedToV = new int[allNGonV.Count][];

            int iv = 0;
            foreach (int i in allNGonV) {
                int[] connectedFaces = mesh.Vertices.GetVertexFaces(i);
                HashSet<int> connectedNGons = new HashSet<int>();

                foreach (int j in connectedFaces)
                    connectedNGons.Add(mesh.Ngons.NgonIndexFromFaceIndex(j));
                nGonsConnectedToV[iv] = connectedNGons.ToArray();

                iv++;
            }

            return nGonsConnectedToV;
        }

        /// <summary>
        /// Get naked topology vertices
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        private static bool[] GetNakedTopologyPointStatus(this Mesh mesh) {
            bool[] nakedTopologyVertices = new bool[mesh.TopologyVertices.Count];

            bool[] nakedVertices = mesh.GetNakedEdgePointStatus();

            for (int i = 0; i < mesh.TopologyVertices.Count; i++)
                nakedTopologyVertices[i] = nakedVertices[mesh.TopologyVertices.MeshVertexIndices(i)[0]];

            return nakedTopologyVertices;
        }

        /// <summary>
        /// Naked Ngons Topology Vertices
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="tvAll"></param>
        /// <returns></returns>
        public static bool[] GetNakedNGonTopologyPointStatus(this Mesh mesh, HashSet<int> tvAll) {
            bool[] nakedNGonTopologyVertices = new bool[tvAll.Count];
            bool[] nakedTopologyVertices = mesh.GetNakedTopologyPointStatus();

            int i = 0;
            foreach (int j in tvAll) {
                nakedNGonTopologyVertices[i] = nakedTopologyVertices[j];
                i++;
            }

            return nakedNGonTopologyVertices;
        }

        /// <summary>
        /// Naked Ngons Topology Vertices
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="tvAll"></param>
        /// <returns></returns>
        public static bool[] GetNakedNGonPointStatus(this Mesh mesh, HashSet<int> vAll) {
            bool[] nakedNGonVertices = new bool[vAll.Count];
            bool[] nakedVertices = mesh.GetNakedEdgePointStatus();

            int i = 0;
            foreach (int j in vAll) {
                nakedNGonVertices[i] = nakedVertices[j];
                i++;
            }

            return nakedNGonVertices;
        }

        /// <summary>
        ///Get ngons edges connected to vertices
        ///Does not work yet because it gets all edges
        ///But what is needed only ngon edges
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="allNGonTv"></param>
        /// <param name="allNGonsE"></param>
        /// <returns></returns>
        public static int[][] GetConnectedNGonEdgesToNGonTopologyVertices(this Mesh mesh, HashSet<int> allNGonTv, HashSet<int> allNGonsE) {
            int[][] nGonEdgesConnectedToNGonTv = new int[allNGonTv.Count][];

            int ie = 0;
            foreach (int i in allNGonTv) {
                mesh.TopologyVertices.SortEdges(i);
                int[] connectedEdges = mesh.TopologyVertices.ConnectedEdges(i);
                // nGonsConnectedToTV[iv] = new int[connectedFaces.Count];
                HashSet<int> connectedNGonsEdges = new HashSet<int>();

                foreach (int j in connectedEdges)
                    if (allNGonsE.Contains(j))
                        connectedNGonsEdges.Add(j);

                nGonEdgesConnectedToNGonTv[ie] = connectedNGonsEdges.ToArray();
                ie++;
            }

            return nGonEdgesConnectedToNGonTv;
        }

        ///  <summary>
        /// Get ngons lines connected to vertices
        /// Does not work yet because it gets all edges
        /// But what is needed only ngon edges
        ///  </summary>
        ///  <param name="mesh"></param>
        /// <param name="connectedToTopoVerticesNGonEdges"></param>
        /// <returns></returns>
        public static Line[][] GetConnectedNGonLinesToNGonTopologyVertices(this Mesh mesh, int[][] connectedToTopoVerticesNGonEdges) {
            Line[][] lines = new Line[connectedToTopoVerticesNGonEdges.Length][];

            for (int i = 0; i < connectedToTopoVerticesNGonEdges.Length; i++) {
                lines[i] = new Line[connectedToTopoVerticesNGonEdges[i].Length];

                for (int j = 0; j < connectedToTopoVerticesNGonEdges[i].Length; j++)
                    lines[i][j] = mesh.TopologyEdges.EdgeLine(connectedToTopoVerticesNGonEdges[i][j]);
            }

            return lines;
        }

        public static Point3d CenterPoint(Point3d[] polyline) {
            int Count = polyline.Length;

            if (Count == 0) { return Point3d.Unset; }

            if (Count == 1) { return polyline[0]; }

            Point3d center = Point3d.Origin;

            double weight = 0.0;

            int stop = (Count - 1);
            if (polyline[0].DistanceToSquared(polyline[polyline.Length - 1]) > 0.001) {
                //Rhino.RhinoApp.WriteLine(polyline[0].DistanceToSquared(polyline[polyline.Length - 1]).ToString());
                stop++;
            }
            for (int i = 0; i < stop; i++) {
                Point3d A = polyline[i];

                Point3d B = polyline[(i + 1) % Count];

                double d = A.DistanceTo(B);

                center += d * 0.5 * (A + B);

                weight += d;
            }

            center /= weight;

            return center;
        }

        public static Point3d CenterPoint(Polyline polyline) {
            int Count = polyline.Count;

            if (Count == 0) { return Point3d.Unset; }

            if (Count == 1) { return polyline[0]; }

            Point3d center = Point3d.Origin;

            double weight = 0.0;

            int stop = (Count - 1);
            if (polyline[0].DistanceToSquared(polyline[polyline.Count - 1]) > 0.001) {
                //Rhino.RhinoApp.WriteLine(polyline[0].DistanceToSquared(polyline[polyline.Length - 1]).ToString());
                stop++;
            }
            for (int i = 0; i < stop; i++) {
                Point3d A = polyline[i];

                Point3d B = polyline[(i + 1) % Count];

                double d = A.DistanceTo(B);

                center += d * 0.5 * (A + B);

                weight += d;
            }

            center /= weight;

            return center;
        }

        public static Point3d MeshFaceCenter(this Mesh mesh, int i) {
            Point3d center = new Point3d();

            if (mesh.Faces[i].IsQuad) {
                Point3d[] pts = new Point3d[] { mesh.Vertices[mesh.Faces[i].A], mesh.Vertices[mesh.Faces[i].B], mesh.Vertices[mesh.Faces[i].C], mesh.Vertices[mesh.Faces[i].D] };
                center = CenterPoint(pts);
                //Point3d c = (mesh.Vertices[mesh.Faces[i].A] + mesh.Vertices[mesh.Faces[i].B] + mesh.Vertices[mesh.Faces[i].C] + mesh.Vertices[mesh.Faces[i].D]);
                //center = new Point3d(c.X * 0.25, c.Y * 0.25, c.Z * 0.25);
            } else {
                //Point3d c = (mesh.Vertices[mesh.Faces[i].A] + mesh.Vertices[mesh.Faces[i].B] + mesh.Vertices[mesh.Faces[i].C]);
                //center = new Point3d(c.X / 3, c.Y / 3, c.Z / 3);
                Point3d[] pts = new Point3d[] { mesh.Vertices[mesh.Faces[i].A], mesh.Vertices[mesh.Faces[i].B], mesh.Vertices[mesh.Faces[i].C] };
                center = CenterPoint(pts);
            }
            return center;
        }

        public static Point3d[] MeshFaceCenters(this Mesh mesh) {
            Point3d[] centers = new Point3d[mesh.Faces.Count];

            for (int i = 0; i < mesh.Faces.Count; i++) {
                if (mesh.Faces[i].IsQuad) {
                    //Point3d c = (mesh.Vertices[mesh.Faces[i].A] + mesh.Vertices[mesh.Faces[i].B] +
                    //             mesh.Vertices[mesh.Faces[i].C] + mesh.Vertices[mesh.Faces[i].D]);
                    //centers[i] = new Point3d(c.X * 0.25, c.Y * 0.25, c.Z * 0.25);
                    Point3d[] pts = new Point3d[] { mesh.Vertices[mesh.Faces[i].A], mesh.Vertices[mesh.Faces[i].B], mesh.Vertices[mesh.Faces[i].C], mesh.Vertices[mesh.Faces[i].D] };
                    centers[i] = CenterPoint(pts);
                } else {
                    //       Point3d c = (mesh.Vertices[mesh.Faces[i].A] + mesh.Vertices[mesh.Faces[i].B] +
                    //mesh.Vertices[mesh.Faces[i].C]);
                    //       centers[i] = new Point3d(c.X / 3, c.Y / 3, c.Z / 3);
                    Point3d[] pts = new Point3d[] { mesh.Vertices[mesh.Faces[i].A], mesh.Vertices[mesh.Faces[i].B], mesh.Vertices[mesh.Faces[i].C] };
                    centers[i] = CenterPoint(pts);
                }
            }
            return centers;
        }

        public static Tuple<bool[], List<Line>, HashSet<int>, int[][], Dictionary<int, Polyline>, Dictionary<int, bool>> FindMeshEdgeByNGonPair(this Mesh mesh, int[][] ngonPairs) {
            //Get ngon boundaries
            int[][] tv = mesh.GetNGonsTopoBoundaries();
            //HashSet<int> allNGonsEdges = mesh.GetAllNGonEdges(tv);
            var edges = mesh.GetNGonEdgesIDAndHashSet(tv);
            Dictionary<int, Polyline> dict = new Dictionary<int, Polyline>();
            Dictionary<int, bool> dictFlags = new Dictionary<int, bool>();

            //Get ngon to face pairs
            int it = 0;

            bool[] edgesFlag = new bool[edges.Item2.Count];

            //Geometry
            List<Line> lines = new List<Line>();

            foreach (int e in edges.Item2) {
                //Get mesh faces pairs
                int[] connectedMeshFaces = mesh.TopologyEdges.GetConnectedFaces(e);

                //if not naked
                if (connectedMeshFaces.Length == 2) {
                    int a = mesh.Ngons.NgonIndexFromFaceIndex(connectedMeshFaces[0]);
                    int b = mesh.Ngons.NgonIndexFromFaceIndex(connectedMeshFaces[1]);

                    //Translation: Set flag to true if Any member of ngonPairs is a list of elements that is exactly the same as the elements in this array of integers
                    bool flag = ngonPairs.Any(p => p.SequenceEqual(new int[2] { a, b }));

                    if (flag == false)
                        flag = ngonPairs.Any(p => p.SequenceEqual(new int[2] { b, a }));

                    if (flag) {
                        edgesFlag[it] = true;
                        dictFlags.Add(edges.Item2.ElementAt(it), true);
                        dict.Add(edges.Item2.ElementAt(it), new Polyline());
                    } else {
                        lines.Add(mesh.TopologyEdges.EdgeLine(edges.Item2.ElementAt(it)));
                        //dict.Add(edges.Item2.ElementAt(it), new Polyline());
                        //dictFlags.Add(edges.Item2.ElementAt(it), false);
                        edgesFlag[it] = false;
                    }
                }

                it++;
            }

            return new Tuple<bool[], List<Line>, HashSet<int>, int[][], Dictionary<int, Polyline>, Dictionary<int, bool>>(edgesFlag, lines, edges.Item2, edges.Item1, dict, dictFlags);
        }

        /// <summary>
        /// Weld mesh vertices by distance (uses RTree)
        /// </summary>
        /// <param name="M"></param>
        /// <param name="radius"></param>
        /// <returns></returns>
        public static Mesh WeldUsingRTree(this Mesh M_, double radius, bool run = false, bool healNakedEdges = false) {
            Mesh M = M_.DuplicateMesh();

            if (healNakedEdges)
                M.HealNakedEdges(radius);

            if (run) {
                //Store naked mesh vertices and their ids
                List<Point3f> pts = new List<Point3f>();
                bool[] flag = M.GetNakedEdgePointStatus();

                allPointsFound = new List<List<int>>();

                //History is need to stop searching rtee
                //When all point groups are found
                history.Clear();

                //Create Rtree
                RTree tree = new RTree();

                for (int i = 0; i < flag.Length; i++) {
                    if (flag[i]) {
                        tree.Insert(M.Vertices[i], i);
                        pts.Add(M.Vertices[i]);
                    }
                }

                //Search RTree
                for (int i = 0; i < pts.Count; i++) {
                    tempIDList = new List<int>();
                    tree.Search(new Sphere(pts[i], radius), Method);

                    if (tempIDList.Count > 1)
                        allPointsFound.Add(tempIDList);

                    if (history.Count == M.Vertices.Count)
                        break;
                }

                //Now move found vertices to one of first vertex position
                foreach (List<int> i in allPointsFound)
                    for (int j = 1; j < i.Count; j++)
                        M.Vertices[i[j]] = M.Vertices[i[0]];
            }

            M.Clean();

            return M;
        }

        //Rtree collections
        private static List<int> history = new List<int>();

        private static List<List<int>> allPointsFound;
        private static List<int> tempIDList = new List<int>();

        private static void Method(object sender, RTreeEventArgs e) {
            if (!history.Contains(e.Id)) {
                tempIDList.Add(e.Id);
                history.Add(e.Id);
            }
        }

        public static Mesh[] ExtrudeMesh(this Mesh m, float dist, bool flag, bool cap, bool bothsides) {
            if (!flag) {
                int[][] id = m.GetNGonsTopoBoundaries();
                Mesh[] extrusion = new Mesh[id.Length];

                for (int i = 0; i < id.Length; i++) {
                    Mesh t = new Mesh();

                    t.Append(m.MeshFromTwoTopoVertices(id[i][0], id[i][id[i].Length - 1], dist, bothsides));

                    for (int j = 1; j < id[i].Length; j++)
                        t.Append(m.MeshFromTwoTopoVertices(id[i][j], id[i][j - 1], dist, bothsides));

                    //cap

                    if (cap) {
                        Polyline[] p = new Polyline[] { new Polyline(), new Polyline() };

                        bool loopFlag = true;

                        for (int j = 0; j < t.Vertices.Count; j += 2) {
                            if (loopFlag)
                                p[0].Add(t.Vertices[j]);
                            else
                                p[1].Add(t.Vertices[j]);

                            loopFlag = !loopFlag;
                        }

                        p[0].Add(p[0][0]);
                        p[1].Add(p[1][0]);
                        p[1].Reverse();
                        t.Append(MeshCreate.MeshFromPolylines(p, -1));
                    }
                    t.Clean();
                    extrusion[i] = t;
                }

                return extrusion;
            } else {
                //0. Output mesh
                Mesh extrusion = new Mesh();

                //1.Create copy of meshes and offset the,
                Mesh meshA = m.Offset(-dist);   //meshA.Flip(true, true, true);
                Mesh meshB = new Mesh();

                if (bothsides)
                    meshB = m.Offset(dist);
                else
                    meshB = m;

                //2. If cap is needed

                extrusion.Append(meshA);
                extrusion.Append(meshB);
                //}

                //to removes faces afterwards
                int faces = extrusion.Faces.Count;

                //3 Draw faces in between offset meshes

                int[][] tv = extrusion.GetNGonsTopoBoundaries();
                var edges = extrusion.GetAllNGonEdges(tv).ToArray();
                var te = extrusion.TopologyEdges;
                int n = (int)(edges.Length * 0.5);

                int[][] v = new int[n][];

                //4. Store edges tp id, only works in this way...
                for (int i = 0; i < n; i++) {
                    Rhino.IndexPair idA = te.GetTopologyVertices(edges[i]);
                    Rhino.IndexPair idB = te.GetTopologyVertices(edges[i + (int)(edges.Length * 0.5)]);

                    int a = extrusion.TopologyVertices.MeshVertexIndices(idA.I)[0];
                    int b = extrusion.TopologyVertices.MeshVertexIndices(idA.J)[0];
                    int c = extrusion.TopologyVertices.MeshVertexIndices(idB.I)[0];
                    int d = extrusion.TopologyVertices.MeshVertexIndices(idB.J)[0];

                    v[i] = new int[] { a, b, c, d };
                }

                if (!cap) {
                    Mesh extrusion2 = new Mesh();
                    extrusion2.Vertices.AddVertices(extrusion.Vertices);

                    //5. Add faces by tp vertex ids
                    for (int i = 0; i < n; i++) {
                        extrusion2.Faces.AddFace(v[i][0], v[i][1], v[i][3], v[i][2]);

                        extrusion2.Ngons.AddNgon(MeshNgon.Create(new[] { v[i][0], v[i][1], v[i][3], v[i][2] }, new[] { i }));
                    }
                    extrusion2.Clean();
                    return new Mesh[] { extrusion2 };
                } else {
                    //5. Add faces by tp vertex ids
                    int f = extrusion.Faces.Count;
                    for (int i = 0; i < n; i++) {
                        extrusion.Faces.AddFace(v[i][0], v[i][1], v[i][3], v[i][2]);
                        extrusion.Ngons.AddNgon(MeshNgon.Create(new[] { v[i][0], v[i][1], v[i][3], v[i][2] }, new[] { f + i }));
                    }

                    return new Mesh[] { extrusion };
                }

                ////If not cap remove faces:
                //if (!cap) {
                //    for (int i = 0; i < faces; i++)
                //        extrusion.Faces.RemoveAt(0);

                //    //    extrusion.Ngons.Clear();
                //}

                //  extrusion.WeldUsingRTree(0.01);
            }
        }

        public static Mesh MeshFromTwoTopoVertices(this Mesh m, int aa, int bb, float dist, bool bothsides) {
            Point3d a = m.TopologyVertices[aa];
            Point3d b = m.TopologyVertices[bb];
            a.Transform(Transform.Translation(dist * m.Normals[aa]));
            b.Transform(Transform.Translation(dist * m.Normals[bb]));

            Mesh temp = new Mesh();
            temp.Vertices.Add(a);
            temp.Vertices.Add(b);

            if (!bothsides) {
                temp.Vertices.Add(m.TopologyVertices[bb]);
                temp.Vertices.Add(m.TopologyVertices[aa]);
            } else {
                Point3d c = m.TopologyVertices[aa];
                Point3d d = m.TopologyVertices[bb];
                c.Transform(Transform.Translation(-dist * m.Normals[aa]));
                d.Transform(Transform.Translation(-dist * m.Normals[bb]));
                temp.Vertices.Add(d);
                temp.Vertices.Add(c);
            }

            temp.Faces.AddFace(0, 1, 2, 3);

            MeshNgon ngonS = MeshNgon.Create(new int[] { 0, 1, 2, 3 }, new int[] { 0 });
            temp.Ngons.AddNgon(ngonS);

            return temp;
        }

        /// <summary>
        /// Adjust mesh by other mesh, works only if meshes are very similar.
        /// For instance for planarization or dynamic relaxation meshes, when just mesh vertex positions are changed.
        /// </summary>
        /// <param name="meshToAdjust"></param>
        /// <param name="meshToFollow"></param>
        /// <returns></returns>
        public static Mesh AdjustMesh(this Mesh meshToAdjust, Mesh meshToFollow) {
            if (meshToAdjust.Vertices.Count != meshToFollow.Vertices.Count)
                return null;
            else {
                for (int i = 0; i < meshToAdjust.Vertices.Count; i++)
                    meshToAdjust.Vertices[i] = meshToFollow.Vertices[i];

                return meshToAdjust.DuplicateMesh();
            }
        }

        /// <summary>
        /// Works only for offseted meshes with ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public static Polyline[][] ProjectPlanarizedMesh(this Mesh mesh, double value) {
            //1 Create loft polylines for each pair of ngons
            Polyline[][] plines = new Polyline[(int)(mesh.Ngons.Count * 0.5)][];

            //2. Boundary indices and planes for lines to intersect
            int[][] b = mesh.GetNGonsTopoBoundaries();
            Plane[] planes = mesh.GetNgonPlanes();

            for (int i = 0; i < mesh.Ngons.Count * 0.5; i++) {
                mesh.Ngons.GetNgonCenter(i);

                Polyline[] polylines = new Polyline[2 + b[i].Length];
                polylines[0] = new Polyline();
                polylines[1] = new Polyline();

                //Create line from offseted ngons and intersect with planes
                for (int j = 0; j < b[i].Length; j++) {
                    int idA = b[i][j];
                    int idB = b[i + (int)(mesh.Ngons.Count * 0.5)][j];

                    Point3d ptAO = mesh.TopologyVertices[idA];
                    Point3d ptBO = mesh.TopologyVertices[idB];

                    Line line = new Line(ptAO, ptBO);
                    line.Transform(Transform.Scale(line.PointAt(0.5), 100));

                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i], out double t1);
                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i + (int)(mesh.Ngons.Count * 0.5)], out double t2);

                    Point3d ptA = ptAO + (line.PointAt(t1) - ptAO) * value;
                    Point3d ptB = ptBO + (line.PointAt(t2) - ptBO) * value;

                    polylines[0].Add(ptA);
                    polylines[1].Add(ptB);
                }

                polylines[0].Add(polylines[0][0]);
                polylines[1].Add(polylines[1][0]);

                plines[i] = polylines;
                PolylineUtil.LoftTwoPolylines(plines[i]).CopyTo(plines[i], 2);
            }
            return plines;
        }

        /// <summary>
        /// Scaling two ngons extruded edges until they can be capped
        /// Done by intersecting with ngon planes
        /// Works only for offseted meshes with ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public static Tuple<Polyline[][], int[][][]> ProjectPlanarizedMesh_VertexMapping(this Mesh mesh, double value) {
            //1 Create loft polylines for each pair of ngons
            int half = (int)(mesh.Ngons.Count * 0.5);

            //[]face pair - two hexagons []polylines between those faces []vertex if of ngon in base mesh
            Polyline[][] polylines = new Polyline[half][];
            int[][][] polylinesId = new int[half][][];

            //2. Boundary indices and planes for lines to intersect
            int[][] b = mesh.GetNGonsTopoBoundaries();
            Plane[] planes = mesh.GetNgonPlanes();

            for (int i = 0; i < mesh.Ngons.Count * 0.5; i++) {
                mesh.Ngons.GetNgonCenter(i);

                polylines[i] = new Polyline[2 + b[i].Length];
                polylinesId[i] = new int[2 + b[i].Length][];
                //Id names for face vertices in base mesh face A 0123 faceB 0256 faceC 569875

                polylines[i][0] = new Polyline();
                polylines[i][1] = new Polyline();
                polylinesId[i][0] = b[i];
                polylinesId[i][1] = b[i + half];

                //Create line from offseted ngons and intersect with planes
                for (int j = 0; j < b[i].Length; j++) {
                    //p[j] = new Polyline() { polylines[0][j], polylines[0][j + 1], polylines[1][j + 1], polylines[1][j], polylines[0][j] };
                    if (j < b[i].Length - 1)
                        polylinesId[i][2 + j] = new[] { b[i][j], b[i][j + 1], b[i + half][j + 1], b[i + half][j] };
                    else
                        polylinesId[i][2 + j] = new[] { b[i][j], b[i][0], b[i + half][0], b[i + half][j] };

                    int idA = b[i][j];
                    int idB = b[i + half][j];

                    Point3d ptAo = mesh.TopologyVertices[idA];
                    Point3d ptBo = mesh.TopologyVertices[idB];

                    Line line = new Line(ptAo, ptBo);
                    line.Transform(Transform.Scale(line.PointAt(0.5), 100));

                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i], out double t1);
                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i + half], out double t2);

                    Point3d ptA = ptAo + (line.PointAt(t1) - ptAo) * value;
                    Point3d ptB = ptBo + (line.PointAt(t2) - ptBo) * value;

                    polylines[i][0].Add(ptA);
                    polylines[i][1].Add(ptB);
                }

                polylines[i][0].Add(polylines[i][0][0]);
                polylines[i][1].Add(polylines[i][1][0]);

                PolylineUtil.LoftTwoPolylines(polylines[i]).CopyTo(polylines[i], 2);
            }
            return new Tuple<Polyline[][], int[][][]>(polylines, polylinesId);
        }

        /// <summary>
        /// Dovetaisl
        /// Scaling two ngons extruded edges until they can be capped
        /// Done by intersecting with ngon planes
        /// Works only for offseted meshes with ngons
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public static Polyline[][] ProjectPlanarizedMesh_Dovetails(this Mesh mesh, double value) {
            //1 Create loft polylines for each pair of ngons
            int half = (int)(mesh.Ngons.Count * 0.5);

            //[]face pair - two hexagons []polylines between those faces []vertex if of ngon in base mesh
            Polyline[][] polylines = new Polyline[half][];
            //int[][][] polylinesId = new int[half][][];

            //2. Boundary indices and planes for lines to intersect
            int[][] b = mesh.GetNGonsTopoBoundaries();
            Plane[] planes = mesh.GetNgonPlanes();

            for (int i = 0; i < mesh.Ngons.Count * 0.5; i++) {
                mesh.Ngons.GetNgonCenter(i);

                polylines[i] = new Polyline[2 + b[i].Length];
                //polylinesId[i] = new int[2 + b[i].Length][];
                //Id names for face vertices in base mesh face A 0123 faceB 0256 faceC 569875

                polylines[i][0] = new Polyline();
                polylines[i][1] = new Polyline();
                //polylinesId[i][0] = b[i];
                //polylinesId[i][1] = b[i + half];

                //Create line from offseted ngons and intersect with planes
                for (int j = 0; j < b[i].Length; j++) {
                    //p[j] = new Polyline() { polylines[0][j], polylines[0][j + 1], polylines[1][j + 1], polylines[1][j], polylines[0][j] };
                    //if (j < b[i].Length - 1)
                    //    polylinesId[i][2 + j] = new[] { b[i][j], b[i][j + 1], b[i + half][j + 1], b[i + half][j] };
                    //else
                    //    polylinesId[i][2 + j] = new[] { b[i][j], b[i][0], b[i + half][0], b[i + half][j] };

                    int idA = b[i][j];
                    int idB = b[i + half][j];

                    Point3d ptAo = mesh.TopologyVertices[idA];
                    Point3d ptBo = mesh.TopologyVertices[idB];

                    Line line = new Line(ptAo, ptBo);
                    line.Transform(Transform.Scale(line.PointAt(0.5), 100));

                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i], out double t1);
                    Rhino.Geometry.Intersect.Intersection.LinePlane(line, planes[i + half], out double t2);

                    Point3d ptA = ptAo + (line.PointAt(t1) - ptAo) * value;
                    Point3d ptB = ptBo + (line.PointAt(t2) - ptBo) * value;

                    polylines[i][0].Add(ptA);
                    polylines[i][1].Add(ptB);
                }

                polylines[i][0].Add(polylines[i][0][0]);
                polylines[i][1].Add(polylines[i][1][0]);

                PolylineUtil.LoftTwoPolylines(polylines[i]).CopyTo(polylines[i], 2);
            }
            return polylines;
        }

        public static Polyline[] ProjectPlanarizedMesh_EdgeMapping(this Mesh mesh, double value) {
            //1.Get hashset of ngons edges
            //Because we have offset surface edges match by indexing: e[i] to e[i + e.Count*0.5]
            int[][] b = mesh.GetNGonsTopoBoundaries();
            HashSet<int> e = mesh.GetAllNGonEdges(b);
            Line[] l = mesh.GetAllNGonEdgesLines(e);
            Plane[] planes = mesh.GetNgonPlanes(); //for project of cross lines
            int[][] nGons = mesh.GetNgonsConnectedToNGonsEdges(e, false); //for adjacency to intersec planes

            int n = (int)(e.Count * 0.5);
            //Polyline[][] polylines = new Polyline[n*2][];
            Polyline[] polylines = new Polyline[n];

            for (int i = 0; i < n; i++) {
                //Retrieve two relative edges
                int tE = e.ElementAt(i);
                int bE = e.ElementAt(i + n);
                Line lnA = mesh.TopologyEdges.EdgeLine(tE);
                Line lnB = mesh.TopologyEdges.EdgeLine(bE);

                //Get end points to create side lines
                Point3d ptA_TE = lnA.From;
                Point3d ptB_TE = lnA.To;
                Point3d ptC_BE = lnB.From;
                Point3d ptD_BE = lnB.To;

                Line left = new Line(ptA_TE, ptC_BE);
                Line right = new Line(ptB_TE, ptD_BE);
                left.Transform(Transform.Scale(left.PointAt(0.5), 1000));
                right.Transform(Transform.Scale(right.PointAt(0.5), 1000));

                //Get adjacent faces to edges
                int[] tENgons = nGons[i];
                int[] bENgons = nGons[i + n];

                //polylines[i] = new Polyline(new []{lnA.From,lnA.To,lnB.To,lnB.From,lnA.From});
                //polylines[i] = new Polyline[tENgons.Length];
                //polylines[i+n] = new Polyline[bENgons.Length];

                double[] tLeft = new double[tENgons.Length + bENgons.Length];
                double[] tRight = new double[tENgons.Length + bENgons.Length];

                for (int j = 0; j < tENgons.Length; j++) {
                    Plane p = planes[tENgons[j]];
                    //polylines[i][j] = new Polyline(new[] {lnA.From, p.Origin, lnA.To, lnA.From});
                    bool flag1 = Rhino.Geometry.Intersect.Intersection.LinePlane(left, p, out tLeft[j]);
                    bool flag2 = Rhino.Geometry.Intersect.Intersection.LinePlane(right, p, out tRight[j]);
                }

                for (int j = 0; j < bENgons.Length; j++) {
                    Plane p = planes[bENgons[j]];
                    //polylines[i + n][j] = new Polyline(new[] {lnB.From, p.Origin, lnB.To, lnB.From});
                    bool flag1 =
                        Rhino.Geometry.Intersect.Intersection.LinePlane(left, p, out tLeft[j + tENgons.Length]);
                    bool flag2 =
                        Rhino.Geometry.Intersect.Intersection.LinePlane(right, p, out tRight[j + tENgons.Length]);
                }

                Array.Sort(tLeft);
                Array.Sort(tRight);

                polylines[i] = new Polyline(new[]
                {
                    left.PointAt(tLeft[0]),
                    left.PointAt(tLeft.Last()),
                    right.PointAt(tRight.Last()),
                    right.PointAt(tRight[0]),
                    left.PointAt(tLeft[0])
                });
            }
            return polylines;
        }

        /// <summary>
        /// Ngons offset
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="dist"></param>
        /// <returns></returns>
        public static Tuple<Polyline[][], Polyline[]> OffsetNgon(this Mesh mesh, double dist) {
            int[][] tv = mesh.GetNGonsTopoBoundaries();
            Point3f[][] ba = mesh.GetNGonsTopoBoundariesPoint3F(tv);
            Point3f[][] bb = mesh.GetNGonsTopoBoundariesPoint3F(tv);
            Point3d[] c = mesh.GetNGonCenters();

            //output
            Polyline[][] p = new Polyline[ba.Length][];
            Polyline[] p2 = new Polyline[ba.Length];

            for (int i = 0; i < bb.Length; i++)
                for (int j = 0; j < bb[i].Length; j++)
                    bb[i][j].Transform(Transform.Scale(c[i], dist));

            for (int i = 0; i < bb.Length; i++) {
                int n = bb[i].Length;
                p[i] = new Polyline[n];

                Point3d[] pts = new Point3d[n + 1];

                for (int j = 0; j < n - 1; j++) {
                    p[i][j] = new Polyline { new Point3d(bb[i][j]), new Point3d(bb[i][j + 1]), new Point3d(ba[i][j + 1]), new Point3d(ba[i][j]), new Point3d(bb[i][j]) };
                    pts[j] = bb[i][j];
                }

                p[i][n - 1] = new Polyline { new Point3d(bb[i][n - 1]), new Point3d(bb[i][0]), new Point3d(ba[i][0]), new Point3d(ba[i][n - 1]), new Point3d(bb[i][n - 1]) };
                pts[n - 1] = bb[i][n - 1];
                pts[n] = bb[i][0];
                p2[i] = new Polyline(pts);
            }

            return new Tuple<Polyline[][], Polyline[]>(p, p2);
        }

        public static int[][] GetAllNGonEdges_TopoVertices(this Mesh mesh, int[][] tNGonsTopoBoundaries, HashSet<int> allNGonEdges) {
            int[][] v = new int[allNGonEdges.Count][];

            for (int i = 0; i < allNGonEdges.Count; i++) {
                int j = allNGonEdges.ElementAt(i);
                var pair = mesh.TopologyEdges.GetTopologyVertices(j);

                v[i] = new int[] { pair.I, pair.J };
            }

            return v;
        }

        /// <summary>
        /// Ngons offset and merge
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="dist"></param>
        /// <returns></returns>
        public static Polyline[] OffsetNgonAndMerge(this Mesh mesh, double dist) {
            int[][] tv = mesh.GetNGonsTopoBoundaries();
            Point3d[] c = mesh.GetNGonCenters();

            HashSet<int> e = mesh.GetAllNGonEdges(tv);

            int[][] ef = mesh.GetNgonsConnectedToNGonsEdges(e, false);
            int[][] edgeV = mesh.GetAllNGonEdges_TopoVertices(tv, e);

            //Point3f[][] frames = new Point3f[edgeV.Length][];
            Polyline[] frames = new Polyline[edgeV.Length];

            for (int i = 0; i < ef.Length; i++) {
                //e[i] == edge id
                //If there are two connected faces
                //Take those face centers
                //Scale copy edge vertices by center to creat faces
                if (ef[i].Length == 2) {
                    //Center for scaling
                    Point3d cA = c[ef[i][0]];
                    Point3d cB = c[ef[i][1]];

                    //Points to scale
                    Point3f ptA = mesh.TopologyVertices[edgeV[i][0]];
                    Point3f ptB = mesh.TopologyVertices[edgeV[i][1]];
                    Point3f ptC = mesh.TopologyVertices[edgeV[i][0]];
                    Point3f ptD = mesh.TopologyVertices[edgeV[i][1]];

                    ptA.Transform(Transform.Scale(cA, dist));
                    ptB.Transform(Transform.Scale(cA, dist));
                    ptC.Transform(Transform.Scale(cB, dist));
                    ptD.Transform(Transform.Scale(cB, dist));

                    Point3d[] frame = new Point3d[] {
                        mesh.TopologyVertices[edgeV[i][0]],
                        ptA,
                       ptB,
                        mesh.TopologyVertices[edgeV[i][1]],
                        ptD,
                       ptC,
                       mesh.TopologyVertices[edgeV[i][0]]
                    };

                    frames[i] = new Polyline(frame);
                }
            }
            return frames;
        }

        /// <summary>
        /// Quads in mesh
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Dictionary<int, int> QuadFacesDict(this Mesh mesh) {
            Dictionary<int, int> id = new Dictionary<int, int>();
            for (int i = 0; i < mesh.Faces.Count; i++)
                if (mesh.Faces[i].IsQuad)
                    id.Add(i, id.Count);
            return id;
        }

        public static int[] QuadFaces(this Mesh mesh) {
            List<int> id = new List<int>();
            for (int i = 0; i < mesh.Faces.Count; i++)
                if (mesh.Faces[i].IsQuad)
                    id.Add(i);
            return id.ToArray();
        }

        /// <summary>
        /// Nested array of vertex - quad faces (face id that are quads)
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static int[][] QuadFaceMap(this Mesh mesh) {
            Dictionary<int, int> qfaces = mesh.QuadFacesDict();

            int[][] id = new int[mesh.Vertices.Count][];

            for (int i = 0; i < mesh.Vertices.Count; i++) {
                List<int> quads = new List<int>();
                int[] f = mesh.Vertices.GetVertexFaces(i);
                for (int j = 0; j < f.Length; j++)
                    if (qfaces.ContainsKey(f[j]))
                        quads.Add(f[j]);
                id[i] = quads.ToArray();
            }
            return id;
        }

        /// <summary>
        /// Nested array of vertex - quad faces (face id that are quads)
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static int[][] QuadFaceMap(this Mesh mesh, Dictionary<int, int> qfaces) {
            int[][] id = new int[mesh.Vertices.Count][];

            for (int i = 0; i < mesh.Vertices.Count; i++) {
                List<int> quads = new List<int>();
                int[] f = mesh.Vertices.GetVertexFaces(i);
                for (int j = 0; j < f.Length; j++)
                    if (qfaces.ContainsKey(f[j]))
                        quads.Add(f[j]);
                id[i] = quads.ToArray();
            }
            return id;
        }

        /// <summary>
        /// mesh faces connected to vertices
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static int[][] FaceMap(this Mesh mesh) {
            int[][] id = new int[mesh.Vertices.Count][];

            for (int i = 0; i < mesh.Vertices.Count; i++)
                id[i] = mesh.Vertices.GetVertexFaces(i);

            return id;
        }

        /// <summary>
        /// Naked mesh edges
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static bool[] NakedNGonEdges(this Mesh mesh, HashSet<int> e) {
            bool[] n = new bool[e.Count];

            for (int i = 0; i < e.Count; i++)
                n[i] = mesh.TopologyEdges.GetConnectedFaces(e.ElementAt(i)).Length == 1;

            return n;
        }

        /// <summary>
        /// Weld mesh , compute normals, compact
        /// </summary>
        /// <param name="mesh"></param>
        //public static void WeldFull(this Mesh mesh, double weldRadius) {
        //    mesh.WeldUsingRTree(weldRadius);
        //}

        public static void Clean(this Mesh mj) {
            //mesh.Compact();
            //mesh.Vertices.CombineIdentical(true, true);
            //mesh.Vertices.CullUnused();

            //if (mesh.Ngons.Count>0)
            //    mesh.UnifyNormalsNGons();

            //mesh.Weld(3.14159265358979);
            //mesh.FaceNormals.ComputeFaceNormals();
            //mesh.Normals.ComputeNormals();
            ////mesh.UnifyNormals();

            //if (mesh.SolidOrientation() == -1)
            //    mesh.Flip(true, true, true);

            mj.Compact();
            mj.Vertices.CombineIdentical(true, true);
            mj.Vertices.CullUnused();

            if (mj.Ngons.Count > 0)
                mj.UnifyNormalsNGons();
            else
                mj.UnifyNormals();

            mj.Weld(3.14159265358979);
            mj.FaceNormals.ComputeFaceNormals();
            mj.Normals.ComputeNormals();

            if (mj.SolidOrientation() == -1)
                mj.Flip(true, true, true);
        }

        public static void UnifyNormalsNGons(this Mesh mesh) {
            uint[][] boundaries = mesh.GetNGonsBoundaries();
            mesh.UnifyNormals();
            uint[][] boundaries2 = mesh.GetNGonsBoundaries();

            uint a, b;

            for (int i = 0; i < boundaries.Length; i++) {
                a = boundaries[i][1];
                b = boundaries2[i][1];

                if (a != b) {
                    List<int> castBoundaries = boundaries[i].ToList().ConvertAll(x => (int)x);
                    List<int> castFaces = mesh.Ngons[i].FaceIndexList().ToList().ConvertAll(x => (int)x);
                    mesh.Ngons[i] = Rhino.Geometry.MeshNgon.Create(castBoundaries, castFaces);
                }
            }
        }

        public static UInt32 Key(int i, int j) {
            return (UInt32)(i << 16 | j);
        }

        /// <summary>
        /// Removes all vertices that are less than vertex valence 3
        /// </summary>
        /// <param name="M"></param>
        /// <param name="fixedPoints"></param>
        /// <returns></returns>
        public static Mesh SimplifyMesh(this Mesh M, List<Point3d> fixedPoints) {
            double tol = Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance;

            int[][] topo = M.GetNGonsTopoBoundaries();

            HashSet<int> topoAll = M.GetAllNGonsTopoVertices();
            int[] topoAll_Array = topoAll.ToArray();

            HashSet<int> edges = M.GetAllNGonEdges(topo);
            int[][] ve = M.GetConnectedNGonEdgesToNGonTopologyVertices(topoAll, edges);

            List<Polyline> polylines = new List<Polyline>();

            for (int i = 0; i < topo.Length; i++) {
                Polyline polyline = new Polyline();

                for (int j = 0; j < topo[i].Length; j++) {
                    int localID = Array.IndexOf(topoAll_Array, topo[i][j]);
                    int eCount = ve[localID].Length;

                    Point3d v = M.TopologyVertices[topo[i][j]];

                    bool flag = false;
                    foreach (Point3d p in fixedPoints) {
                        if (p.DistanceToSquared(v) < 0.0001)
                            flag = true;
                    }

                    if (eCount > 2 || flag) {
                        polyline.Add(v);
                    }
                }//for i

                polyline.Close();
                if (polyline.IsValid)
                    polylines.Add(polyline);
            }//for j

            Mesh simplifiedMesh = MeshCreate.MeshFromPolylines(polylines, 0.0);

            return simplifiedMesh;
        }

        /// <summary>
        /// Get Ngon Count
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>Oppo
        public static int _countF(this Mesh M) {
            return M.Ngons.Count;
        }

        /// <summary>
        /// Get Ngon Vertex and Edge Count
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F"></param>
        /// <returns></returns>
        public static int _countE(this Mesh M, int F) {
            return M.Ngons[F].BoundaryVertexCount;
        }

        /// <summary>
        /// Get Ngon Vertex and Edge Count
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F"></param>
        /// <returns></returns>
        public static int _countV(this Mesh M, int F) {
            return M.Ngons[F].BoundaryVertexCount;
        }

        /// <summary>
        /// Get NGon Boundaries as topological vertex ID array
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static int[] _v(this Mesh M, int F, bool topologyVertices = true) {
            if (topologyVertices)
                return NGonUtil.VertexTopoVertex(M, M.Ngons[F].BoundaryVertexIndexList());

            uint[] vUint = M.Ngons[F].BoundaryVertexIndexList();
            int[] vInt = new int[vUint.Length];
            for (int i = 0; i < vUint.Length; i++)
                vInt[i] = (int)vUint[i];

            return vInt;
        }

        /// <summary>
        /// Get NGons Boundaries as topological vertex ID array
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static int[][] _v(this Mesh M) {
            int[][] boundaries = new int[M.Ngons.Count][];

            for (int i = 0; i < M.Ngons.Count; i++)
                boundaries[i] = M._v(i);

            return boundaries;
        }

        public static int[] _fv(this Mesh M, int F, bool topo) {
            uint[] uintv = M.Ngons[F].BoundaryVertexIndexList();
            int[] v = new int[uintv.Length];
            for (int i = 0; i < uintv.Length; i++) {
                v[i] = (topo) ? (int)uintv[i] : M.TopologyVertices.TopologyVertexIndex((int)uintv[i]);
            }

            return v;
        }

        /// <summary>
        /// Get Ngon Edge vertices
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F"></param>
        /// <param name="E"></param>
        /// <returns></returns>
        public static int[] _e(this Mesh M, int F, int E, int next = 0) {
            var v = M.Ngons[F].BoundaryVertexIndexList();

            int v0 = (int)v[MathUtil.Wrap(E + 0 + next, v.Length)];
            int v1 = (int)v[MathUtil.Wrap(E + 1 + next, v.Length)];

            return new int[] { v0, v1 };
        }

        public static int _meshE(this Mesh M, int F, int E, int next = 0) {
            var v = M.Ngons[F].BoundaryVertexIndexList();

            int v0 = (int)v[MathUtil.Wrap(E + 0 + next, v.Length)];
            int v1 = (int)v[MathUtil.Wrap(E + 1 + next, v.Length)];

            v0 = M.TopologyVertices.TopologyVertexIndex(v0);
            v1 = M.TopologyVertices.TopologyVertexIndex(v1);

            int meshEdge = M.TopologyEdges.GetEdgeIndex(v0, v1);

            return meshEdge;
        }

        /// <summary>
        /// Get NGons Face Edges, as mesh edge indices
        /// </summary>
        /// <param name="M"></param>
        /// <param name="nGonTV"></param>
        /// <returns></returns>
        public static int[] _fe(this Mesh M, int F) {
            int n = M.Ngons[F].BoundaryVertexCount;
            int[] fe = new int[n];
            int[] v = M._v(F);

            for (int j = 0; j < n - 1; j++)
                fe[j] = M.TopologyEdges.GetEdgeIndex(v[j], v[j + 1]);

            int tempEId = M.TopologyEdges.GetEdgeIndex(v[0], v[n - 1]);
            fe[n - 1] = tempEId;

            return fe;
        }

        /// <summary>
        /// Get NGons Faces Edges, as mesh edge indices
        /// </summary>
        /// <param name=","></param>
        /// <param name="nGonTV"></param>
        /// <returns></returns>
        public static int[][] _FE(this Mesh M, int[][] nGonTV) {
            int[][] nGonE = new int[M.Ngons.Count][];

            for (int i = 0; i < M.Ngons.Count; i++) {
                nGonE[i] = M._fe(i);
            }

            return nGonE;
        }

        public static List<int> _FEFlattenV(this Mesh M) {
            List<int> v = new List<int>();

            int counter = 0;
            for (int i = 0; i < M._countF(); i++)
                for (int j = 0; j < M._countE(i); j++)
                    v.Add(counter++);

            return v;
        }

        /// <summary>
        /// //face, edge - flatten id of faces edge (duplicates)
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static int[][] _FEFlatten(this Mesh M) {
            int[][] FE_ = new int[M._countF()][];//face, edge - flatten id

            int counter = 0;
            for (int i = 0; i < M._countF(); i++) {
                FE_[i] = new int[M._countE(i)];

                for (int j = 0; j < M._countE(i); j++)
                    FE_[i][j] = counter++;
            }

            return FE_;
        }

        /// <summary>
        /// flatten id of faces edge (duplicates) - face, edge
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static List<int[]> _FlattenFE(this Mesh M) {
            List<int[]> _FE = new List<int[]>(); //flatten id - face, edge

            for (int i = 0; i < M._countF(); i++)
                for (int j = 0; j < M._countE(i); j++)
                    _FE.Add(new int[] { i, j });

            return _FE;
        }

        /// <summary>
        /// edge adjacency: current edge - next face edge, prev face edge, opposite face edge
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static List<int[]>[] _eehalf(this Mesh M) {
            var eehalf = new List<int[]>[M._countF()];

            var FE_ = M._FEFlatten();

            for (int i = 0; i < M._countF(); i++) {
                var edges = new List<int[]>();

                for (int j = 0; j < M._countE(i); j++) {
                    int eA = FE_[i][j];
                    int eBNext = FE_[i].Next(j);
                    int eBPrev = FE_[i].Prev(j);

                    int[] oppo = M._OppositeFE(i, j);
                    if (oppo[0] != -1) {//not a boundary
                        int eBOppo = FE_[oppo[0]][oppo[1]];
                        edges.Add(new int[] { eA, eBOppo });
                        //if(i == 0 && j == 0) Print(eA.ToString() + " " + eBNext.ToString() + " " + eBPrev.ToString() + " " + eBOppo.ToString());
                    }

                    //next / prev
                    edges.Add(new int[] { eA, eBNext });
                    edges.Add(new int[] { eA, eBPrev });
                }

                eehalf[i] = edges;
            }

            return eehalf;
        }

        /// <summary>
        /// NGons By MeshEdge
        /// </summary>
        /// <param name="M"></param>
        /// <param name="MeshEdge"></param>
        /// <returns></returns>
        public static int[] _ef(this Mesh M, int MeshEdge) {
            //Get connected mesh faces
            int[] connectedFaces = M.TopologyEdges.GetConnectedFaces(MeshEdge);
            int[] connectedNGons = new int[connectedFaces.Length];

            for (int i = 0; i < connectedFaces.Length; i++)
                connectedNGons[i] = M.Ngons.NgonIndexFromFaceIndex(connectedFaces[i]);

            return connectedNGons;
        }

        /// <summary>
        /// Get edge face adjacency, boundaries would have 1 face
        /// id array corresponds to M.EAll method order
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static int[][] _EF(this Mesh M) {
            Dictionary<int, int> eAll = M._EAll();

            int[][] ef = new int[eAll.Count][];

            int i = 0;
            foreach (KeyValuePair<int, int> meshEdge_FlatListOfID in eAll)
                ef[i++] = M._ef(meshEdge_FlatListOfID.Key);

            return ef;
        }

        /// <summary>
        /// array of tuples - all edges
        /// array - number of connected faces
        /// tuple - face, local edge
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static Tuple<int, int>[][] _EFE(this Mesh M) {
            Dictionary<int, int> eAll = M._EAll();
            Tuple<int, int>[][] efe = new Tuple<int, int>[eAll.Count][];
            int i = 0;

            //we have mesh edge and face edge
            foreach (KeyValuePair<int, int> meshEdge_FlatListOfID in eAll) {
                int meshedge = meshEdge_FlatListOfID.Key;
                int[] faces = M._ef(meshEdge_FlatListOfID.Key);
                efe[i] = new Tuple<int, int>[faces.Length];

                for (int j = 0; j < faces.Length; j++) {
                    int[] fe = M._fe(faces[j]);
                    int e = Array.IndexOf(fe, meshedge);
                    efe[i][j] = new Tuple<int, int>(faces[j], e);
                }

                i++;
            }

            return efe;
        }

        /// <summary>
        /// Get edge face adjacency, boundaries would have 1 face
        /// Dictionary contains mesh edges
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static Dictionary<int, int[]> _EFDict(this Mesh M) {
            Dictionary<int, int> eAll = M._EAll();

            Dictionary<int, int[]> ef = new Dictionary<int, int[]>();

            foreach (KeyValuePair<int, int> meshEdge_FlatListOfID in eAll)
                ef.Add(meshEdge_FlatListOfID.Key, M._ef(meshEdge_FlatListOfID.Key));

            return ef;
        }

        public static int[] _vf(this Mesh M, int MeshVertex) {
            int[] connectedNGons = new int[0];

            int tv = M.TopologyVertices.TopologyVertexIndex(MeshVertex);

            int[] connectedFaces = M.TopologyVertices.ConnectedFaces(tv);

            HashSet<int> connectedNGonsHash = new HashSet<int>();
            foreach (int j in connectedFaces)
                connectedNGonsHash.Add(M.Ngons.NgonIndexFromFaceIndex(j));

            connectedNGons = connectedNGonsHash.ToArray();

            return connectedNGons;
        }

        public static int[] _ve(this Mesh M, int MeshVertex) {
            int tv = M.TopologyVertices.TopologyVertexIndex(MeshVertex);
            M.TopologyVertices.SortEdges(tv);

            int[] connectedEdges = M.TopologyVertices.ConnectedEdges(tv);

            HashSet<int> connectedNGonsEdgesHash = new HashSet<int>();

            foreach (int j in connectedEdges)
                if (!M.TopologyEdges.IsNgonInterior(j))
                    connectedNGonsEdgesHash.Add(j);

            return connectedNGonsEdgesHash.ToArray();
        }

        /// <summary>
        /// Get opposite Ngon, its local edge and meshe edge
        /// </summary>
        /// <param name="M"></param>
        /// <param name="f - ngon id"></param>
        /// <param name="e - local edge 0, 1, 2 - n"></param>
        /// <param name="next"></param>
        /// <returns></returns>
        public static int[] _OppositeFE(this Mesh M, int F, int E, int next = 0) {
            //Get mesh edge from mesh face e

            int eWrapped = MathUtil.Wrap(E, M.Ngons[F].BoundaryVertexCount);

            int me = M._fe(F)[eWrapped];

            //Get connected mesh faces
            int[] connectedFaces = M.TopologyEdges.GetConnectedFaces(me);

            int oppositeFace = -1;

            if (connectedFaces.Length == 2) {
                int f0 = M.Ngons.NgonIndexFromFaceIndex(connectedFaces[0]);
                int f1 = M.Ngons.NgonIndexFromFaceIndex(connectedFaces[1]);
                oppositeFace = (F == f0) ? f1 : f0;
            }

            int oppositeEdge = -1;

            if (oppositeFace != -1) {
                int[] oppositeFE = M._fe(oppositeFace);

                oppositeEdge = Array.IndexOf(oppositeFE, me);
                oppositeEdge = MathUtil.Wrap(oppositeEdge + next, oppositeFE.Length);
            }

            int meOppo = (oppositeFace == -1) ? -1 : M._fe(oppositeFace)[oppositeEdge];

            return new int[] { oppositeFace, oppositeEdge, me, meOppo };//me
        }

        /// <summary>
        /// Find Sharing Mesh Edge
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F0"></param>
        /// <param name="F1"></param>
        /// <returns></returns>
        public static int _CommonEdge(this Mesh M, int F0, int F1) {
            int[] e0 = M._fe(F0);
            int[] e1 = M._fe(F1);

            var common = e0.Intersect(e1);

            if (common.Count() > 0)
                return common.First();
            else
                return -1;
        }

        /// <summary>
        /// Find Sharing Mesh Edges
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F0"></param>
        /// <param name="F1"></param>
        /// <returns></returns>
        public static int[] _CommonEdges(this Mesh M, int F0, int F1) {
            int[] e0 = M._fe(F0);
            int[] e1 = M._fe(F1);

            var common = e0.Intersect(e1);

            return common.ToArray();
        }

        //////////////////////////////////Geometry//////////////////////////////////

        /// <summary>
        /// Get NGone Edge Line by Face and Edge
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F"></param>
        /// <param name="E"></param>
        /// <returns></returns>
        public static Line _Line(this Mesh M, int F, int E) {
            int eWrapped = MathUtil.Wrap(E, M.Ngons[F].BoundaryVertexCount);
            int[] v = M._e(F, eWrapped);
            return new Line(M.Vertices[v[0]], M.Vertices[v[1]]);
        }

        public static Line _Line(this Mesh M, int[] FE) {
            return _Line(M, FE[0], FE[1]);
        }

        /// <summary>
        /// Get NGon Polyline from Mesh
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Polyline _Polyline(this Mesh M, int F) {
            int[] v = M._v(F);
            Polyline p = new Polyline();

            for (int j = 0; j < v.Length; j++)
                p.Add(M.Vertices[v[j]]);

            p.Close();

            return p;
        }

        /// <summary>
        /// Get NGons as Polylines from Mesh
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static Polyline[] _Polylines(this Mesh M) {
            Polyline[] polylines = new Polyline[M.Ngons.Count];

            for (int i = 0; i < M.Ngons.Count; i++) {
                polylines[i] = M._Polyline(i);
            }

            return polylines;
        }

        /// <summary>
        /// Get Ngon Center
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="ngon"></param>
        /// <returns></returns>
        public static Point3d _FCenter(this Mesh M, int F) {
            uint[] vertices = M.Ngons.GetNgon(F).BoundaryVertexIndexList();

            Polyline outline = new Polyline();
            foreach (int v in vertices) {
                outline.Add(M.Vertices[v]);
            }

            return PolylineUtil.CenterPoint(outline);
        }

        /// <summary>
        /// Get Ngon points
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="F"></param>
        /// <returns></returns>
        public static Point3d[] _Points(this Mesh M, int F) {
            uint[] v = M.Ngons[F].BoundaryVertexIndexList();
            Point3d[] p = new Point3d[v.Length];

            for (int i = 0; i < v.Length; i++)
                p[i] = M.Vertices[(int)v[i]];

            return p;
        }

        /// <summary>
        /// Get Ngon Plane
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Plane _Plane(this Mesh M, int F) {
            return new Plane(M._FCenter(F), M._Normal(F));
        }

        /// <summary>
        /// Get Ngon Planes
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static Plane[] _FPlanes(this Mesh M) {
            Plane[] p = new Plane[M.Ngons.Count];
            for (int i = 0; i < M.Ngons.Count; i++) {
                //Plane.FitPlaneToPoints(pts[i], out p[i]);
                p[i] = M._Plane(i);
                //p[i].Bake(100);
                //Rhino.RhinoApp.WriteLine(p[i].IsValid.ToString());
            }
            return p;
        }

        /// <summary>
        /// Get Ngon normal
        /// </summary>
        /// <param name="M"></param>
        /// <param name="F"></param>
        /// <returns></returns>
        public static Vector3d _Normal(this Mesh M, int F) {
            Vector3d vector3d = new Vector3d();

            uint[] faces = M.Ngons[F].FaceIndexList();

            for (int i = 0; i < faces.Length; i++)
                vector3d += M.FaceNormals[(int)faces[i]];

            vector3d.Unitize();

            return vector3d;
        }

        ///// <summary>
        ///// Get Mesh Topology vertices in all NGons (HashSet)
        ///// </summary>
        ///// <param name="mesh"></param>
        ///// <returns></returns>
        //public static int[] _VTopoAll(this Mesh M) {
        //    HashSet<int> allNGonTV = new HashSet<int>();

        //    for (int i = 0; i < M.Ngons.Count; i++) {
        //        uint[] meshV = M.Ngons[i].BoundaryVertexIndexList();
        //        int[] meshTv = _VtoVTopo(M, meshV);

        //        foreach (int j in meshTv)
        //            allNGonTV.Add(j);
        //    }
        //    return allNGonTV.ToArray();
        //}

        /// <summary>
        /// Get Mesh vertices in all NGons (HashSet)
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Dictionary<int, int> _VAll(this Mesh M, bool topo = false) {
            HashSet<int> allNGonV = new HashSet<int>();

            for (int i = 0; i < M.Ngons.Count; i++) {
                uint[] meshV = M.Ngons[i].BoundaryVertexIndexList();
                foreach (uint j in meshV) {
                    if (!topo) {
                        allNGonV.Add((int)j);
                    } else {
                        allNGonV.Add(M.TopologyVertices.TopologyVertexIndex((int)j));
                    }
                }
            }

            Dictionary<int, int> meshVertexNGonVertex = new Dictionary<int, int>();

            int counter = 0;
            foreach (int i in allNGonV) {
                meshVertexNGonVertex.Add(i, counter++);
            }

            return meshVertexNGonVertex;
        }

        /// <summary>
        /// Dicionary of all mesh edges, first int - meshes Edge. second int - flattend id list
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static Dictionary<int, int> _EAll(this Mesh M) {
            Dictionary<int, int> allE = new Dictionary<int, int>();
            HashSet<int> hash = new HashSet<int>();
            int counter = 0;

            try {
                for (int i = 0; i < M.Ngons.Count; i++) {
                    int n = M.Ngons[i].BoundaryVertexCount;

                    int[] fv = M._fv(i, true);

                    for (int j = 0; j < n - 1; j++) {
                        int et = M.TopologyEdges.GetEdgeIndex(fv[j], fv[j + 1]);

                        if (hash.Add(et))
                            allE.Add(et, counter++);

                        //if (!allE.ContainsKey(et))
                        //    allE.Add(et, counter++);
                    }

                    int e = M.TopologyEdges.GetEdgeIndex(fv[0], fv[n - 1]);

                    if (hash.Add(e))
                        allE.Add(e, counter++);

                    //if (!allE.ContainsKey(e)) {
                    //    allE.Add(e, counter++);
                    //}
                }
            } catch (Exception e) {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
            return allE;
        }

        /// <summary>
        /// Get outlines of faces edges
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static List<Line> _EFLinesAll(this Mesh M) {
            var faceLines = new List<Line>();

            for (int i = 0; i < M._countF(); i++) {
                int[] fv = M._fv(i, false);

                for (int j = 0; j < fv.Length; j++) {
                    faceLines.Add(new Line(M.Vertices[fv[j]], M.Vertices[fv.Next(j)]));
                }
            }

            return faceLines;
        }

        public static Line[][] _EFLines(this Mesh M) {
            var faceLines = new Line[M._countF()][];

            for (int i = 0; i < M._countF(); i++) {
                int[] fv = M._fv(i, false);
                faceLines[i] = new Line[fv.Length];

                for (int j = 0; j < fv.Length; j++) {
                    faceLines[i][j] = new Line(M.Vertices[fv[j]], M.Vertices[fv.Next(j)]);
                }
            }

            return faceLines;
        }

        /// <summary>
        /// Get outlines of faces edges, where the end vertex is the center of ngon
        /// </summary>
        /// <param name="M"></param>
        /// <returns></returns>
        public static List<Polyline> _EFPolylinesAll(this Mesh M) {
            var facePolylines = new List<Polyline>();

            for (int i = 0; i < M._countF(); i++) {
                int[] fv = M._fv(i, false);
                Point3d c = M._FCenter(i);
                for (int j = 0; j < fv.Length; j++) {
                    facePolylines.Add(new Polyline() { M.Vertices[fv[j]], M.Vertices[fv.Next(j)], c, M.Vertices[fv[j]] });
                }
            }

            return facePolylines;
        }

        /// <summary>
        /// Convert mesh vertex ID array to mesh topological vertex ID array
        /// </summary>
        /// <param name="mesh"></param>
        /// <param name="vertexIDs"></param>
        /// <returns></returns>
        public static int[] _VtoVTopo(this Mesh mesh, uint[] vertexIDs) {
            int[] topoVertices = new int[vertexIDs.Length];

            for (int i = 0; i < vertexIDs.Length; i++)
                topoVertices[i] = mesh.TopologyVertices.TopologyVertexIndex((int)vertexIDs[i]);

            return topoVertices;
        }

        /// <summary>
        /// Get NGons as Polylines from Mesh
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Polyline[] _FPolylines(this Mesh M) {
            Polyline[] polylines = new Polyline[M.Ngons.Count];

            for (int i = 0; i < M.Ngons.Count; i++) {
                uint[] boundaries = M.Ngons[i].BoundaryVertexIndexList();

                Polyline p = new Polyline();

                for (int j = 0; j < boundaries.Length; j++)
                    p.Add(M.Vertices[(int)boundaries[j]]);

                p.Add(M.Vertices[(int)boundaries[0]]);
                polylines[i] = p;
            }

            return polylines;
        }

        /// <summary>
        /// Get NGons edges as Lines from Mesh Face
        /// </summary>
        /// <param name="mesh"></param>
        /// <returns></returns>
        public static Line[] _flines(this Mesh M, int i) {
            uint[] boundaries = M.Ngons[i].BoundaryVertexIndexList();
            Line[] lines = new Line[boundaries.Length];

            for (int j = 0; j < boundaries.Length; j++) {
                Point3d p0 = M.Vertices[(int)boundaries[j]];
                Point3d p1 = M.Vertices[(int)boundaries[(j + 1) % boundaries.Length]];
                lines[j] = new Line(p0, p1);
            }

            return lines;
        }
    }
}