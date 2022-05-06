using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino;
using Rhino.Geometry;
using rhino_util;

namespace joinery_solver_gh
{
    public class chevron
    {
        //User inputs
        public Mesh mesh;

        private double edge_rotation;
        private double edge_offset;
        private double box_height;
        private double top_plate_inlet;
        private double plate_thickness;
        private bool ortho;

        //class generated properties
        public Dictionary<int, int[]> f_e;

        public Dictionary<int, bool> f_rotation_flip;
        public Plane[][] bi_planes = new Plane[0][];
        public Plane[][] e_planes = new Plane[0][];
        public Plane[] f_planes = new Plane[0];

        //output
        public int[][] f_v;

        public int[][] f_me;
        public List<Polyline> plines;//pair of outlines
        public Vector3d[][] insertion_vectors;//v0,v1,v2,v3,v4,v5
        public List<int[]> adjacency;//v0,v1, f0, f1
        public int[][] joints_per_face;//0,0,1,1,2,2
        public List<int[]> three_valence;//43, 41, 45, 41

        //display
        public Vector3d[] box_insertion_vectors;//v0,v1,v2,v3,v4,v5

        public Line[] box_insertion_lines;//v0,v1,v2,v3,v4,v5

        public chevron()
        {
        }

        public chevron(Mesh mesh, double edge_rotation, double edge_offset, double box_height, double top_plate_inlet, double plate_thickness, bool ortho)
        {
            this.mesh = mesh.DuplicateMesh();
            this.mesh.Flip(true, true, true);

            this.edge_rotation = rhino_util.MathUtil.Constrain(edge_rotation, -30, 30);
            this.edge_offset = rhino_util.MathUtil.Constrain(edge_offset, -2, 2);

            this.box_height = rhino_util.MathUtil.Constrain(box_height, 1, double.MaxValue);
            this.top_plate_inlet = rhino_util.MathUtil.Constrain(top_plate_inlet, 1, box_height * 0.333);
            this.plate_thickness = rhino_util.MathUtil.Constrain(plate_thickness, 1, box_height * 0.333);
            this.ortho = ortho;

            //output
            this.plines = new List<Polyline>(mesh.Faces.Count * 8);
            this.insertion_vectors = new Vector3d[mesh.Faces.Count * 4][];
            this.adjacency = new List<int[]>(mesh.Faces.Count * 4);
            this.joints_per_face = new int[mesh.Faces.Count * 4][];
            this.three_valence = new List<int[]>(mesh.Faces.Count * 4);
            this.f_v = new int[mesh.Faces.Count][];
            this.f_me = new int[mesh.Faces.Count][];

            //display
            this.box_insertion_vectors = new Vector3d[mesh.Faces.Count];
            this.box_insertion_lines = new Line[mesh.Faces.Count];
        }

        public Mesh chevron_grid(Surface s, double u_divisions, double v_division_dist, double shift, double scale)
        {
            //Inputs to construct mesh
            var f = new List<MeshFace>();
            var v = new List<Point3d>();

            //Get Surface Interval, divisionStep, baseStep, maxStep and Delta parameters
            Interval DomU = new Interval(s.Domain(0));
            Interval DomV = new Interval(s.Domain(1));
            double half_v = DomV.T1 * 0.5;
            double StepU = (DomU.T1 - DomU.T0) / u_divisions;
            double baseStepV = v_division_dist;
            double maxStepV = 2500;
            double Delta = maxStepV - baseStepV;

            double totalV = DomV[1] - DomV[0];
            double rem = (totalV / 2) - ((maxStepV / 2) + baseStepV + Delta);
            double stepV = rem / baseStepV;

            //Iterate number strips of the NURBS
            double ctU = 0;

            for (int j = 0; j < u_divisions; j++)
            {
                double ctV = 0;
                double thresh = totalV / 2;
                double StepV1 = baseStepV;
                bool b = true;
                var ListV = new List<double>();

                Point3d p0, p1, p2, p6, p7, p8;
                Point3d savept6 = Point3d.Unset;
                Point3d savept7 = Point3d.Unset;
                Point3d savept8 = Point3d.Unset;
                int iterations = 0;

                while (b && iterations < 1000)
                {
                    iterations++;
                    ListV.Add(StepV1);

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Get First Half of a Quads
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    if (ctV == 0)
                    {
                        p0 = s.PointAt(ctU, ctV);
                        p1 = s.PointAt(ctU + StepU * 0.5, ctV);
                        p2 = s.PointAt(ctU + StepU, ctV);
                        p6 = s.PointAt(ctU, ctV + StepV1 * (1 - shift / 2));
                        p7 = s.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + shift / 2));
                        p8 = s.PointAt(ctU + StepU, ctV + StepV1 * (1 - shift / 2));
                        savept6 = p6;
                        savept7 = p7;
                        savept8 = p8;
                    }
                    else
                    {
                        p0 = savept6;
                        p1 = savept7;
                        p2 = savept8;
                        p6 = s.PointAt(ctU, ctV + StepV1 * (1 - shift / 2));
                        p7 = s.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + shift / 2));
                        p8 = s.PointAt(ctU + StepU, ctV + StepV1 * (1 - shift / 2));
                        savept6 = p6;
                        savept7 = p7;
                        savept8 = p8;
                    }

                    //Add mesh faces
                    f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                    v.AddRange(new Point3d[] { p0, p6, p7, p1 });
                    f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                    v.AddRange(new Point3d[] { p1, p7, p8, p2 });

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Increment V Parameter, and multiply by scale due to curvature change
                    //Question what is a the right scale?
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ctV += StepV1;
                    thresh -= StepV1;
                    StepV1 += StepV1 * scale;

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Mirror Side
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    if (ctV + StepV1 > half_v)
                    {
                        //Rhino.RhinoApp.WriteLine(thresh.ToString()+ "     U         " +DomU.ToString() + "      V      "+DomV.ToString());
                        ListV.Add(thresh);
                        ListV.Reverse();
                        double revCt = totalV / 2; //rev counter

                        for (int i = 0; i < ListV.Count - 1; i++)
                        {
                            revCt += ListV[i];

                            if (i == 0)
                            {
                                p0 = s.PointAt(ctU, revCt - ListV[i + 1] * shift / 2);
                                p1 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * shift / 2);
                                p2 = s.PointAt(ctU + StepU, revCt - ListV[i + 1] * shift / 2);

                                f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                                v.AddRange(new Point3d[] { p6, p0, p1, p7 });

                                f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                                v.AddRange(new Point3d[] { p7, p1, p2, p8 });

                                p6 = s.PointAt(ctU, revCt + ListV[i + 1] * (1 - shift / 2));
                                p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + shift / 2));
                                p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - shift / 2));
                                savept6 = p6;
                                savept7 = p7;
                                savept8 = p8;
                            }
                            else if (i == ListV.Count - 2)
                            {
                                p0 = savept6;
                                p1 = savept7;
                                p2 = savept8;
                                p6 = s.PointAt(ctU, revCt + ListV[i + 1]);
                                p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1]);
                                p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1]);
                            }
                            else
                            {
                                p0 = savept6;
                                p1 = savept7;
                                p2 = savept8;
                                p6 = s.PointAt(ctU, revCt + ListV[i + 1] * (1 - shift / 2));
                                p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + shift / 2));
                                p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - shift / 2));
                                savept6 = p6;
                                savept7 = p7;
                                savept8 = p8;
                            }

                            f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                            v.AddRange(new Point3d[] { p1, p7, p8, p2 });
                            f.Add(new MeshFace(v.Count, v.Count + 1, v.Count + 2, v.Count + 3));
                            v.AddRange(new Point3d[] { p0, p6, p7, p1 });
                        }
                        b = false;
                    }
                }

                ctU += StepU;
            }

            //Create Mesh
            Mesh mesh = new Mesh();
            mesh.Vertices.AddVertices(v);
            mesh.Faces.AddFaces(f);
            mesh.Weld(100);
            mesh.RebuildNormals();

            return mesh;
        }

        public int[] get_mesh_face_edges(int f, bool flag)
        {
            int[] edge_id = mesh.TopologyEdges.GetEdgesForFace(f);
            int[] ids = flag ? new int[] { 0, 1 } : new int[] { 1, 2 };
            return ids;
        }

        public void stripper(
          //ref List<Mesh> meshes,
          //ref DataTree<int> tree_face_id,
          //ref DataTree<bool> tree_face_flags,
          //ref DataTree<int> tree_edge_id

          )
        {
            this.f_e = new Dictionary<int, int[]>();
            this.f_rotation_flip = new Dictionary<int, bool>();

            Dictionary<int, int[]> f_e_temp = new Dictionary<int, int[]>();
            Dictionary<int, bool> f_rotation_flip_temp = new Dictionary<int, bool>();

            bool[] flagArray = new bool[mesh.Faces.Count];

            int num = 0;
            for (int i = 0; num < mesh.Faces.Count & i < mesh.Faces.Count; i++)
            {
                int face_1 = 0;
                int num2 = 0;

                //First iteration will be 0
                while (num2 < mesh.Faces.Count)
                {
                    if (flagArray[num2])
                    {
                        //Print(num2.ToString());
                        num2++;
                    }
                    else
                    {
                        face_1 = num2;
                        break;
                    }
                }

                Mesh mesh1 = new Mesh();
                mesh1.Vertices.AddVertices(mesh.Vertices);
                mesh1.Faces.AddFace(mesh.Faces[face_1]);

                //Collect ids
                //tree_face_id.Add(face_1, new GH_Path(face_1));
                //tree_face_flags.Add(i % 2 == 0, new GH_Path(face_1));
                //tree_edge_id.AddRange(get_mesh_face_edges(mesh, face_1, i % 2 == 0), new GH_Path(face_1));

                f_e_temp.Add(face_1, get_mesh_face_edges(face_1, i % 2 == 0));
                f_rotation_flip_temp.Add(face_1, i % 2 == 0);

                List<int> nums = new List<int>();

                nums.Add(face_1);
                flagArray[face_1] = true;
                num++;

                List<bool> flags = new List<bool>() { false };
                bool flag = false;

                while (!flag)
                {
                    bool flag1 = false;
                    for (int j = 0; j < mesh1.Faces.Count; j++)
                    {
                        if (!flags[j])
                        {
                            int[] f_n = mesh.Faces.AdjacentFaces(nums[j]);

                            for (int k = 0; k < (int)f_n.Length; k++)
                            {
                                if (!flagArray[f_n[k]])
                                {
                                    MeshFace f = mesh1.Faces[j];
                                    int a = f.A;
                                    int b = f.B;
                                    int c = f.C;
                                    int d = f.D;
                                    f = mesh.Faces[f_n[k]];
                                    int a1 = f.A;
                                    int b1 = f.B;
                                    int c1 = f.C;
                                    int d1 = f.D;
                                    if (a == b1 && b == a1 || c == d1 && d == c1 || c == b1 && d == a1 || b == c1 && a == d1)
                                    {
                                        //Collect ids
                                        //tree_face_id.Add(f_n[k], new GH_Path(f_n[k]));
                                        //tree_face_flags.Add(i % 2 == 0, new GH_Path(f_n[k]));
                                        //tree_edge_id.AddRange(get_mesh_face_edges(mesh, face_1, i % 2 == 0), new GH_Path(f_n[k]));
                                        f_e_temp.Add(f_n[k], get_mesh_face_edges(f_n[k], i % 2 == 0));
                                        f_rotation_flip_temp.Add(f_n[k], i % 2 == 0);

                                        //Create mesh
                                        mesh1.Faces.AddFace(mesh.Faces[f_n[k]]);
                                        nums.Add(f_n[k]);
                                        flags.Add(false);
                                        flagArray[f_n[k]] = true;
                                        num++;
                                        flag1 = true;
                                    }
                                }
                            }
                            flags[j] = true;
                        }
                    }

                    if (flag1)
                        continue;

                    flag = true;
                    mesh1.Vertices.CullUnused();
                    //meshes.Add(mesh1);
                }

                var f_e_temp_keys = f_e_temp.Keys.ToArray();
                if (i % 2 == 0)
                {
                    Array.Reverse(f_e_temp_keys);
                }

                //for(int j = f_e_temp_keys.Length - 1; j >= 0; j--){
                for (int j = 0; j < f_e_temp_keys.Length; j++)
                {
                    f_e.Add(
                      f_e_temp_keys[j],
                      f_e_temp[f_e_temp_keys[j]]
                      );

                    f_rotation_flip.Add(
                      f_e_temp_keys[j],
                      f_rotation_flip_temp[f_e_temp_keys[j]]
                      );
                }

                f_e_temp = new Dictionary<int, int[]>();
                f_rotation_flip_temp = new Dictionary<int, bool>();
            }
            //return meshes;
        }

        public void update_neighbour_plane(Plane update_plane, int curr_face, int curr_edge)
        {
            int[] face_neighbors = this.mesh.TopologyEdges.GetConnectedFaces(curr_edge);
            if (face_neighbors.Length == 1) return;

            if (face_neighbors[0] == curr_face)
                Array.Reverse(face_neighbors);

            int[] neighbor_edges = new int[4];
            for (int i = 0; i < 4; i++)
                neighbor_edges[i] = mesh.TopologyEdges.GetEdgeIndex(f_v[face_neighbors[0]][i], f_v[face_neighbors[0]][(i + 1) % 4]);

            //  mesh.TopologyEdges.GetEdgesForFace(face_neighbors[0]);
            int neighbor_edge = Array.IndexOf(neighbor_edges, curr_edge);

            e_planes[face_neighbors[0]][neighbor_edge] = new Plane(update_plane.Origin, -update_plane.XAxis, update_plane.YAxis);
        }

        public void get_edge_planes()
        {
            try
            {
                for (int i = 0; i < mesh.Faces.Count; i++)
                    f_v[i] = new int[] { mesh.Faces[i][0], mesh.Faces[i][1], mesh.Faces[i][2], mesh.Faces[i][3] };

                e_planes = new Plane[f_v.Length][];
                f_planes = new Plane[f_v.Length];

                for (int i = 0; i < f_v.Length; i++)
                {
                    int n = f_v[i].Length;
                    e_planes[i] = new Plane[n];
                    f_planes[i] = new Plane(mesh.MeshFaceCenter(i), mesh.FaceNormals[i]);
                    f_me[i] = new int[f_v[i].Length];

                    for (int j = 0; j < f_v[i].Length; j++)
                    {
                        // int v0 = fv[i][MathUtil.Wrap(j - 1, n)];
                        int v1 = f_v[i][(j + 0) % n];
                        int v2 = f_v[i][(j + 1) % n];

                        int e = mesh.TopologyEdges.GetEdgeIndex(v1, v2);
                        f_me[i][j] = e;
                        int[] cf = mesh.TopologyEdges.GetConnectedFaces(e);

                        Vector3d sum = Vector3d.Zero;
                        for (int k = 0; k < cf.Length; k++)
                            sum += mesh.FaceNormals[cf[k]];
                        sum /= cf.Length;

                        Plane edgePlane = new Plane(
                          (Point3d)(mesh.Vertices[v1] + mesh.Vertices[v2]) * 0.5,
                          mesh.Vertices[v1] - mesh.Vertices[v2], sum);

                        e_planes[i][j] = edgePlane;
                    }//for j
                }//for i
            }
            catch (Exception e)
            {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
        }

        public Plane orthogonalize_planes(Plane plane)
        {
            double[] zaxis_coord_abs = new double[] { Math.Abs(plane.ZAxis.X), Math.Abs(plane.ZAxis.Y), Math.Abs(plane.ZAxis.Z) };
            double[] zaxis_coord = new double[] { (plane.ZAxis.X), (plane.ZAxis.Y), (plane.ZAxis.Z) };
            int[] id = new int[] { 0, 1, 2 };

            //return plane;
            Array.Sort(zaxis_coord_abs, id);
            double coord = Math.Sign(zaxis_coord[id[2]]) * Math.Ceiling(Math.Abs(zaxis_coord[id[2]]));

            if (id[2] == 0)
            {
                var plane0 = new Plane(plane.Origin, new Vector3d(coord, 0, 0));
                //RhinoApp.WriteLine(plane0.ToString());
                return plane0;
            }
            else if (id[2] == 1)
            {
                var plane0 = new Plane(plane.Origin, new Vector3d(0, coord, 0));

                //RhinoApp.WriteLine(plane0.ToString());
                return plane0;
            }
            else
            {
                var plane0 = new Plane(plane.Origin, new Vector3d(0, 0, coord));
                //RhinoApp.WriteLine(plane0.ToString());
                return plane0;
            }
        }

        public void rotate_planes()
        {
            for (int i = 0; i < mesh.Faces.Count; i++)
            {
                //int[] edges = mesh.TopologyEdges.GetEdgesForFace(i);

                for (int j = 0; j < 4; j++)
                {
                    int e = mesh.TopologyEdges.GetEdgeIndex(f_v[i][j], f_v[i][(j + 1) % 4]);

                    if (mesh.TopologyEdges.GetConnectedFaces(e).Length == 2)
                    {
                        int sign = this.f_rotation_flip[i] ? -1 : 1;
                        sign = j % 2 == 0 ? sign * 1 : sign * -1;
                        sign = 1;

                        if (Array.IndexOf(f_e[i], j) != -1)
                        {
                            if (j % 2 == 0 && this.f_rotation_flip[i])
                            {
                                //sign *= -1;
                                //e_planes[i][j].Rotate(RhinoMath.ToRadians(this.edge_rotation * sign), e_planes[i][j].YAxis);
                                //update_neighbour_plane(e_planes[i][j], i, e);

                                e_planes[i][j] = PlaneUtil.MovePlanebyAxis(e_planes[i][j], this.plate_thickness * this.edge_offset);
                                update_neighbour_plane(e_planes[i][j], i, e);
                                // continue;
                            }
                            if (j % 2 == 0 && !this.f_rotation_flip[i])
                            {
                                //sign *= 1;
                                //e_planes[i][j].Rotate(RhinoMath.ToRadians(this.edge_rotation * sign * 1), e_planes[i][j].YAxis);
                                update_neighbour_plane(e_planes[i][j], i, e);

                                //RhinoApp.WriteLine(this.edge_offset.ToString());
                                e_planes[i][j] = PlaneUtil.MovePlanebyAxis(e_planes[i][j], this.plate_thickness * this.edge_offset);
                                update_neighbour_plane(e_planes[i][j], i, e);
                                //continue;
                            }
                            if (j % 2 == 1 && this.f_rotation_flip[i])
                            {
                                //RhinoApp.WriteLine(this.edge_offset.ToString());
                                //e_planes[i][j] = PlaneUtil.MovePlanebyAxis(e_planes[i][j], this.plate_thickness * this.edge_offset);
                                sign *= 1;
                                e_planes[i][j].Rotate(RhinoMath.ToRadians(this.edge_rotation * sign * 1), e_planes[i][j].YAxis);

                                update_neighbour_plane(e_planes[i][j], i, e);
                                sign *= 1;
                                continue;
                            }
                            if (j % 2 == 1 && !this.f_rotation_flip[i])
                            {
                                sign *= -1;
                                e_planes[i][j].Rotate(RhinoMath.ToRadians(this.edge_rotation * sign * 1), e_planes[i][j].YAxis);
                                //e_planes[i][j] = PlaneUtil.MovePlanebyAxis(e_planes[i][j], this.plate_thickness * this.edge_offset);
                                update_neighbour_plane(e_planes[i][j], i, e);
                                continue;
                            }
                        }
                    }
                    else if (this.ortho)
                    {
                        e_planes[i][j] = orthogonalize_planes(e_planes[i][j]);
                    }
                }
            }
        }

        public void get_bisector_planes()
        {
            bi_planes = new Plane[mesh.Faces.Count][];

            for (int i = 0; i < mesh.Faces.Count; i++)
            {
                bi_planes[i] = new Plane[4];
                for (int j = 0; j < 4; j++)
                {
                    var pl1 = e_planes[i][j];
                    var pl0 = e_planes[i][MathUtil.Wrap(j + 1, 4)];
                    bi_planes[i][j] = PlaneUtil.DihedralPlane(pl0, pl1);
                }
            }//for i
        }

        public void get_plates()
        {
            foreach (KeyValuePair<int, int[]> o in f_e)
            {
                int f = o.Key;
                int[] e = o.Value;
                //int[] all_e = mesh.TopologyEdges.GetEdgesForFace(f);

                //Rhino.RhinoApp.WriteLine(e[0].ToString());
                //Rhino.RhinoApp.WriteLine(e[1].ToString());

                //Offset Planes
                var e_planes_local = new List<Plane>(4);
                for (int j = 0; j < 4; j++)
                {
                    Plane pl = Array.IndexOf(e, j) != -1 ? PlaneUtil.MovePlanebyAxis(e_planes[f][j], this.plate_thickness) : e_planes[f][j];
                    e_planes_local.Add(pl);
                }

                //Get Top Plates

                plines.Add(PolylineUtil.PolylineFromPlanes(PlaneUtil.MovePlanebyAxis(this.f_planes[f], this.box_height * 0.5 - this.top_plate_inlet - this.plate_thickness * 0.5), e_planes_local));
                plines.Add(PolylineUtil.PolylineFromPlanes(PlaneUtil.MovePlanebyAxis(this.f_planes[f], this.box_height * 0.5 - this.top_plate_inlet + this.plate_thickness * 0.5), e_planes_local));

                plines.Add(PolylineUtil.PolylineFromPlanes(PlaneUtil.MovePlanebyAxis(this.f_planes[f], -this.box_height * 0.5 + this.top_plate_inlet - this.plate_thickness * 0.5), e_planes_local));
                plines.Add(PolylineUtil.PolylineFromPlanes(PlaneUtil.MovePlanebyAxis(this.f_planes[f], -this.box_height * 0.5 + this.top_plate_inlet + this.plate_thickness * 0.5), e_planes_local));

                //Get Side Plates
                for (int j = 0; j < e.Length; j++)
                {
                    int curr = e[j];
                    int prev = MathUtil.Wrap(e[j] - 1, 4);
                    int next = MathUtil.Wrap(e[j] + 1, 4);

                    //Side planes
                    Plane side_plane0 = PlaneUtil.MovePlanebyAxis(this.f_planes[f], this.box_height * 0.5);
                    //Plane side_plane1 = this.bi_planes[f][curr];
                    Plane side_plane1 = j == 0 ? e_planes[f][prev] : this.bi_planes[f][prev];
                    Plane side_plane2 = PlaneUtil.MovePlanebyAxis(this.f_planes[f], -this.box_height * 0.5);
                    Plane side_plane3 = j != 0 ? e_planes[f][next] : this.bi_planes[f][curr];
                    // Plane side_plane3 = this.bi_planes[f][prev];
                    var side_planes = new List<Plane> { side_plane0, side_plane1, side_plane2, side_plane3 };

                    //Top and bottom plane plane
                    Plane base_plane0 = this.e_planes[f][curr];
                    Plane base_plane1 = PlaneUtil.MovePlanebyAxis(this.e_planes[f][curr], this.plate_thickness);

                    plines.Add(PolylineUtil.PolylineFromPlanes(base_plane0, side_planes));
                    plines.Add(PolylineUtil.PolylineFromPlanes(base_plane1, side_planes));
                }
            }
        }

        //output
        //    public List<Polyline> plines;//pair of outlines
        //    public Vector3d[][] insertion_vectors;//v0,v1,v2,v3,v4,v5
        //    public int[][] adjacency;//0,1
        //    public int[][] joints_per_face;//0,0,1,1,2,2
        //    public List<int[]> three_valence;//43, 41, 45, 41
        public void get_joinery_solver_output()
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Insertion vectors
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            int counter = 0;
            foreach (KeyValuePair<int, int[]> o in f_e)
            {
                int f = o.Key;
                //RhinoApp.WriteLine(f.ToString());
                int[] e = o.Value;
                Array.Sort(e);
                if (e[0] == 0 && e[1] == 3)
                    Array.Reverse(e);

                //Get vertices to construct vectors
                int[] v = new int[] { this.mesh.Faces[f].A, this.mesh.Faces[f].B, this.mesh.Faces[f].C, this.mesh.Faces[f].D };
                int v0_0 = v[e[0]];
                int v0_1 = v[(e[0] + 1) % 4];
                int v1_0 = v[e[1]];
                int v1_1 = v[(e[1] + 1) % 4];

                Vector3d dir0 = this.mesh.Vertices[v0_1] - this.mesh.Vertices[v0_0];
                Vector3d dir1 = this.mesh.Vertices[v0_1] - this.mesh.Vertices[v1_1];
                dir0.Unitize();
                dir1.Unitize();

                Line l0 = PlaneUtil.PlanePlane(this.f_planes[f], this.bi_planes[f][e[0]]);
                Vector3d bisector_dir0 = l0.To - l0.From;
                bisector_dir0.Unitize();

                Line l1 = PlaneUtil.PlanePlane(this.f_planes[f], this.bi_planes[f][(e[1] + 1) % 4]);
                Vector3d bisector_dir1 = l1.To - l1.From;
                bisector_dir1.Unitize();

                //
                //        Vector3d dir_box = dir1 + dir0;
                //        dir_box = bisector_dir1;
                //
                //        dir_box = dir_box.UnitVector();

                //Assign vectors per box
                this.box_insertion_vectors[counter] = bisector_dir1;
                this.box_insertion_lines[counter] = new Line(
                  PolylineUtil.CenterPoint(this.plines[counter * 8 + 1]),
                  PolylineUtil.CenterPoint(this.plines[counter * 8 + 1]) + bisector_dir1 * 300);

                //Assign vectors per face

                //top and bottom plate
                var insertions = new Vector3d[] { Vector3d.Zero, Vector3d.Zero, bisector_dir1, bisector_dir1, bisector_dir1, bisector_dir1 };
                insertions[2 + ((e[1] + 2) % 4)] = bisector_dir0;
                insertions[2 + ((e[1] + 3) % 4)] = bisector_dir0;
                this.insertion_vectors[counter * 4 + 0] = insertions;
                this.insertion_vectors[counter * 4 + 1] = insertions;

                //side plates
                this.insertion_vectors[counter * 4 + 2] = new Vector3d[] { Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero };
                this.insertion_vectors[counter * 4 + 3] = new Vector3d[] { Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero };
                counter++;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Flatten ID Mapping
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            counter = 0;
            var dict_plate_id_meshface_id = new Dictionary<int, int>();
            foreach (KeyValuePair<int, int[]> o in f_e)
                dict_plate_id_meshface_id.Add(o.Key, counter++);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Adjacency
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            HashSet<long> unique_key = new HashSet<long>();
            //Box adjacency
            counter = 0;
            foreach (KeyValuePair<int, int[]> o in f_e)
            {
                int f = o.Key;
                int[] e = o.Value;

                //Current box
                this.adjacency.Add(new int[] { counter * 4 + 0, counter * 4 + 2, -1, -1 });
                this.adjacency.Add(new int[] { counter * 4 + 0, counter * 4 + 3, -1, -1 });
                this.adjacency.Add(new int[] { counter * 4 + 1, counter * 4 + 2, -1, -1 });
                this.adjacency.Add(new int[] { counter * 4 + 1, counter * 4 + 3, -1, -1 });
                this.adjacency.Add(new int[] { counter * 4 + 2, counter * 4 + 3, -1, -1 });
                //Neighbors
                int m_e_0 = this.f_me[f][e[0]];// mesh.TopologyEdges.GetEdgeIndex(v[e[0]], v[(e[0] + 1) % 4]);
                int m_e_1 = this.f_me[f][e[1]];//mesh.TopologyEdges.GetEdgeIndex(v[e[1]], v[(e[1] + 1) % 4]);

                int[] f0 = this.mesh.TopologyEdges.GetConnectedFaces(m_e_0);
                if (f0.Length == 2)
                {
                    int next_face = f0[0] == f ? f0[1] : f0[0];
                    this.adjacency.Add(new int[] { dict_plate_id_meshface_id[next_face] * 4 + 0, counter * 4 + 2, -1, -1 });
                    this.adjacency.Add(new int[] { dict_plate_id_meshface_id[next_face] * 4 + 1, counter * 4 + 2, -1, -1 });
                }

                int[] f1 = this.mesh.TopologyEdges.GetConnectedFaces(m_e_1);
                if (f1.Length == 2)
                {
                    int next_face = f1[0] == f ? f1[1] : f1[0];
                    this.adjacency.Add(new int[] { dict_plate_id_meshface_id[next_face] * 4 + 0, counter * 4 + 3, -1, -1 });
                    this.adjacency.Add(new int[] { dict_plate_id_meshface_id[next_face] * 4 + 1, counter * 4 + 3, -1, -1 });
                }

                counter++;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Three Valence
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////

            counter = 0;
            foreach (KeyValuePair<int, int[]> o in f_e)
            {
                int f = o.Key;
                int[] e = o.Value;

                // RhinoApp.WriteLine(f.ToString());
                //RhinoApp.WriteLine(e[0].ToString());
                // RhinoApp.WriteLine(e[1].ToString()) ;

                int[] v = new int[] { this.mesh.Faces[f].A, this.mesh.Faces[f].B, this.mesh.Faces[f].C, this.mesh.Faces[f].D };
                //for(int j = 0; j<4;j++)
                //v[j] =    this.mesh.TopologyVertices.TopologyVertexIndex(v[j]);

                int m_e_0 = this.f_me[f][e[0]];// mesh.TopologyEdges.GetEdgeIndex(v[e[0]], v[(e[0] + 1) % 4]);
                int m_e_1 = this.f_me[f][e[1]];//mesh.TopologyEdges.GetEdgeIndex(v[e[1]], v[(e[1] + 1) % 4]);

                //RhinoApp.WriteLine(m_e_0.ToString());
                //RhinoApp.WriteLine(m_e_1.ToString());

                int[] e_f_0 = this.mesh.TopologyEdges.GetConnectedFaces(m_e_0);
                //RhinoApp.WriteLine(e_f_0.Length.ToString());
                if (e_f_0.Length == 2)
                {
                    int f_0 = e_f_0[Math.Abs(Array.IndexOf(e_f_0, f) - 1)];
                    int nei0 = dict_plate_id_meshface_id[f_0];
                    this.three_valence.Add(new int[] { counter * 4 + 0, counter * 4 + 2, nei0 * 4 + 0, counter * 4 + 2 });
                    this.three_valence.Add(new int[] { counter * 4 + 1, counter * 4 + 2, nei0 * 4 + 1, counter * 4 + 2 });
                }

                int[] e_f_1 = this.mesh.TopologyEdges.GetConnectedFaces(m_e_1);
                //RhinoApp.WriteLine(e_f_1.Length.ToString());
                if (e_f_1.Length == 2)
                {
                    int f_1 = e_f_1[Math.Abs(Array.IndexOf(e_f_1, f) - 1)];
                    int nei1 = dict_plate_id_meshface_id[f_1];
                    this.three_valence.Add(new int[] { counter * 4 + 0, counter * 4 + 3, nei1 * 4 + 0, counter * 4 + 3 });
                    this.three_valence.Add(new int[] { counter * 4 + 1, counter * 4 + 3, nei1 * 4 + 1, counter * 4 + 3 });
                }
                //break;

                counter++;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Joints per face
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            counter = 0;
            foreach (KeyValuePair<int, int[]> o in f_e)
            {
                int f = o.Key;
                int[] e = o.Value;

                //top-bottom
                this.joints_per_face[counter * 4 + 0] = new int[] { 0, 0, 20, 20, 20, 20 };
                this.joints_per_face[counter * 4 + 1] = new int[] { 0, 0, 20, 20, 20, 20 };

                //sides
                //set all to 0 except the dovetail
                int a = e[0] == 3 && e[1] == 0 ? 1 : 0;
                int b = e[0] == 3 && e[1] == 0 ? 0 : 1;
                this.joints_per_face[counter * 4 + 2 + a] = new int[] { 0, 0, 0, 0, 10, 0 };
                this.joints_per_face[counter * 4 + 2 + b] = new int[] { 0, 0, 10, 0, 0, 0 };
                counter++;
            }
        }

        public void run()
        {
            //var watch = System.Diagnostics.Stopwatch.StartNew();

            stripper();

            //watch.Stop();
            //RhinoApp.WriteLine("Loop 1 Execution Time: {watch.ElapsedMilliseconds} ms");
            //watch.Reset();

            get_edge_planes();
            rotate_planes();
            get_bisector_planes();
            get_plates();
            get_joinery_solver_output();
        }
    }
}
