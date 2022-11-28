using System;
using System.Runtime.InteropServices;

namespace joinery_solver_net
{
    public class Unsafe
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Basic Method and Examples for PInvoke
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //private string dllName = System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "joinery_solver_pinvoke.dll" : "joinery_solver_pinvoke.dylib";
        public const string dllName = "joinery_solver_pinvoke.dll";

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int test_get_square(int n);

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void release_double(IntPtr arr, bool is_array); // release input coordinates

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void release_int(IntPtr arr, bool is_array); //release output indices array

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void test_set_array([MarshalAs(UnmanagedType.LPArray)] double[] coord, int coord_size);

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void test_get_array(ref IntPtr coord_out, ref int coord_size_out);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Implementations
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int pinvoke_get_connection_zones(
            [MarshalAs(UnmanagedType.LPArray)] int[] f, int f_s, [MarshalAs(UnmanagedType.LPArray)] double[] v, int v_s,
            [MarshalAs(UnmanagedType.LPArray)] int[] vec_f, int vec_f_s, [MarshalAs(UnmanagedType.LPArray)] double[] vec_v, int vec_v_s,
            [MarshalAs(UnmanagedType.LPArray)] int[] joint_types_f, int joint_types_f_s, [MarshalAs(UnmanagedType.LPArray)] int[] joint_types_v, int joint_types_v_s,
            [MarshalAs(UnmanagedType.LPArray)] int[] threevalence_f, int threevalence_f_s, [MarshalAs(UnmanagedType.LPArray)] int[] threevalence_v, int threevalence_v_s,
            [MarshalAs(UnmanagedType.LPArray)] int[] adjacency_v, int adjacency_v_s,
            [MarshalAs(UnmanagedType.LPArray)] double[] jointparams_v, int jointparams_v_s,
            [MarshalAs(UnmanagedType.LPArray)] double[] scale_v, int scale_v_s,
            string file_and_folder_of_joint_library_xml,
            ref IntPtr groups_f, ref int groups_f_s, ref IntPtr out_f, ref int out_f_s, ref IntPtr out_v, ref int out_v_s,
            int search_type = 1, int output_type = 4, int triangulate = 0
            );

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int pinvoke_find_closest_plateside_to_line(
            //input - polylines flat list and a flat list of lines
            [MarshalAs(UnmanagedType.LPArray)] int[] f, int f_s, [MarshalAs(UnmanagedType.LPArray)] double[] v, int v_s,
            [MarshalAs(UnmanagedType.LPArray)] double[] lines_v, int lines_v_s,
            //output - jagged array of vectors
            ref IntPtr out_f, ref int out_f_s, ref IntPtr out_v, ref int out_v_s
        );

        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int pinvoke_find_closest_plateside_to_indexedpoint(
        //input - polylines flat list and a flat list of lines
        [MarshalAs(UnmanagedType.LPArray)] int[] f, int f_s, [MarshalAs(UnmanagedType.LPArray)] double[] v, int v_s,
        [MarshalAs(UnmanagedType.LPArray)] double[] pts_v, int pts_v_s,
        [MarshalAs(UnmanagedType.LPArray)] int[] ids_v, int ids_v_s,
        //output - jagged array of vectors
        ref IntPtr out_f, ref int out_f_s, ref IntPtr out_v, ref int out_v_s
    );
    }
}
