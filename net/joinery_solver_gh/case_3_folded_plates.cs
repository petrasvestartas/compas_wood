using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino.Geometry;
using rhino_util;

namespace joinery_solver_gh
{
    public class case_3_folded_plates
    {
        //Diamond Subdivision
        public Mesh m = null;

        public List<bool> flags = new List<bool>();
        public bool[][] edge_joints = new bool[0][];
        public List<int[]> adjacency = new List<int[]>();
        private Surface s = null;
        private int u_divisions = 1;
        private int v_divisions = 1;
        private List<Plane> base_planes = new List<Plane>();
        public List<Line> insertion_lines = new List<Line>();

        //Properties for plates
        private bool has_ngons = false;

        private double face_thickness = 1;
        private double chamfer = 0.01;
        private double extend = 1;
        private List<double> face_positions = new List<double>();

        public int f;
        public int e;
        public int[][] f_v;
        public int[][] f_e;
        public int[][] e_f;
        public Dictionary<int, int> e_ngon_e;
        public Plane[] f_planes = new Plane[0];
        public Plane[][] fe_planes = new Plane[0][];
        public Plane[][] bi_planes = new Plane[0][];
        public Plane[] e_planes = new Plane[0];
        public Line[] e_lines = new Line[0];
        public Vector3d[] insertion_vectors = new Vector3d[0];

        //Output
        public string[][] f_polylines_index = new string[0][];

        public Plane[][] f_polylines_planes = new Plane[0][];
        public Polyline[][] f_polylines = new Polyline[0][];

        public case_3_folded_plates(Surface s, int u_divisions, int v_divisions, List<Plane> base_planes, double thickness, double chamfer, List<double> face_positions)
        {//List<Line> insertion_vectors,
            this.s = s;
            this.u_divisions = u_divisions;
            this.v_divisions = v_divisions;
            this.base_planes = base_planes;

            //Run
            m = diamond_subdivision();
            this.has_ngons = m.Ngons.Count > 0;
            this.face_thickness = thickness;
            this.chamfer = chamfer;
            this.extend = extend;
            this.face_positions = face_positions;

            //Properties for plates
            get_face_vertices();
            get_faces_planes();
            get_face_edge_planes();
            get_bisector_planes();
            get_edges();
            get_edge_faces();
            //get_edge_vectors(insertion_vectors);
            get_face_polylines();
            get_insertion_vectors();
        }

        public Mesh diamond_subdivision()
        {
            Mesh m = new Mesh();
            flags = new List<bool>();

            if (u_divisions < 1)
                u_divisions = 1;

            if (v_divisions < 1)
                v_divisions = 1;

            int v = 0;
            List<MeshFace> face = new List<MeshFace>();
            List<Point3d> pts = new List<Point3d>();
            Interval interval = s.Domain(0);
            Interval interval1 = s.Domain(1);
            double step_u = (interval.T1 - interval.T0) / (double)u_divisions;
            double step_v = (interval1.T1 - interval1.T0) / (double)v_divisions;

            double sum_step_u = 0;

            for (int i = 0; i < u_divisions; i++)
            {
                int a = i % 2 == 0 ? 1 : 0;
                int b = i % 2 == 0 ? 2 : 3;

                double sum_step_v = 0;
                for (int j = 0; j < v_divisions; j++)
                {
                    Point3d p0 = s.PointAt(sum_step_u, sum_step_v);
                    Point3d p1 = s.PointAt(sum_step_u + step_u, sum_step_v);

                    s.PointAt(sum_step_u, sum_step_v + step_v * 0.5);
                    s.NormalAt(sum_step_u, sum_step_v + step_v * 0.5);
                    s.PointAt(sum_step_u + step_u, sum_step_v + step_v * 0.5);
                    s.NormalAt(sum_step_u + step_u, sum_step_v + step_v * 0.5);

                    Point3d p2 = s.PointAt(sum_step_u, sum_step_v + step_v);
                    Point3d p3 = s.PointAt(sum_step_u + step_u, sum_step_v + step_v);
                    Point3d p4 = s.PointAt(sum_step_u + step_u * 0.5, sum_step_v + step_v * 1.5);
                    Point3d p5 = s.PointAt(sum_step_u + step_u * 0.5, sum_step_v + step_v * 0.5);
                    Point3d p6 = s.PointAt(sum_step_u + step_u * 0.5, sum_step_v - step_v * 0.5);
                    Point3d p7 = (p5 + p6) / 2;
                    Point3d p8 = (p4 + p5) / 2;

                    if (j == 0)
                    {
                        ///////////////////////////////////////
                        //start triangles
                        ///////////////////////////////////////
                        Point3d p9 = s.PointAt(sum_step_u + step_u * 0.5, interval1.T0);
                        Line line = new Line(p5, p6);
                        Point3d cp = line.ClosestPoint(p9, false);

                        if (base_planes.Count > 0)
                        {
                            if (Math.Abs(base_planes.First().ZAxis.X) + Math.Abs(base_planes.First().ZAxis.Y) + Math.Abs(base_planes.First().ZAxis.Z) > 0.01)
                            {
                                double t;
                                Rhino.Geometry.Intersect.Intersection.LinePlane(line, base_planes.First(), out t);
                                cp = line.PointAt(t);
                            }
                        }
                        // cp = p7;

                        pts.AddRange(new Point3d[] { p5, cp, p1 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;

                        pts.AddRange(new Point3d[] { cp, p5, p0 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;
                    }

                    pts.AddRange(new Point3d[] { p1, p3, p5 });
                    this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                    face.Add(new MeshFace(v, v + 1, v + 2));
                    v += 3;
                    pts.AddRange(new Point3d[] { p2, p0, p5 });
                    this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                    face.Add(new MeshFace(v, v + 1, v + 2));
                    v += 3;

                    if (j != v_divisions - 1)
                    {
                        pts.AddRange(new Point3d[] { p4, p5, p3 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;
                        pts.AddRange(new Point3d[] { p5, p4, p2 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;
                    }
                    else
                    {
                        ///////////////////////////////////////
                        //end triangles
                        ///////////////////////////////////////
                        Point3d p9 = s.PointAt(sum_step_u + step_u * 0.5, interval1.T1);
                        Line line = new Line(p4, p5);
                        Point3d cp = line.ClosestPoint(p9, false);

                        if (base_planes.Count > 0)
                        {
                            if (Math.Abs(base_planes.Last().ZAxis.X) + Math.Abs(base_planes.Last().ZAxis.Y) + Math.Abs(base_planes.Last().ZAxis.Z) > 0.01)
                            {
                                double t;
                                Rhino.Geometry.Intersect.Intersection.LinePlane(line, base_planes.Last(), out t);
                                cp = line.PointAt(t);
                            }
                        }
                        //cp = p8;

                        pts.AddRange(new Point3d[] { cp, p5, p3 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;

                        pts.AddRange(new Point3d[] { p5, cp, p2 });
                        this.flags.Add(face.Count % 4 == a || face.Count % 4 == b);
                        face.Add(new MeshFace(v, v + 1, v + 2));
                        v += 3;
                    }
                    sum_step_v += step_v;
                }
                sum_step_u += step_u;
            }

            m.Vertices.AddVertices(pts);

            m.Faces.AddFaces(face);
            m.Weld(3.14159);
            m.RebuildNormals();

            edge_joints = new bool[face.Count][];
            this.adjacency = new List<int[]>();
            //RhinoApp.WriteLine(face.Count.ToString());
            // RhinoApp.WriteLine(m.Faces.Count.ToString()) ;
            for (int i = 0; i < this.flags.Count; i++)
            {
                if (this.flags[i])
                {
                    int[] nei = m.Faces.AdjacentFaces(i);

                    foreach (int j in nei)
                        this.adjacency.Add(new int[2] { i, j });
                }
                edge_joints[i] = Enumerable.Repeat(this.flags[i], 9).ToArray();
            }

            return m;
        }

        //Compute mesh properties

        public void get_face_vertices()
        {
            if (has_ngons)
            {
                this.f = this.m.Ngons.Count;
                f_v = m.GetNGonsTopoBoundaries();
            }
            else
            {
                this.f = this.m.Faces.Count;
                f_v = new int[this.m.Faces.Count][];
                for (int i = 0; i < this.m.Faces.Count; i++)

                    f_v[i] = this.m.Faces[i].IsQuad ? new int[] { this.m.Faces[i][0], this.m.Faces[i][1], this.m.Faces[i][2], this.m.Faces[i][3] } : new int[] { this.m.Faces[i][0], this.m.Faces[i][1], this.m.Faces[i][2] };
            }
        }

        public void get_faces_planes()
        {
            if (has_ngons)
            {
                f_planes = m._FPlanes();
            }
            else
            {
                f_planes = new Plane[m.Faces.Count];
                for (int i = 0; i < m.Faces.Count; i++)
                    f_planes[i] = new Plane(m.MeshFaceCenter(i), m.FaceNormals[i]);
            }
        }

        public void set_edge_normals()
        {
            var topologyEdges = m.TopologyEdges;
            var faceNormals = m.FaceNormals;

            e_planes = new Plane[topologyEdges.Count];

            for (int i = 0; i < (int)topologyEdges.Count; i++)
            {
                Line line = topologyEdges.EdgeLine(i);
                int[] connectedFaces = topologyEdges.GetConnectedFaces(i);

                Vector3d xaxis = (line.To - line.From).UnitVector();
                //Vector3d item1 = new Vector3d();
                Vector3d zaxis = new Vector3d();

                if ((int)connectedFaces.Length != 2)//naked
                    zaxis = Vector3d.CrossProduct(faceNormals[connectedFaces[0]], xaxis);
                else
                {//inner
                    Vector3d zaxis_avg = ((faceNormals[connectedFaces[0]] + faceNormals[connectedFaces[1]]) / 2);
                    zaxis_avg.Unitize();
                    zaxis = Vector3d.CrossProduct(xaxis, zaxis_avg);
                }
                this.e_planes[i] = new Plane(line.PointAt(0.5), zaxis);
            }
        }

        public void get_face_edge_planes()
        {
            set_edge_normals();
            fe_planes = new Plane[f][];

            for (int i = 0; i < f; i++)
            {
                int n = f_v[i].Length;
                fe_planes[i] = new Plane[n];

                for (int j = 0; j < f_v[i].Length; j++)
                {
                    int v1 = f_v[i][(j + 0) % n];
                    int v2 = f_v[i][(j + 1) % n];

                    int e = this.m.TopologyEdges.GetEdgeIndex(v1, v2);
                    int[] cf = this.m.TopologyEdges.GetConnectedFaces(e);

                    Vector3d sum = Vector3d.Zero;
                    for (int k = 0; k < cf.Length; k++)
                        sum += this.m.FaceNormals[cf[k]];
                    sum /= cf.Length;

                    //Plane edgePlane = new Plane((Point3d) (this.m.Vertices[v1] + this.m.Vertices[v2]) * 0.5, this.m.Vertices[v1] - this.m.Vertices[v2], sum);
                    Plane edgePlane = Plane.Unset;

                    //Error
                    if (cf.Length == 2)
                    {
                        //RhinoApp.WriteLine(this.m.MeshFaceCenter(cf[0]).ToString());

                        edgePlane = PlaneUtil.DihedralPlane(
                          new Plane(this.m.MeshFaceCenter(cf[0]), this.m.FaceNormals[cf[0]]),
                          new Plane(this.m.MeshFaceCenter(cf[1]), this.m.FaceNormals[cf[1]])

                          );

                        edgePlane = this.e_planes[e];
                    }
                    else
                    {
                        edgePlane = new Plane((Point3d)(this.m.Vertices[v1] + this.m.Vertices[v2]) * 0.5, this.m.Vertices[v1] - this.m.Vertices[v2], sum);
                    }

                    fe_planes[i][j] = edgePlane;

                    //Error Edge plane is not average but dihedral

                    //Adjust to surface
                    //          if(s != null){
                    //
                    //            double u, v;
                    //            s.ClosestPoint(fe_planes[i][j].Origin, out u, out v);
                    //            Vector3d normal = s.NormalAt(u, v);
                    //            //Vector3d xaxis = Vector3d.CrossProduct(normal, e_planes[i][j].YAxis);
                    //            //Vector3d yaxis = Vector3d.CrossProduct(normal, xaxis);
                    //            //s.PointAt(u, v)
                    //            fe_planes[i][j] = new Plane(fe_planes[i][j].Origin, Vector3d.CrossProduct(normal, fe_planes[i][j].ZAxis), normal);
                    //
                    //          }
                }//for j
            }//for i
        }

        public void get_bisector_planes()
        {
            bi_planes = new Plane[f][];

            for (int i = 0; i < f; i++)
            {
                int n = f_v[i].Length;
                bi_planes[i] = new Plane[n];
                for (int j = 0; j < n; j++)
                {
                    var pl1 = fe_planes[i][j];
                    var pl0 = fe_planes[i][MathUtil.Wrap(j + 1, n)];
                    bi_planes[i][j] = PlaneUtil.DihedralPlane(pl0, pl1);
                }
            }//for i
        }

        public void get_edges()
        {
            if (this.has_ngons)
            {
                e_ngon_e = this.m._EAll();
                this.e = e_ngon_e.Count;
            }
            else
            {
                e_ngon_e = new Dictionary<int, int>();
                for (int i = 0; i < this.m.TopologyEdges.Count; i++)
                {
                    e_ngon_e.Add(i, i);
                }
            }
            this.e = e_ngon_e.Count;
        }

        public void get_edge_faces()
        {
            if (this.has_ngons)
            {
                this.e_f = this.m._EF();
            }
            else
            {
                this.e_f = new int[this.m.TopologyEdges.Count][];
                for (int i = 0; i < this.m.TopologyEdges.Count; i++)
                    this.e_f[i] = this.m.TopologyEdges.GetConnectedFaces(i);
            }
        }

        //
        //    List<int> found_ids = new List<int>();
        //    public void get_edge_vectors(List<Line> lines){
        //
        //      //Init empty vectors
        //      this.insertion_vectors = new Vector3d[this.e];
        //
        //
        //      //Get edge lines including naked
        //      this.e_lines = new Line[this.e];
        //      foreach(var pair in this.e_ngon_e){
        //        this.e_lines[pair.Value] = this.m.TopologyEdges.EdgeLine(pair.Key);
        //        //RhinoDoc.ActiveDoc.Objects.AddLine(this.e_lines[pair.Value]);
        //        //RhinoApp.WriteLine(string.Format("{0} {1}",pair.Key,pair.Value));
        //      }
        //
        //
        //
        //      //Create rtree and add lines
        //      RTree rTree = new RTree();
        //      for (int i = 0; i < this.e_lines.Length; i++)
        //        rTree.Insert(this.e_lines[i].BoundingBox, i);
        //
        //      //Search tree
        //      for(int i = 0; i < lines.Count;i++ ){
        //        BoundingBox bbox = lines[i].BoundingBox;
        //        bbox.Inflate(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
        //        bbox.Inflate(1);
        //        rTree.Search(lines[i].BoundingBox, BoundingBoxCallback);
        //
        //        //iterate through found ids and check if end point lies on a mesh edge
        //        foreach(var  id in found_ids ){
        //          //RhinoDoc.ActiveDoc.Objects.AddLine(lines[i]);
        //          //RhinoDoc.ActiveDoc.Objects.AddLine(e_lines[id]);
        //          //break;
        //          if( this.e_lines[id].ClosestPoint(lines[i].From, true).DistanceToSquared(lines[i].From) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance){
        //            this.insertion_vectors[id] = lines[i].Direction.UnitVector();
        //            //RhinoApp.WriteLine("Hi");
        //          }else if( this.e_lines[id].ClosestPoint(lines[i].To, true).DistanceToSquared(lines[i].To) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance){
        //            this.insertion_vectors[id] = lines[i].Direction.UnitVector();
        //            // RhinoApp.WriteLine("Hi");
        //          }
        //        }
        //        //break;
        //        // RhinoApp.WriteLine(found_ids.Count.ToString());
        //        found_ids = new List<int>();
        //
        //      }
        //
        //    }
        //
        //    void BoundingBoxCallback(object sender, RTreeEventArgs e)
        //    {
        //      found_ids.Add(e.Id);
        //    }
        //

        //Compute geometry
        public void get_face_polylines()
        {
            bool zero_layer = this.face_positions.Count == 0;

            this.f_polylines_index = new string[f][];
            this.f_polylines_planes = new Plane[f][];
            this.f_polylines = new Polyline[f][];

            for (int i = 0; i < f; i++)
            {
                if (this.face_positions.Count == 0)
                {
                    f_polylines[i] = new Polyline[2];

                    //f_polylines[i][0] = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_thickness * -0.0), fe_planes[i], bi_planes[i], 2);
                    //f_polylines[i][1] = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_thickness * 1.0), fe_planes[i], bi_planes[i], 2);

                    Polyline pline0 = PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_thickness * -0.0), fe_planes[i].ToList(), true);
                    Polyline pline1 = PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_thickness * 1.0), fe_planes[i].ToList(), true);

                    f_polylines_planes[i] = new Plane[1]{
            f_planes[i].MovePlanebyAxis(this.face_thickness * 1.0)
            };

                    f_polylines_index[i] = new string[1]{
            i.ToString()
            };
                }
                else
                {
                    f_polylines[i] = new Polyline[this.face_positions.Count * 2];
                    f_polylines_planes[i] = new Plane[this.face_positions.Count];
                    f_polylines_index[i] = new string[this.face_positions.Count];

                    for (int j = 0; j < this.face_positions.Count; j++)
                    {
                        Polyline pline0 = PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * -0.0), fe_planes[i].ToList(), true);
                        Polyline pline1 = PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * 1.0), fe_planes[i].ToList(), true);
                        // Polyline pline0 = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * -0.0), fe_planes[i], bi_planes[i], 2);
                        // Polyline pline1 = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * 1.0), fe_planes[i], bi_planes[i], 2);
                        //f_polylines[i][j] = rhino_util.PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_thickness[j]), e_planes[i].ToList());
                        f_polylines[i][j * 2 + 0] = pline0;
                        f_polylines[i][j * 2 + 1] = pline1;
                        f_polylines_planes[i][j] = f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * 1.0);

                        f_polylines_index[i][j] = this.face_positions.Count == 1 ? i.ToString() : string.Format("{0}-{1}", i, j);
                    }
                }
            }





        }

        public void get_insertion_vectors()
        {
            for (int i = 0; i < f; i++)
            {
                if (!this.flags[i])
                {
                    Vector3d vec = f_polylines[i][0][2] - f_polylines[i][0][0];
                    vec.Unitize();
                    vec *= 10;

                    for (int j = 0; j < 3; j++)
                    {
                        Point3d mid = (f_polylines[i][0][j] + f_polylines[i][0][(j + 1) % 3]) * 0.5;
                        Line l = new Line(mid, vec);
                        insertion_lines.Add(l);
                    }
                }
            }

            //chamfer
            if (this.chamfer > 0.000001)
            {

                for (int i = 0; i < f; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {

                        f_polylines[i][j] = PolylineUtil.Chamfer(f_polylines[i][j], -this.chamfer);
                    }
                }
            }
        }
    }
}
