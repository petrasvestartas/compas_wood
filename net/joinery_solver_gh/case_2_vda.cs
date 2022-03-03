using Rhino.Geometry;
using rhino_util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace joinery_solver_gh
{
    public class case_2_vda
    {
        //input
        public Mesh m = null;

        public Surface s = null;
        public double face_thickness = 1;
        public List<double> face_positions = new List<double>();
        public List<Brep> projection_breps = new List<Brep>();
        public List<double> edge_divisions = new List<double>();
        public List<double> edge_division_len = new List<double>();
        public Vector3d[] insertion_vectors = new Vector3d[0];

        //indices
        private bool has_ngons = false;

        public int f;
        public int e;
        public int[][] f_v;
        public int[][] f_e;
        public int[][] e_f;
        public Dictionary<int, int> e_ngon_e;
        private double rect_width = 1;
        private double rect_height = 1;
        private double rect_thickness = 1;

        //planes
        public Plane[] f_planes = new Plane[0];

        public Plane[][] fe_planes = new Plane[0][];
        public Plane[][] bi_planes = new Plane[0][];
        public Plane[] e90_planes = new Plane[0];
        public Plane[][] e90_multiple_planes = new Plane[0][];
        public Line[] e_lines = new Line[0];

        //output
        //polylines
        public string[][] f_polylines_index = new string[0][];

        public Plane[][] f_polylines_planes = new Plane[0][];
        public Polyline[][] f_polylines = new Polyline[0][];

        public string[][] e_polylines_index = new string[0][];
        public Plane[][] e_polylines_planes = new Plane[0][];
        public Polyline[][] e_polylines = new Polyline[0][];

        //Division per edge
        //Number of layers
        //Length of joint
        //Rotate or insertion direction per each edge

        public case_2_vda(Mesh m, Surface s,
          double face_thickness, List<double> face_positions, List<double> edge_divisions,
          List<double> edge_division_len, List<Line> insertion_vectors,
        double rect_width, double rect_height, double rect_thickness, List<Brep> projection_breps)
        {
            if (m == null) return;
            if (!m.IsValid) return;
            m.Weld(0.01);
            m.RebuildNormals();
            this.m = m.DuplicateMesh();
            this.s = s;
            this.face_thickness = face_thickness;
            this.face_positions = face_positions;
            this.face_positions.Sort();

            if (this.face_positions.Count == 0)
                this.face_positions = new List<double> { 0 };

            this.projection_breps = projection_breps;
            this.edge_divisions = edge_divisions;
            this.edge_division_len = edge_division_len;
            this.edge_division_len = edge_division_len;
            this.has_ngons = m.Ngons.Count > 0;
            this.rect_width = rect_width;
            this.rect_height = rect_height;
            this.rect_thickness = rect_thickness;
            //run methods in constructor
            get_face_vertices();
            get_faces_planes();
            get_face_edge_planes();
            get_bisector_planes();
            get_face_polylines();
            get_edges();
            get_edge_faces();
            get_edge_vectors(insertion_vectors);
            get_edge_planes();
            get_connectors();
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

            //Adjust to surface
            if (s != null)
            {
                for (int i = 0; i < f_planes.Length; i++)
                {
                    double u, v;
                    s.ClosestPoint(f_planes[i].Origin, out u, out v);
                    Vector3d normal = s.NormalAt(u, v);
                    Vector3d xaxis = Vector3d.CrossProduct(normal, f_planes[i].YAxis);
                    Vector3d yaxis = Vector3d.CrossProduct(normal, xaxis);
                    //f_planes[i] = new Plane(s.PointAt(u, v), xaxis, yaxis);
                    f_planes[i] = new Plane(f_planes[i].Origin, xaxis, yaxis);
                }
            }
        }

        public void get_face_edge_planes()
        {
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

                    Plane edgePlane = new Plane((Point3d)(this.m.Vertices[v1] + this.m.Vertices[v2]) * 0.5, this.m.Vertices[v1] - this.m.Vertices[v2], sum);
                    fe_planes[i][j] = edgePlane;

                    //Adjust to surface
                    if (s != null)
                    {
                        double u, v;
                        s.ClosestPoint(fe_planes[i][j].Origin, out u, out v);
                        Vector3d normal = s.NormalAt(u, v);
                        //Vector3d xaxis = Vector3d.CrossProduct(normal, e_planes[i][j].YAxis);
                        //Vector3d yaxis = Vector3d.CrossProduct(normal, xaxis);
                        //s.PointAt(u, v)
                        fe_planes[i][j] = new Plane(fe_planes[i][j].Origin, Vector3d.CrossProduct(normal, fe_planes[i][j].ZAxis), normal);
                    }

                    foreach (Brep brep in this.projection_breps)
                    {
                        if (brep.IsPointInside(fe_planes[i][j].Origin, 0.01, false))
                        {
                            Transform xform = Rhino.Geometry.Transform.PlanarProjection(Plane.WorldXY);
                            fe_planes[i][j].Transform(xform);
                        }
                    }
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

        private List<int> found_ids = new List<int>();

        public void get_edge_vectors(List<Line> lines)
        {
            //Init empty vectors
            this.insertion_vectors = new Vector3d[this.e];

            //Get edge lines including naked
            this.e_lines = new Line[this.e];
            foreach (var pair in this.e_ngon_e)
            {
                this.e_lines[pair.Value] = this.m.TopologyEdges.EdgeLine(pair.Key);
                //RhinoDoc.ActiveDoc.Objects.AddLine(this.e_lines[pair.Value]);
                //RhinoApp.WriteLine(string.Format("{0} {1}",pair.Key,pair.Value));
            }

            //Create rtree and add lines
            RTree rTree = new RTree();
            for (int i = 0; i < this.e_lines.Length; i++)
                rTree.Insert(this.e_lines[i].BoundingBox, i);

            //Search tree
            for (int i = 0; i < lines.Count; i++)
            {
                BoundingBox bbox = lines[i].BoundingBox;
                bbox.Inflate(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance);
                bbox.Inflate(1);
                rTree.Search(lines[i].BoundingBox, BoundingBoxCallback);

                //iterate through found ids and check if end point lies on a mesh edge
                foreach (var id in found_ids)
                {
                    //RhinoDoc.ActiveDoc.Objects.AddLine(lines[i]);
                    //RhinoDoc.ActiveDoc.Objects.AddLine(e_lines[id]);
                    //break;
                    if (this.e_lines[id].ClosestPoint(lines[i].From, true).DistanceToSquared(lines[i].From) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
                    {
                        this.insertion_vectors[id] = lines[i].Direction.UnitVector();
                        //RhinoApp.WriteLine("Hi");
                    }
                    else if (this.e_lines[id].ClosestPoint(lines[i].To, true).DistanceToSquared(lines[i].To) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
                    {
                        this.insertion_vectors[id] = lines[i].Direction.UnitVector();
                        // RhinoApp.WriteLine("Hi");
                    }
                }
                //break;
                // RhinoApp.WriteLine(found_ids.Count.ToString());
                found_ids = new List<int>();
            }
        }

        private void BoundingBoxCallback(object sender, RTreeEventArgs e)
        {
            //var boxSearchData = e.ToString();
            // RhinoApp.WriteLine(string.Format("{0} {1}",e.Id,e.IdB));
            found_ids.Add(e.Id);
        }

        //todo
        public void get_edge_planes()
        {
            e90_planes = new Plane[e_ngon_e.Count];
            e90_multiple_planes = new Plane[e_ngon_e.Count][];
            for (int i = 0; i < e_ngon_e.Count; i++)
            {
                int id = e_ngon_e.ElementAt(i).Key;

                if (this.m.TopologyEdges.GetConnectedFaces(id).Length == 1)
                {
                    e90_planes[i] = Plane.Unset;
                    e90_multiple_planes[i] = new Plane[] { Plane.Unset };
                    continue;
                }

                Line edge_line = this.m.TopologyEdges.EdgeLine(id);
                Point3d origin = edge_line.Mid();
                Vector3d zaxis = edge_line.Direction.UnitVector();
                Rhino.IndexPair pair = this.m.TopologyEdges.GetTopologyVertices(id);
                //edge vertex normals
                Vector3d yaxis = ((Vector3d)(this.m.Normals[pair.I] + this.m.Normals[pair.J])) * 0.5;
                //or face normals
                yaxis = (this.f_planes[this.e_f[i][0]].ZAxis + this.f_planes[this.e_f[i][1]].ZAxis) * 0.5;

                //this.f_planes[]

                if (this.s != null)
                {
                    double u, v;
                    this.s.ClosestPoint(origin, out u, out v);
                    yaxis = this.s.NormalAt(u, v);
                }

                Vector3d xaxis = Vector3d.CrossProduct(zaxis, yaxis);

                //orient x-axis towards first edge index
                if ((origin + xaxis).DistanceToSquared(this.f_planes[this.e_f[i][0]].Origin) <
                  (origin - xaxis).DistanceToSquared(this.f_planes[this.e_f[i][0]].Origin))
                    xaxis *= -1;

                //Incase insertion vectors are given
                if (Math.Abs(insertion_vectors[i].X)+ Math.Abs(insertion_vectors[i].Y)+ Math.Abs(insertion_vectors[i].Z) >0.01 )
                {
                    //Project given vector to a plane
                    Plane edge_plane = new Plane(this.e_lines[i].Mid(), yaxis);
                    Transform xform = Transform.PlanarProjection(edge_plane);
                    Vector3d xaxis_ = insertion_vectors[i];
                    xaxis_.Transform(xform);

                    if ((Point3d.Origin + xaxis_).DistanceToSquared(Point3d.Origin + xaxis) >
                      (Point3d.Origin - xaxis_).DistanceToSquared(Point3d.Origin + xaxis))
                        xaxis_ *= -1;
                    xaxis = xaxis_;

                    //orient x-axis towards first edge index
                }

                //set plane
                e90_planes[i] = new Plane(origin, xaxis, yaxis);

                //1) By N 2) by division length 3) in both cases consider lists
                //Skip emppty

                int divisions = 1;

                if (this.edge_division_len.Count > 0)
                {
                    if (this.edge_division_len.Count == this.e)
                    {
                        divisions = (int)rhino_util.MathUtil.Constrain((int)Math.Ceiling(edge_line.Length / this.edge_division_len[i]), 1, 10);
                    }
                    else
                    {
                        divisions = (int)rhino_util.MathUtil.Constrain((int)Math.Ceiling(edge_line.Length / this.edge_division_len[0]), 1, 10);
                    }
                }
                else if (this.edge_divisions.Count > 0)
                {
                    if (this.edge_divisions.Count == this.e)
                    {
                        divisions = (int)this.edge_divisions[0];
                    }
                    else
                    {
                        divisions = (int)this.edge_divisions[i];
                    }
                }

                Point3d[] pts = edge_line.InterpolateLine(divisions, false);
                e90_multiple_planes[i] = new Plane[pts.Length];

                for (int j = 0; j < pts.Length; j++)
                {
                    e90_multiple_planes[i][j] = PlaneUtil.ChangeOrigin(e90_planes[i], pts[j]);
                }
            }
        }

        public void get_connectors()
        {
            this.e_polylines = new Polyline[e90_multiple_planes.Length][];
            this.e_polylines_planes = new Plane[e90_multiple_planes.Length][];
            this.e_polylines_index = new string[e90_multiple_planes.Length][];

            for (int i = 0; i < e90_multiple_planes.Length; i++)
            {
                this.e_polylines[i] = new Polyline[e90_multiple_planes[i].Length * 2];
                this.e_polylines_planes[i] = new Plane[e90_multiple_planes[i].Length];
                this.e_polylines_index[i] = new string[e90_multiple_planes[i].Length];

                for (int j = 0; j < e90_multiple_planes[i].Length; j++)
                {
                    if (e90_multiple_planes[i][j] == Plane.Unset)
                    {
                        this.e_polylines[i] = new Polyline[0];
                        //this.e_polylines_planes[i][j] = null;
                        this.e_polylines_index[i][j] = null;
                    }
                    else
                    {
                        if (this.rect_height > 0 && this.rect_width > 0)
                        {
                            //type0

                            Rectangle3d rect0 = new Rectangle3d(
                              e90_multiple_planes[i][j].MovePlanebyAxis(this.rect_thickness * 0.5),
                              new Interval(-rect_width * 0.5, rect_width * 0.5),
                              new Interval(-rect_height * 0.5, rect_height * 0.5));
                            Rectangle3d rect1 = new Rectangle3d(
                              e90_multiple_planes[i][j].MovePlanebyAxis(this.rect_thickness * -0.5),
                              new Interval(-rect_width * 0.5, rect_width * 0.5),
                              new Interval(-rect_height * 0.5, rect_height * 0.5));
                            this.e_polylines[i][j * 2 + 0] = rect0.ToPolyline();
                            this.e_polylines[i][j * 2 + 1] = rect1.ToPolyline();
                        }
                        else if (this.rect_height <= 0 && this.rect_width <= 0)
                        {
                            double w = Math.Abs(this.rect_height);
                            double h = Math.Abs(this.rect_width);

                            //type1
                            Plane e_plane = new Plane(e90_multiple_planes[i][j]);
                            e_plane.Rotate(Math.PI * 0.5, e_plane.YAxis);
                            Plane top_plane_0 = this.f_planes[this.e_f[i][0]].MovePlanebyAxis(this.face_positions.Last()).MovePlanebyAxis(this.face_thickness * 0.5 + h);
                            Plane top_plane_1 = this.f_planes[this.e_f[i][1]].MovePlanebyAxis(this.face_positions.Last()).MovePlanebyAxis(this.face_thickness * 0.5 + h);
                            Plane bot_plane_0 = this.f_planes[this.e_f[i][0]].MovePlanebyAxis(this.face_positions.First()).MovePlanebyAxis(this.face_thickness * -0.5 - h);
                            Plane bot_plane_1 = this.f_planes[this.e_f[i][1]].MovePlanebyAxis(this.face_positions.First()).MovePlanebyAxis(this.face_thickness * -0.5 - h);

                            Plane e_plane_ = new Plane(this.e90_planes[i]);
                            e_plane_.Rotate(Math.PI * 0.5, e_plane_.YAxis);

                            //            if((e_plane_.Origin + e_plane_.ZAxis).DistanceToSquared(this.f_planes[this.e_f[i][0]].Origin) >
                            //              (e_plane_.Origin - e_plane_.ZAxis).DistanceToSquared(this.f_planes[this.e_f[i][0]].Origin))
                            //              e_plane.Flip();

                            Plane[] side_planes = new Plane[]{
                top_plane_0,
                e_plane,
                top_plane_1,
                e_plane.MovePlanebyAxis(w * 0.5),
                bot_plane_1,
                e_plane,
                bot_plane_0,
                e_plane.MovePlanebyAxis(w * -0.5),
                };
                            Polyline type1_0 = rhino_util.PolylineUtil.PolylineFromPlanes(e90_multiple_planes[i][j].MovePlanebyAxis(-this.rect_thickness * 0.5), side_planes.ToList());
                            Polyline type1_1 = type1_0.Translate(e90_multiple_planes[i][j].ZAxis * this.rect_thickness);
                            this.e_polylines[i][j * 2 + 0] = type1_0;
                            this.e_polylines[i][j * 2 + 1] = type1_1;
                        }

                        this.e_polylines_planes[i][j] = e90_multiple_planes[i][j].MovePlanebyAxis(this.rect_thickness * 0.5);
                        this.e_polylines_index[i][j] = string.Format("{0}-{1}_{2}", this.e_f[i][0], this.e_f[i][1], j);

                    }
                }
            }
        }

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

                    f_polylines[i][0] = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_thickness * -0.5), fe_planes[i], bi_planes[i], 2);
                    f_polylines[i][1] = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_thickness * 0.5), fe_planes[i], bi_planes[i], 2);

                    f_polylines_planes[i] = new Plane[1]{
            f_planes[i].MovePlanebyAxis(this.face_thickness * 0.5)
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
                        Polyline pline0 = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * -0.5), fe_planes[i], bi_planes[i], 2);
                        Polyline pline1 = PolylineUtil.OutlineFromFaceEdgeCorner(f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * 0.5), fe_planes[i], bi_planes[i], 2);
                        //f_polylines[i][j] = rhino_util.PolylineUtil.PolylineFromPlanes(f_planes[i].MovePlanebyAxis(this.face_thickness[j]), e_planes[i].ToList());
                        f_polylines[i][j * 2 + 0] = pline0;
                        f_polylines[i][j * 2 + 1] = pline1;
                        f_polylines_planes[i][j] = f_planes[i].MovePlanebyAxis(this.face_positions[j] + this.face_thickness * 0.5);

                        f_polylines_index[i][j] = this.face_positions.Count == 1 ? i.ToString() : string.Format("{0}-{1}", i, j);
                    }
                }
            }
        }
    }
}
