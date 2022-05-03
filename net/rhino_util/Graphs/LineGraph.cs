using Grasshopper;
using Grasshopper.Kernel.Data;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util.graphs {

    public static class LineGraph {

        public static Mesh CurveMesh(List<Line> lines, List<double> r_, double DivisionDist, int DivisionU, int DivisionV,
            ref DataTree<Point3d> anchors0, ref DataTree<Point3d> anchors1, ref DataTree<Polyline> outlines, ref Circle[] graphNodes, List<Circle> circleRadius = null) {
            Tuple<Point3d[], List<string>, List<int>, List<int>, List<int>, DataTree<int>> data = GetGraphData(lines);
            List<Point3d> TP = data.Item1.ToList();

            List<int> U = data.Item3;
            List<int> V = data.Item4;
            DataTree<int> VAdj = data.Item6;

            DataTree<Vector3d> sortedVecs = new DataTree<Vector3d>();
            DataTree<Vector3d> sortedBisectors = new DataTree<Vector3d>();
            DataTree<Polyline> output = new DataTree<Polyline>();
            DataTree<Mesh> outputMesh = new DataTree<Mesh>();
            DataTree<Point3d> outputPt = new DataTree<Point3d>();
            DataTree<Point3d> outputPtArc = new DataTree<Point3d>();
            Mesh joinedMesh = new Mesh();

            DataTree<int> sortedIDs = new DataTree<int>();
            var polys = new List<Polyline>();

            double[] radius = Enumerable.Repeat(1.0, TP.Count).ToArray();

            List<double> r = new List<double>();
            if (r_.Count == 0) {
                r.Add(1);
            } else {
                r = r_;
            }

            if (r.Count < TP.Count) {
                radius = new double[radius.Length];
                for (int i = 0; i < TP.Count; i++) {
                    radius[i] = r[i % r.Count];
                }
            } else {
                radius = r.ToArray();
                // Rhino.RhinoApp.WriteLine("correct");
            }

            PointCloud cloud = new PointCloud(TP);

            for (int i = 0; i < circleRadius.Count; i++) {
                int cp = cloud.ClosestPoint(circleRadius[i].Center);
                if (cloud[cp].Location.DistanceToSquared(circleRadius[i].Center) < 0.01) {
                    //r[cp] = circleRadius[i].Radius;
                    if (circleRadius[i].Plane.ZAxis != Vector3d.ZAxis)
                        radius[cp] = -circleRadius[i].Radius;
                    else
                        radius[cp] = circleRadius[i].Radius;
                }
            }

            for (int i = 0; i < TP.Count; i++) {
                //create node vectors and sort them
                //also sort ids
                Vector3d[] vec = new Vector3d[VAdj.Branch(i).Count];
                int[] nei = VAdj.Branch(i).ToArray();

                for (int j = 0; j < VAdj.Branch(i).Count; j++) {
                    Point3d p0 = TP[i];
                    Point3d p1 = TP[VAdj.Branch(i)[j]];
                    vec[j] = p1 - p0;
                    vec[j].Unitize();

                    vec[j] *= Math.Abs(radius[j]);
                }

                Array.Sort(vec, nei, new ClockwiseVector3dComparer());
                sortedVecs.AddRange(vec, new GH_Path(i));
                sortedIDs.AddRange(nei, new GH_Path(i));

                //Bisector
                Vector3d[] bisectors = new Vector3d[VAdj.Branch(i).Count];
                if (VAdj.Branch(i).Count > 1) {
                    for (int j = 0; j < VAdj.Branch(i).Count; j++) {
                        Vector3d vec0 = vec[j];
                        Vector3d vec1 = vec[(j + 1) % VAdj.Branch(i).Count];
                        Vector3d bisector = new Vector3d(vec1);

                        Vector3d vvec0 = new Vector3d(vec0);
                        Vector3d vvec1 = new Vector3d(vec1);
                        vvec0.Unitize();
                        vvec1.Unitize();
                        vvec0.Rotate(-Math.PI * 0.5, Vector3d.ZAxis);
                        double angle = Vector3d.VectorAngle(vec0, vec1, new Plane(TP[i], vec0, vvec0)) * 0.5;
                        //           Vector3d cross = Rhino.Geometry.Vector3d.CrossProduct(vvec0, vvec1);
                        //         angle =  Math.Atan2(cross * Vector3d.ZAxis, vvec0*vvec1);

                        bisector.Rotate(angle, Vector3d.ZAxis);
                        bisector.Unitize();
                        bisector *= Math.Abs(radius[i]);
                        bisectors[j] = bisector;
                    }

                    sortedBisectors.AddRange(bisectors, new GH_Path(i));
                } else {
                    Vector3d vec0 = new Vector3d(vec[0]);
                    Vector3d vec1 = new Vector3d(vec[0]);
                    vec0.Rotate(-Math.PI * 0.5, Vector3d.ZAxis);
                    vec1.Rotate(Math.PI * 0.5, Vector3d.ZAxis);
                    vec0.Unitize();
                    vec1.Unitize();
                    vec0 *= Math.Abs(radius[i]);
                    vec1 *= Math.Abs(radius[i]);
                    Vector3d[] bisectors_ = new Vector3d[] { vec0, vec1 };
                    //Array.Sort(bisectors_, new ClockwiseVector3dComparer());

                    sortedBisectors.AddRange(bisectors_, new GH_Path(i));
                }
            }

            DataTree<Polyline> polylines = new DataTree<Polyline>();
            // for(int i = 0; i < 1;i++){//TP.Count
            for (int i = 0; i < TP.Count; i++) {
                //for(int j = 1; j < 2;j++){//VAdj.Branch(i).Count
                for (int j = 0; j < VAdj.Branch(i).Count; j++) {//VAdj.Branch(i).Count
                    int currentNei = sortedIDs.Branch(i)[j];
                    int[] neinei = sortedIDs.Branch(currentNei).ToArray();
                    int neiID = Array.IndexOf(neinei, i);

                    neiID = MathUtil.Wrap(neiID - 1, neinei.Length);
                    Vector3d neiVec = new Vector3d(sortedBisectors.Branch(currentNei)[neiID]);

                    if (neinei.Length == 1) {
                        neiVec = new Vector3d(sortedBisectors.Branch(currentNei)[1]);
                    }

                    bool flag1 = sortedIDs.Branch(currentNei).Count == 1;
                    bool flag0 = sortedIDs.Branch(i).Count == 1;

                    Line line = Line.Unset;
                    Polyline anchorsPoly = new Polyline();
                    Arc anchorsArc = Arc.Unset;

                    Vector3d arcVec0 = Vector3d.Zero;
                    Arc arc0 = Arc.Unset;

                    if (flag0) {
                        //Create arc for naked part
                        arcVec0 = TP[i] - TP[currentNei];
                        arcVec0.Unitize();
                        arcVec0 *= radius[i];//Negative gaussian flip

                        line = new Line(TP[i] + arcVec0, TP[i] + sortedBisectors.Branch(i)[j]);
                        anchorsPoly = new Polyline(new Point3d[] { TP[i] + sortedBisectors.Branch(i)[j], TP[i] + arcVec0, TP[i] - sortedBisectors.Branch(i)[j] });
                        anchorsArc = new Arc(TP[i] + sortedBisectors.Branch(i)[j], TP[i] - arcVec0, TP[i] - sortedBisectors.Branch(i)[j]);
                        // Rhino.RhinoDoc.ActiveDoc.Objects.AddArc(anchorsArc);

                        Vector3d arcVec0mid = (sortedBisectors.Branch(i)[j] + arcVec0);
                        arcVec0mid.Unitize();
                        arcVec0mid *= radius[i];

                        arc0 = new Arc(
                          TP[i] + arcVec0,
                          TP[i] + arcVec0mid,
                          TP[i] + sortedBisectors.Branch(i)[j]
                          );

                        // Rhino.RhinoDoc.ActiveDoc.Objects.AddArc(arc0);
                    }

                    Vector3d arcVec1 = Vector3d.Zero;
                    Arc arc1 = Arc.Unset;

                    if (flag1) {
                        //Create arc for naked part
                        arcVec1 = TP[i] - TP[currentNei];
                        arcVec1.Unitize();
                        arcVec1 *= -radius[currentNei];//Negative gaussian flip

                        line = new Line(TP[currentNei] + arcVec1, TP[currentNei] + neiVec);

                        Vector3d arcVec1mid = (neiVec + arcVec1);
                        arcVec1mid.Unitize();
                        arcVec1mid *= -radius[currentNei];

                        arc1 = new Arc(
                          TP[currentNei] + arcVec1,
                          TP[currentNei] + arcVec1mid,
                          TP[currentNei] + neiVec
                          );
                        // Rhino.RhinoDoc.ActiveDoc.Objects.AddArc(arc1);
                    }

                    Polyline polyline = new Polyline(new Point3d[]{
          TP[i] + arcVec0,
          TP[i] + sortedBisectors.Branch(i)[j],
          TP[currentNei] + neiVec,
          TP[currentNei] + arcVec1,
          });
                    polyline.Close();
                    polys.Add(polyline);
                    // if(neinei.Length == 1){
                    output.Add(polyline, new GH_Path(i));

                    //Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(polyline.SegmentAt(1));
                    int dirV = (int)Math.Max(1, Math.Ceiling(polyline.SegmentAt(3).Length / DivisionDist));
                    //Rhino.RhinoApp.WriteLine(dirV.ToString());
                    if (DivisionDist == 0)
                        dirV = DivisionV;

                    //Create surface
                    /*
                    Line line0 = new Line(TP[i] + arcVec0, TP[i] + sortedBisectors.Branch(i)[j]);
                    Line line1 = new Line(TP[currentNei] + arcVec1, TP[currentNei] + neiVec);
                    Brep loft = Brep.CreateFromLoft(new Curve[]{line0.ToNurbsCurve(),line1.ToNurbsCurve()}, Point3d.Unset, Point3d.Unset, LoftType.Tight, false)[0];

                    if(flag0){
                    loft = Brep.CreateFromLoft(new Curve[]{arc0.ToNurbsCurve(),line1.ToNurbsCurve()}, Point3d.Unset, Point3d.Unset, LoftType.Tight, false)[0];
                    }else if(flag1){
                    loft = Brep.CreateFromLoft(new Curve[]{line0.ToNurbsCurve(),arc1.ToNurbsCurve()}, Point3d.Unset, Point3d.Unset, LoftType.Tight, false)[0];
                    //  Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(loft);
                    }
                    */

                    Brep brep = Brep.CreateFromCornerPoints(polyline[0], polyline[1], polyline[2], polyline[3], Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                    if (brep != null) {
                        int dirU = Math.Max(1, DivisionU);
                        //          if(line != Line.Unset){
                        //            Point3d[] nakedPt;
                        //            line.ToNurbsCurve().DivideByCount(dirU, true, out nakedPt);
                        //            outputPt.AddRange(nakedPt, new GH_Path(i));
                        //          }
                        if (anchorsArc != Arc.Unset) {
                            Point3d[] nakedPt;
                            anchorsPoly.SegmentAt(0).ToNurbsCurve().DivideByCount(dirU, true, out nakedPt);
                            outputPt.AddRange(nakedPt, new GH_Path(i));
                            anchorsPoly.SegmentAt(1).ToNurbsCurve().DivideByCount(dirU, true, out nakedPt);
                            for (int k = 1; k < nakedPt.Length; k++) {
                                outputPt.Add(nakedPt[k], new GH_Path(i));
                            }

                            anchorsArc.ToNurbsCurve().DivideByCount(dirU * 2, true, out nakedPt);
                            outputPtArc.AddRange(nakedPt, new GH_Path(i));
                        }

                        Surface surf = brep.Surfaces[0];

                        Mesh mesh = MeshCreate.QuadMesh(surf, dirU, dirV, true, false);

                        outputMesh.Add(mesh, new GH_Path(i));
                        joinedMesh.Append(mesh);
                    }
                }
            }

            joinedMesh.WeldUsingRTree(0.01);
            anchors0 = outputPt;
            anchors1 = outputPtArc;
            outlines = output;
            Circle[] circles = new Circle[data.Item1.Length];
            for (int i = 0; i < circles.Length; i++)
                circles[i] = new Circle(data.Item1[i], Math.Abs(radius[i]));
            graphNodes = circles;
            return joinedMesh;

            //A = sortedVecs;
            //B = sortedBisectors;

            // C = joinedMesh;
            //D = output;
            //E = outputPt;//anchors
            //F = outputPtArc;//anchors
        }

        public static double Angle_2D(Vector3d A, Vector3d B, Plane P) //, out string str_error
        {
            string str_error = null;
            double num;
            Point3d origin = P.Origin + A;
            Point3d point3d = P.Origin + B;
            origin = P.ClosestPoint(origin);
            point3d = P.ClosestPoint(point3d);
            Vector3d vector3d = origin - P.Origin;
            Vector3d origin1 = point3d - P.Origin;
            if (!vector3d.Unitize()) {
                str_error = "First vector is zero-length after projection onto the Plane";
                num = double.NaN;
            } else if (origin1.Unitize()) {
                double num1 = Math.Acos(Math.Max(Math.Min(vector3d * origin1, 1), -1));
                if (Math.Abs(num1) < 1E-32) {
                    num = 0;
                } else if (Math.Abs(num1 - 3.14159265358979) >= 1E-32) {
                    Vector3d vector3d1 = Vector3d.CrossProduct(vector3d, origin1);
                    num = (P.ZAxis.IsParallelTo(vector3d1) != 1 ? 6.28318530717959 - num1 : num1);
                } else {
                    num = 3.14159265358979;
                }
            } else {
                str_error = "Second vector is zero-length after projection onto the Plane";
                num = double.NaN;
            }
            return num;
        }

        public class ClockwiseVector3dComparer : IComparer<Vector3d> {

            public int Compare(Vector3d v1, Vector3d v2) {
                if (v1.X >= 0) {
                    if (v2.X < 0) {
                        return -1;
                    }
                    return -Comparer<double>.Default.Compare(v1.Y, v2.Y);
                } else {
                    if (v2.X >= 0) {
                        return 1;
                    }
                    return Comparer<double>.Default.Compare(v1.Y, v2.Y);
                }
            }
        }

        public static Tuple<Point3d[], List<string>, List<int>, List<int>, List<int>, DataTree<int>> GetGraphData(List<Line> lines) {
            rhino_util.graphs.UndirectedGraph g = LinesToUndirectedGrap(lines);

            DataTree<int> dataTreeA = new DataTree<int>();
            List<String> vertices = g.GetVertices();
            for (int i = 0; i < vertices.Count; i++)
                dataTreeA.AddRange(g.GetNeighbours(vertices[i]), new Grasshopper.Kernel.Data.GH_Path(i));

            DataTree<int> dataTreeB = new DataTree<int>();
            List<rhino_util.graphs.Edge> edges = g.GetEdges();

            List<int> u = new List<int>();
            List<int> v = new List<int>();
            List<int> w = new List<int>();
            for (int i = 0; i < edges.Count; i++) {
                u.Add(edges[i].u);
                v.Add(edges[i].v);
                w.Add(1);
                dataTreeB.Add(edges[i].u, new Grasshopper.Kernel.Data.GH_Path(i));
                dataTreeB.Add(edges[i].v, new Grasshopper.Kernel.Data.GH_Path(i));
            }

            PointCloud pointCloud = ((PointCloud)g.GetAttribute());

            return new Tuple<Point3d[], List<string>, List<int>, List<int>, List<int>, DataTree<int>>(pointCloud.GetPoints(), g.GetVertices(), u, v, w, dataTreeA);
        }

        private static rhino_util.graphs.UndirectedGraph LinesToUndirectedGrap(List<Line> lines) {
            List<Point3d> pts = new List<Point3d>();

            foreach (Line l in lines) {
                pts.Add(l.From);
                pts.Add(l.To);
            }

            //Sorting
            var edges = new List<int>();

            var allPoints = new List<Point3d>(pts); //naked points

            int i = 0;

            while (allPoints.Count != 0) {
                Point3d pt = allPoints[0];
                allPoints.RemoveAt(0);

                for (int d = 0; d < pts.Count; d++) {
                    if (pt.Equals(pts[d])) {
                        edges.Add(d);
                        break;
                    }
                }

                i++;
            }

            var uniqueVertices = new HashSet<int>(edges).ToList();

            //Creating typological points
            var topologyPoints = new PointCloud();

            foreach (int k in uniqueVertices)
                topologyPoints.Add(pts[k]);

            //var vertices = Enumerable.Range(0, uniqueVertices.Count);

            for (int k = 0; k < uniqueVertices.Count; k++)
                if (uniqueVertices.ElementAt(k) != k)
                    for (int l = 0; l < edges.Count; l++)
                        if (edges[l] == uniqueVertices[k])
                            edges[l] = k;

            //Create graph
            rhino_util.graphs.UndirectedGraph g = new rhino_util.graphs.UndirectedGraph(uniqueVertices.Count);

            for (int k = 0; k < uniqueVertices.Count; k++)
                g.InsertVertex(k.ToString());

            for (int k = 0; k < edges.Count; k += 2)
                g.InsertEdge(edges[k].ToString(), edges[k + 1].ToString());

            g.SetAttribute((object)topologyPoints);

            return g;
        }
    }
}