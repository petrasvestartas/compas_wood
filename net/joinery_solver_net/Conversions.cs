using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino;
using Rhino.Geometry;

namespace joinery_solver_net
{
    internal static class Conversions
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Rhino Helpers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        internal static void list_coord(ref List<Rhino.Geometry.Polyline> polylines, out int[] f, out int f_s, out double[] v, out int v_s)
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

        internal static void list_coord(ref Polyline[][] polylines, out int[] f, out int f_s, out double[] v, out int v_s)
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

        internal static void list_coord(ref Vector3d[][] vectors, out int[] f, out int f_s, out double[] v, out int v_s)
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

        internal static void list_coord(ref List<List<Vector3d>> vectors, out int[] f, out int f_s, out double[] v, out int v_s)
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

        internal static void list_coord(ref int[][] numbers, out int[] f, out int f_s, out int[] v, out int v_s)
        {
            if (numbers == null)
            {
                f = null;
                v = null;
                f_s = 0;
                v_s = 0;
            }
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

        internal static void list_coord(ref List<List<int>> numbers, out int[] f, out int f_s, out int[] v, out int v_s)
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

        internal static void list_coord(ref List<int[]> numbers, out int[] f, out int f_s, out int[] v, out int v_s)
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

        internal static void list_coord(ref List<List<int>> numbers, out int[] v, out int v_s)
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

        internal static void list_coord(ref List<int[]> numbers, out int[] v, out int v_s)
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

        internal static void list_coord(ref List<double> numbers, out double[] f, out int f_s)
        {
            f_s = numbers.Count;
            f = System.Linq.Enumerable.ToArray(numbers);
        }

        internal static void list_coord(ref List<int> numbers, out int[] f, out int f_s)
        {
            f_s = numbers.Count;
            f = System.Linq.Enumerable.ToArray(numbers);
        }

        internal static void list_coord(ref List<Point3d> points, out double[] f, out int f_s)
        {
            f_s = points.Count * 3;
            f = new double[f_s];

            for (int i = 0; i < points.Count; i++)
            {
                f[i * 3 + 0] = points[i].X;
                f[i * 3 + 1] = points[i].Y;
                f[i * 3 + 2] = points[i].Z;
            }
        }

        internal static void list_coord(ref List<Line> lines, out double[] f, out int f_s)
        {
            f_s = lines.Count * 6;
            f = new double[f_s];

            for (int i = 0; i < lines.Count; i++)
            {
                f[i * 6 + 0] = lines[i].FromX;
                f[i * 6 + 1] = lines[i].FromY;
                f[i * 6 + 2] = lines[i].FromZ;
                f[i * 6 + 3] = lines[i].ToX;
                f[i * 6 + 4] = lines[i].ToY;
                f[i * 6 + 5] = lines[i].ToZ;
            }
        }

        internal static void coord_list(ref IntPtr groups_f, ref IntPtr out_f, ref IntPtr out_v, ref int groups_f_s, ref int out_f_s, ref int out_v_s, ref Polyline[][] polylines)
        {
            //transfer data from c++
            //Console.WriteLine(groups_f_s.ToString());
            int[] sharp_groups_f = new int[groups_f_s];
            int[] sharp_out_f = new int[out_f_s];
            double[] sharp_out_v = new double[out_v_s];
            System.Runtime.InteropServices.Marshal.Copy(groups_f, sharp_groups_f, 0, sharp_groups_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_f, sharp_out_f, 0, sharp_out_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_v, sharp_out_v, 0, sharp_out_v.Length);
            Unsafe.release_int(groups_f, true);
            Unsafe.release_int(out_f, true);
            Unsafe.release_double(out_v, true);

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
        }

        internal static void coord_list(ref IntPtr out_f, ref IntPtr out_v, ref int out_f_s, ref int out_v_s, ref Vector3d[][] vectors)
        {
            //Console.WriteLine("Hi");
            //transfer data from c++
            int[] sharp_out_f = new int[out_f_s];
            double[] sharp_out_v = new double[out_v_s];
            //Console.WriteLine(out_f_s.ToString());
            //Console.WriteLine(out_v_s.ToString());
            System.Runtime.InteropServices.Marshal.Copy(out_f, sharp_out_f, 0, sharp_out_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_v, sharp_out_v, 0, sharp_out_v.Length);
            Unsafe.release_int(out_f, true);
            Unsafe.release_double(out_v, true);

            if (out_f_s == 0 || out_v_s == 0) return;

            //create c# objects
            vectors = new Vector3d[out_f_s][];

            for (int i = 0; i < out_f_s; i++)
            {
                vectors[i] = new Vector3d[sharp_out_f[i]];
                if (sharp_out_f[i] == 0) return;
            }

            //Console.WriteLine();
            //fill array with points
            int a = 0;
            int b = 0;

            //Console.WriteLine("Hi");
            //Console.WriteLine(out_f_s.ToString());

            for (int i = 0; i < sharp_out_v.Length; i += 3)
            {
                //Console.WriteLine(sharp_out_v[i + 0].ToString());
                //Console.WriteLine(sharp_out_v[i + 1].ToString());
                //Console.WriteLine(sharp_out_v[i + 2].ToString());
                vectors[a][b] = new Vector3d(sharp_out_v[i + 0], sharp_out_v[i + 1], sharp_out_v[i + 2]);

                if (b == (sharp_out_f[a] - 1))
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
            }
        }

        internal static void coord_list(ref IntPtr out_f, ref IntPtr out_v, ref int out_f_s, ref int out_v_s, ref int[][] ids)
        {
            //transfer data from c++
            int[] sharp_out_f = new int[out_f_s];
            int[] sharp_out_v = new int[out_v_s];
            System.Runtime.InteropServices.Marshal.Copy(out_f, sharp_out_f, 0, sharp_out_f.Length);
            System.Runtime.InteropServices.Marshal.Copy(out_v, sharp_out_v, 0, sharp_out_v.Length);
            Unsafe.release_int(out_f, true);
            Unsafe.release_int(out_v, true);

            //create c# objects
            ids = new int[out_f_s][];

            for (int i = 0; i < out_f_s; i++)
            {
                ids[i] = new int[sharp_out_f[i]];
                if (sharp_out_f[i] == 0) return;
            }

            //Console.WriteLine();
            //fill array with points
            int a = 0;
            int b = 0;

            for (int i = 0; i < sharp_out_v.Length; i++)
            {
                //Console.WriteLine(sharp_out_v[i + 0].ToString());
                //Console.WriteLine(sharp_out_v[i + 1].ToString());
                //Console.WriteLine(sharp_out_v[i + 2].ToString());
                ids[a][b] = sharp_out_v[i + 0];

                if (b == (sharp_out_f[a] - 1))
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
            }
        }
    }
}
