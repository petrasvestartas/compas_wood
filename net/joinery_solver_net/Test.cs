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
            Conversions.list_coord(ref data.polylines, out int[] f, out int f_s, out double[] v, out int v_s);
            Conversions.list_coord(ref data.face_vectors, out int[] vec_f, out int vec_f_s, out double[] vec_v, out int vec_v_s);
            Conversions.list_coord(ref data.joints_types, out int[] jointtypes_f, out int jointtypes_f_s, out int[] jointtypes_v, out int jointtypes_v_s);
            Conversions.list_coord(ref data.three_valence, out int[] threevalence_f, out int threevalence_f_s, out int[] threevalence_v, out int threevalence_v_s);
            Conversions.list_coord(ref data.adjacency, out int[] adjacency_v, out int adjacency_v_s);
            Conversions.list_coord(ref jointparams, out double[] jointparams_v, out int jointparams_v_s);
            watch.Stop();

            // RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Run the method
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            watch.Start();
            //IntPtr coord_out = IntPtr.Zero;
            //int coord_size_out = 0;
            //Unsafe.test_get_array(ref coord_out, ref coord_size_out);

            IntPtr groups_f = IntPtr.Zero, out_f = IntPtr.Zero, out_v = IntPtr.Zero;
            int groups_f_s = 0, out_f_s = 0, out_v_s = 0;
            int r = Unsafe.pinvoke_get_connection_zones(
                f, f_s, v, v_s,
                vec_f, vec_f_s, vec_v, vec_v_s,
                jointtypes_f, jointtypes_f_s, jointtypes_v, jointtypes_v_s,
                threevalence_f, threevalence_f_s, threevalence_v, threevalence_v_s,
                adjacency_v, adjacency_v_s,
                jointparams_v, jointparams_v_s,
                 System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location).ToString(),
                ref groups_f, ref groups_f_s, ref out_f, ref out_f_s, ref out_v, ref out_v_s,
                search_type, division_distance, shift, output_type, triangulate
                );

            //RhinoApp.WriteLine(System.IO.Directory.GetCurrentDirectory().ToString());

            if (r == 0)
                return;
            watch.Stop();

            //RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Convert arrays to data
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            watch.Start();
            Conversions.coord_list(ref groups_f, ref out_f, ref out_v, ref groups_f_s, ref out_f_s, ref out_v_s, ref polylines);
            watch.Stop();

            //RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //OUTPUT
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //    std::vector<std::vector<CGAL_Polyline>>& plines,
            //    std::vector<std::vector<int>>& top_face_triangulation,
        }

        public static void pinvoke_find_closest_plateside_to_line(
                //input
                Polyline[][] polylines,
                List<Line> insertion_lines,
                //output
                ref Vector3d[][] insertion_vectors

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
            Conversions.list_coord(ref polylines, out int[] f, out int f_s, out double[] v, out int v_s);
            Conversions.list_coord(ref insertion_lines, out double[] lines_v, out int lines_v_s);

            //foreach (var coord in lines_v)
            //    Console.WriteLine(coord);

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

            IntPtr out_f = IntPtr.Zero, out_v = IntPtr.Zero;
            int out_f_s = 0, out_v_s = 0;
            int r = Unsafe.pinvoke_find_closest_plateside_to_line(
                //input
                f, f_s, v, v_s,
                lines_v, lines_v_s,
                 //outout
                 ref out_f, ref out_f_s, ref out_v, ref out_v_s
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
            Conversions.coord_list(ref out_f, ref out_v, ref out_f_s, ref out_v_s, ref insertion_vectors);
            watch.Stop();

            //RhinoApp.WriteLine(watch.ElapsedMilliseconds.ToString());
            watch.Reset();
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //OUTPUT
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //    std::vector<std::vector<CGAL_Polyline>>& plines,
            //    std::vector<std::vector<int>>& top_face_triangulation,
        }

        public static void pinvoke_find_closest_plateside_to_indexedpoint(
        //input
        Polyline[][] polylines,
        List<Point3d> joint_points,
         List<int> joint_points_ids,
        //output
        ref int[][] joints_ids

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
            Conversions.list_coord(ref polylines, out int[] f, out int f_s, out double[] v, out int v_s);
            Conversions.list_coord(ref joint_points, out double[] pts_v, out int pts_v_s);
            Conversions.list_coord(ref joint_points_ids, out int[] ids_v, out int ids_v_s);

            //foreach (var coord in lines_v)
            //    Console.WriteLine(coord);

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

            IntPtr out_f = IntPtr.Zero, out_v = IntPtr.Zero;
            int out_f_s = 0, out_v_s = 0;
            int r = Unsafe.pinvoke_find_closest_plateside_to_indexedpoint(
                //input
                f, f_s, v, v_s,
                pts_v, pts_v_s,
                ids_v, ids_v_s,
                 //output
                 ref out_f, ref out_f_s, ref out_v, ref out_v_s
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
            Conversions.coord_list(ref out_f, ref out_v, ref out_f_s, ref out_v_s, ref joints_ids);
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
