using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino.Geometry;
using Rhino;

namespace joinery_solver_net
{
    public static class Test
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Basic Method and Examples for PInvoke
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        public static int get_square(int n)
        {
            return Unsafe.test_get_square(n);
        }

        public static void test_set_coord()
        {
            double[] coord = new double[] { 0.1, 0.2, 0.3 };
            int coord_size = 3;
            Unsafe.test_set_array(coord, coord_size);
        }

        public static void test_get_coord()
        {
            IntPtr coord_out = IntPtr.Zero;
            int coord_size_out = 0;
            Unsafe.test_get_array(ref coord_out, ref coord_size_out);
            double[] sharp_coord = new double[coord_size_out];
            System.Runtime.InteropServices.Marshal.Copy(coord_out, sharp_coord, 0, sharp_coord.Length);
            Unsafe.release_double(coord_out, true);
            foreach (double o in sharp_coord)
                Console.Write(o.ToString() + "\n");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Rhino Helpers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        private static void list_coord(ref List<Rhino.Geometry.Polyline> polylines, out int[] f, out int f_s, out double[] v, out int v_s)
        {
            //f = null;
            //f_s = 0;
            //double[] v = null;
            v_s = 0;

            f_s = polylines.Count;
            f = new int[polylines.Count];

            for (int i = 0; i < polylines.Count; i++)
            {
                f[i] = polylines[i].Count;
                v_s += polylines[i].Count * 3;
            }
            v = new double[v_s];

            int count = 0;
            for (int i = 0; i < polylines.Count; i++)
            {
                for (int j = 0; j < polylines[i].Count; j++)
                {
                    v[count * 3 + 0] = polylines[i][j].X;
                    v[count * 3 + 1] = polylines[i][j].Y;
                    v[count * 3 + 2] = polylines[i][j].Z;
                    count++;
                }
            }
        }

        private static void list_coord(ref Polyline[][] polylines, out int[] f, out int f_s, out double[] v, out int v_s)
        {
            f_s = 0;
            v_s = 0;
            for (int i = 0; i < polylines.Length; i++)
            {
                f_s += polylines[i].Length;//number of faces
                for (int j = 0; j < polylines[i].Length; j++)
                    v_s += polylines[i][j].Count * 3;//number of vertices
            }
            f = new int[f_s];
            v = new double[v_s];

            //Asign faces
            int counter = 0;
            for (int i = 0; i < polylines.Length; i++)
                for (int j = 0; j < polylines[i].Length; j++)
                    f[counter++] = polylines[i][j].Count;

            //Assign vertices
            int count = 0;
            for (int i = 0; i < polylines.Length; i++)
            {
                for (int j = 0; j < polylines[i].Length; j++)
                {
                    for (int k = 0; k < polylines[i][j].Count; k++)
                    {
                        v[count * 3 + 0] = polylines[i][j][k].X;
                        v[count * 3 + 1] = polylines[i][j][k].Y;
                        v[count * 3 + 2] = polylines[i][j][k].Z;
                        count++;
                    }
                }
            }
        }

        private static void list_coord(ref Vector3d[][] vectors, out int[] f, out int f_s, out double[] v, out int v_s)
        {
            //f = null;
            //f_s = 0;
            //double[] v = null;
            v_s = 0;

            f_s = vectors.Count();
            f = new int[vectors.Count()];

            for (int i = 0; i < vectors.Count(); i++)
            {
                f[i] = vectors[i].Count();
                v_s += vectors[i].Count() * 3;
            }
            v = new double[v_s];

            int count = 0;
            for (int i = 0; i < vectors.Count(); i++)
            {
                for (int j = 0; j < vectors[i].Count(); j++)
                {
                    v[count * 3 + 0] = vectors[i][j].X;
                    v[count * 3 + 1] = vectors[i][j].Y;
                    v[count * 3 + 2] = vectors[i][j].Z;
                    count++;
                }
            }
        }

        private static void list_coord(ref List<List<Vector3d>> vectors, out int[] f, out int f_s, out double[] v, out int v_s)
        {
            //f = null;
            //f_s = 0;
            //double[] v = null;
            v_s = 0;

            f_s = vectors.Count;
            f = new int[vectors.Count];

            for (int i = 0; i < vectors.Count; i++)
            {
                f[i] = vectors[i].Count;
                v_s += vectors[i].Count * 3;
            }
            v = new double[v_s];

            int count = 0;
            for (int i = 0; i < vectors.Count; i++)
            {
                for (int j = 0; j < vectors[i].Count; j++)
                {
                    v[count * 3 + 0] = vectors[i][j].X;
                    v[count * 3 + 1] = vectors[i][j].Y;
                    v[count * 3 + 2] = vectors[i][j].Z;
                    count++;
                }
            }
        }

        private static void list_coord(ref int[][] numbers, out int[] f, out int f_s, out int[] v, out int v_s)
        {
            v_s = 0;
            f_s = numbers.Count();
            f = new int[numbers.Count()];

            for (int i = 0; i < numbers.Count(); i++)
            {
                f[i] = numbers[i].Count();
                v_s += numbers[i].Count() * 1;
            }
            v = new int[v_s];

            int count = 0;
            for (int i = 0; i < numbers.Count(); i++)
            {
                for (int j = 0; j < numbers[i].Count(); j++)
                {
                    v[count * 1 + 0] = numbers[i][j];
                    count++;
                }
            }
        }

        private static void list_coord(ref List<List<int>> numbers, out int[] f, out int f_s, out int[] v, out int v_s)
        {
            v_s = 0;
            f_s = numbers.Count;
            f = new int[numbers.Count];

            for (int i = 0; i < numbers.Count; i++)
            {
                f[i] = numbers[i].Count;
                v_s += numbers[i].Count * 1;
            }
            v = new int[v_s];

            int count = 0;
            for (int i = 0; i < numbers.Count; i++)
            {
                for (int j = 0; j < numbers[i].Count; j++)
                {
                    v[count * 1 + 0] = numbers[i][j];
                    count++;
                }
            }
        }

        private static void list_coord(ref List<int[]> numbers, out int[] f, out int f_s, out int[] v, out int v_s)
        {
            v_s = 0;
            f_s = numbers.Count;
            f = new int[numbers.Count];

            for (int i = 0; i < numbers.Count; i++)
            {
                f[i] = numbers[i].Count();
                v_s += numbers[i].Count() * 1;
            }
            v = new int[v_s];

            int count = 0;
            for (int i = 0; i < numbers.Count; i++)
            {
                for (int j = 0; j < numbers[i].Count(); j++)
                {
                    v[count * 1 + 0] = numbers[i][j];
                    count++;
                }
            }
        }

        private static void list_coord(ref List<List<int>> numbers, out int[] v, out int v_s)
        {
            v_s = 0;

            for (int i = 0; i < numbers.Count; i++)
            {
                v_s += numbers[i].Count * 1;
            }
            v = new int[v_s];

            int count = 0;
            for (int i = 0; i < numbers.Count; i++)
            {
                for (int j = 0; j < numbers[i].Count; j++)
                {
                    v[count * 1 + 0] = numbers[i][j];
                    count++;
                }
            }
        }

        private static void list_coord(ref List<int[]> numbers, out int[] v, out int v_s)
        {
            v_s = 0;

            for (int i = 0; i < numbers.Count; i++)
            {
                v_s += numbers[i].Count() * 1;
            }
            v = new int[v_s];

            int count = 0;
            for (int i = 0; i < numbers.Count; i++)
            {
                for (int j = 0; j < numbers[i].Count(); j++)
                {
                    v[count * 1 + 0] = numbers[i][j];
                    count++;
                }
            }
        }

        private static void list_coord(ref List<double> numbers, out double[] f, out int f_s)
        {
            f_s = numbers.Count;
            f = System.Linq.Enumerable.ToArray(numbers);
        }

        private static void list_coord(ref List<int> numbers, out int[] f, out int f_s)
        {
            f_s = numbers.Count;
            f = System.Linq.Enumerable.ToArray(numbers);
        }

        private static void coord_list(ref IntPtr groups_f, ref IntPtr out_f, ref IntPtr out_v, ref int groups_f_s, ref int out_f_s, ref int out_v_s, ref Polyline[][] polylines)
        {
            //transfer data from c++
            //Console.WriteLine(groups_f_s.ToString());
            int[] sharp_groups_f = new int[groups_f_s];
            int[] sharp_out_f = new int[out_f_s];
            double[] sharp_out_v = new double[out_v_s];
            System.Runtime.InteropServices.Marshal.Copy(groups_f, sharp_groups_f, 0, sharp_groups_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_f, sharp_out_f, 0, sharp_out_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_v, sharp_out_v, 0, sharp_out_v.Length);
            //Unsafe.release_int(groups_f, true);
            //Unsafe.release_int(out_f, true);
            //Unsafe.release_double(out_v, true);

            //create c# polylines
            polylines = new Polyline[groups_f_s][];

            int face_count = 0;
            for (int i = 0; i < groups_f_s; i++)
            {
                polylines[i] = new Polyline[sharp_groups_f[i]];
                //Console.WriteLine();
                //Console.WriteLine(sharp_groups_f[i].ToString());

                for (int j = 0; j < sharp_groups_f[i]; j++)
                {
                    polylines[i][j] = new Polyline(sharp_out_f[face_count]);
                    //Console.WriteLine(sharp_out_f[face_count]);
                    face_count++;
                }
            }

            //Console.WriteLine();
            //fill polylines with points
            int a = 0;
            int b = 0;
            int pline_count = 0;
            for (int i = 0; i < sharp_out_v.Length; i += 3)
            {
                //Console.WriteLine(sharp_out_v[i + 0].ToString());
                //Console.WriteLine(sharp_out_v[i + 1].ToString());
                //Console.WriteLine(sharp_out_v[i + 2].ToString());
                polylines[a][b].Add(sharp_out_v[i + 0], sharp_out_v[i + 1], sharp_out_v[i + 2]);

                if (polylines[a][b].Count() == sharp_out_f[pline_count])
                {//check if the current polyline is filled
                    if (b == (sharp_groups_f[a] - 1))
                    {//number of current polylines == group count index
                        //Console.WriteLine("");
                        //Console.WriteLine("New group");
                        //Console.WriteLine("");
                        //New group
                        a++;
                        b = 0;
                    }
                    else
                    {
                        //Console.WriteLine("");
                        //Console.WriteLine("Next polyline");
                        //Console.WriteLine("");
                        //Or Next polyline
                        b++;
                    }
                    pline_count++;
                }
            }

            //Console.WriteLine("");
            //foreach (var plines in polylines) {
            //    Console.WriteLine("");
            //    Console.WriteLine("New Group");
            //    foreach (var pline in plines) {
            //        Console.WriteLine("New Polyline");
            //        foreach (var p in pline)
            //            Console.WriteLine(p.ToString());
            //    }
            //}
            //Console.WriteLine("");
            //Console.WriteLine(groups_f_s);
            //Console.WriteLine(out_f_s);
            //Console.WriteLine(out_v_s);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Implementations
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //            Parameters
        //----------
        //    polylines_vertices_XYZ: pairs of polylines
        //   face_vectors_XYZ : vectors to orient a joint
        //   face_joints_types_int : joint id, existing in the code
        //    three_valance_element_indices_and_instruction: special case e.g.alignment of rectangles
        //  adjacency : a list of plate id integer pairs

        //Where are joint parameters

        //  search_type : 0 - face - to - face, 1 - plane - to - face(cross), 2 - all
        //    division_distance: division parameter if tile allows this
        //    shift: distance value if tile allows this
        //    output_type: 0 - top outlines and one joint_area, 1 - top outlines and two joint lines,
        //    2 - top outlines and joint volumes first and third, 3 - top outlines and joint polylines,
        //    4 - get_joints_geometry_as_closed_polylines_performing_intersection
        //    triangulate: 0 - not output as mesh, 1 - output as mesh, currently not supported in python

        //   Returns
        //    -------
        //    List of Polylines
        //        with added joints
        public static void pinvoke_get_connection_zones(
            ref Polyline[][] polylines,
            Data data,
            List<double> jointparams,
            int search_type = 1,
            double division_distance = 300,
            double shift = 0.6,
            int output_type = 4,
            int triangulate = 0

            )
        {
            var watch = new System.Diagnostics.Stopwatch();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Input
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Convert polylines to coordinate array
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            watch.Start();
            list_coord(ref data.polylines, out int[] f, out int f_s, out double[] v, out int v_s);
            list_coord(ref data.face_vectors, out int[] vec_f, out int vec_f_s, out double[] vec_v, out int vec_v_s);
            list_coord(ref data.joints_types, out int[] jointtypes_f, out int jointtypes_f_s, out int[] jointtypes_v, out int jointtypes_v_s);
            list_coord(ref data.three_valence, out int[] threevalence_f, out int threevalence_f_s, out int[] threevalence_v, out int threevalence_v_s);
            list_coord(ref data.adjacency, out int[] adjacency_v, out int adjacency_v_s);
            list_coord(ref jointparams, out double[] jointparams_v, out int jointparams_v_s);
            watch.Stop();

            // RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Run the method
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            watch.Start();
            IntPtr coord_out = IntPtr.Zero;
            int coord_size_out = 0;
            Unsafe.test_get_array(ref coord_out, ref coord_size_out);

            IntPtr groups_f = IntPtr.Zero, out_f = IntPtr.Zero, out_v = IntPtr.Zero;
            int groups_f_s = 0, out_f_s = 0, out_v_s = 0;
            int r = Unsafe.pinvoke_get_connection_zones(
                f, f_s, v, v_s,
                vec_f, vec_f_s, vec_v, vec_v_s,
                jointtypes_f, jointtypes_f_s, jointtypes_v, jointtypes_v_s,
                threevalence_f, threevalence_f_s, threevalence_v, threevalence_v_s,
                adjacency_v, adjacency_v_s,
                jointparams_v, jointparams_v_s,
                ref groups_f, ref groups_f_s, ref out_f, ref out_f_s, ref out_v, ref out_v_s,
                search_type, division_distance, shift, output_type, triangulate
                );

            if (r == 0)
                return;
            watch.Stop();

            //RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Convert arrays to data
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            watch.Start();
            coord_list(ref groups_f, ref out_f, ref out_v, ref groups_f_s, ref out_f_s, ref out_v_s, ref polylines);
            watch.Stop();

            //RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //OUTPUT
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //    std::vector<std::vector<CGAL_Polyline>>& plines,
            //    std::vector<std::vector<int>>& top_face_triangulation,
        }
    }
}
