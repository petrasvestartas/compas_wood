using System;
using System.Collections.Generic;
using System.Linq;
using Grasshopper;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;

namespace rhino_util
{
    public static class GrasshopperUtil
    {
        public static List<T> Flatten<T>(this List<List<T>> nestedList)
        {
            List<T> flattenList = new List<T>();

            for (int i = 0; i < nestedList.Count; i++)
                flattenList.AddRange(nestedList[i]);

            return flattenList;
        }

        //public static List<Tuple<T1, T2>> Pack(List<T> listA, List<T> listB) {
        //    IEnumerable<Tuple<A, B>> pairs = listA.Zip(listB, (a, b) => Tuple.Create(a, b));

        //    return pairs.ToList();
        //}

        public static void AddToDictionary(ref Dictionary<string, List<int>> dict, string k, int v)
        {
            if (dict.ContainsKey(k))
            {
                dict[k].Add(v);
            }
            else
            {
                dict.Add(k, new List<int> { v });
            }
        }

        public static Tuple<List<A>, List<B>> Unpack<A, B>(this List<Tuple<A, B>> list)
        {
            var listA = new List<A>(list.Count);
            var listB = new List<B>(list.Count);

            foreach (var t in list)
            {
                listA.Add(t.Item1);
                listB.Add(t.Item2);
            }

            return Tuple.Create(listA, listB);
        }

        public static Tuple<List<A>, List<B>, List<C>> Unpack<A, B, C>(this List<Tuple<A, B, C>> list)
        {
            var listA = new List<A>(list.Count);
            var listB = new List<B>(list.Count);
            var listC = new List<C>(list.Count);

            foreach (var t in list)
            {
                listA.Add(t.Item1);
                listB.Add(t.Item2);
                listC.Add(t.Item3);
            }

            return Tuple.Create(listA, listB, listC);
        }

        public static Tuple<List<A>, List<B>, List<C>, List<D>> Unpack<A, B, C, D>(this List<Tuple<A, B, C, D>> list)
        {
            var listA = new List<A>(list.Count);
            var listB = new List<B>(list.Count);
            var listC = new List<C>(list.Count);
            var listD = new List<D>(list.Count);

            foreach (var t in list)
            {
                listA.Add(t.Item1);
                listB.Add(t.Item2);
                listC.Add(t.Item3);
                listD.Add(t.Item4);
            }

            return Tuple.Create(listA, listB, listC, listD);
        }

        public static Tuple<List<A>, List<B>, List<C>, List<D>, List<E>> Unpack<A, B, C, D, E>(this List<Tuple<A, B, C, D, E>> list)
        {
            var listA = new List<A>(list.Count);
            var listB = new List<B>(list.Count);
            var listC = new List<C>(list.Count);
            var listD = new List<D>(list.Count);
            var listE = new List<E>(list.Count);

            foreach (var t in list)
            {
                listA.Add(t.Item1);
                listB.Add(t.Item2);
                listC.Add(t.Item3);
                listD.Add(t.Item4);
                listE.Add(t.Item5);
            }

            return Tuple.Create(listA, listB, listC, listD, listE);
        }

        /// <summary>
        /// rotate array CW
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="arr"></param>
        /// <returns></returns>
        public static T[] shiftLeft<T>(this T[] arr)
        {
            T[] demo = new T[arr.Length];

            for (int i = 0; i < arr.Length - 1; i++)
                demo[i] = arr[i + 1];
            demo[demo.Length - 1] = arr[0];

            return demo;
        }

        public static T[] shiftRight<T>(this T[] arr)
        {
            T[] demo = new T[arr.Length];

            for (int i = 1; i < arr.Length; i++)
                demo[i] = arr[i - 1];
            demo[0] = arr[demo.Length - 1];

            return demo;
        }

        public static T[] Shift<T>(this T[] array, int shiftValue)
        {
            var newArray = new T[array.Length];
            shiftValue -= array.Length;
            if (shiftValue < 0) shiftValue *= -1;

            for (var i = 0; i < array.Length; i++)
            {
                var index = (i + shiftValue) % array.Length;
                newArray[i] = array[index];
            }
            return newArray;
        }

        public static List<T>[] PartitionsBySize<T>(List<T> list, int size, bool shift = false)
        {
            if (list == null)
                throw new ArgumentNullException("list");

            if (size < 1)
                throw new ArgumentOutOfRangeException("totalPartitions");

            if (shift)
            {
                int half = (int)(size * 0.5);

                var start = list.GetRange(0, half).ToList();
                list.RemoveRange(0, half);

                int count = (int)Math.Ceiling(list.Count / (double)size);
                List<T>[] partitions = new List<T>[count + 1];
                partitions[0] = start;

                int k = 0;
                for (int i = 0; i < partitions.Length - 1; i++)
                {
                    partitions[i + 1] = new List<T>(size);
                    for (int j = k; j < k + size; j++)
                    {
                        if (j >= list.Count)
                            break;
                        partitions[i + 1].Add(list[j]);
                    }
                    k += size;
                }

                return partitions;
            }
            else
            {
                int count = (int)Math.Ceiling(list.Count / (double)size);
                List<T>[] partitions = new List<T>[count];

                int k = 0;
                for (int i = 0; i < partitions.Length; i++)
                {
                    partitions[i] = new List<T>(size);
                    for (int j = k; j < k + size; j++)
                    {
                        if (j >= list.Count)
                            break;
                        partitions[i].Add(list[j]);
                    }
                    k += size;
                }

                return partitions;
            }
        }

        public static List<List<T>> Split<T>(List<T> source, int size)
        {
            List<List<T>> ret = new List<List<T>>();

            for (int i = 0; i < source.Count; i += size)
                ret.Add(source.GetRange(i, Math.Min(size, source.Count - i)));

            return ret;
        }

        public static T[][] SplitArray<T>(List<T> source, int size)
        {
            var ret = new T[source.Count / size][];

            int counter = 0;
            for (int i = 0; i < source.Count; i += size)
                ret[counter++] = (source.GetRange(i, Math.Min(size, source.Count - i))).ToArray();

            return ret;
        }

        public static List<List<T>> SplitFramed<T>(List<T> source, int size, bool addEnd = false)
        {
            List<List<T>> ret = new List<List<T>>();

            for (int i = 0; i < source.Count; i += size)
            {
                if (i == 0)
                    ret.Add(source.GetRange(i, Math.Min(size, source.Count - i)));
                else
                {
                    ret.Add(source.GetRange(i - 1, Math.Min(size + 1, source.Count - i)));
                }
            }

            if (addEnd)
            {
                if (ret.Count > 1)
                {
                    ret[ret.Count - 1].Add(source[0]);
                }
            }

            return ret;
        }

        public static IEnumerable<IEnumerable<T>> PartitionByN<T>(this ICollection<T> items, int numberOfChunks)
        {
            if (numberOfChunks <= 0 || numberOfChunks > items.Count)
                throw new ArgumentOutOfRangeException("numberOfChunks");

            int sizePerPacket = items.Count / numberOfChunks;
            int extra = items.Count % numberOfChunks;

            for (int i = 0; i < numberOfChunks - extra; i++)
                yield return items.Skip(i * sizePerPacket).Take(sizePerPacket);

            int alreadyReturnedCount = (numberOfChunks - extra) * sizePerPacket;
            int toReturnCount = extra == 0 ? 0 : (items.Count - numberOfChunks) / extra + 1;
            for (int i = 0; i < extra; i++)
                yield return items.Skip(alreadyReturnedCount + i * toReturnCount).Take(toReturnCount);
        }

        private static Random rng = new Random(0);

        public static void Shuffle<T>(this IList<T> list, int seed = 0)
        {
            // if (seed >0 )
            rng = new Random(seed);

            int n = list.Count;
            while (n > 1)
            {
                n--;
                int k = rng.Next(n + 1);
                T value = list[k];
                list[k] = list[n];
                list[n] = value;
            }
        }

        public static Dictionary<int, int> DictFromHash(HashSet<int> h)
        {
            Dictionary<int, int> d = new Dictionary<int, int>();
            for (int i = 0; i < h.Count; i++)
                d.Add(h.ElementAt(i), i);
            return d;
        }

        public static void Debug(object o)
        {
            Rhino.RhinoApp.WriteLine(o.ToString());
        }

        public static DataTree<T> IE<T>(IEnumerable<T> list, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();

            if (iteration == -1)
            {
                int count = 0;
                foreach (var t in list)
                {
                    tree.Add(t, new GH_Path(count));
                    count++;
                }
            }
            else
            {
                tree.AddRange(list, new GH_Path(new int[] { iteration }));
            }

            return tree;
        }

        public static DataTree<T> IE2<T>(IEnumerable<IEnumerable<T>> list, int iteration = 0)
        {
            if (list == null) return new DataTree<T>();
            DataTree<T> tree = new DataTree<T>();
            int i = 0;
            foreach (IEnumerable<T> innerList in list)
            {
                if (innerList == null) continue;
                tree.AddRange(innerList, new GH_Path(new int[] { iteration, i }));
                i++;
            }
            return tree;
        }

        public static GH_Structure<GH_String> IE2_GH_Structure(IEnumerable<IEnumerable<string>> list, int iteration = 0)
        {
            if (list == null) return new GH_Structure<GH_String>();
            var tree = new GH_Structure<GH_String>();
            int i = 0;
            foreach (IEnumerable<string> innerList in list)
            {
                if (innerList == null) continue;
                foreach (var s in innerList)
                {
                    if (s == null) continue;
                    //Rhino.RhinoApp.WriteLine(s);
                    tree.Append(new GH_String(s), new GH_Path(new int[] { i }));//i
                }
                i++;
            }
            return tree;
        }

        public static GH_Structure<GH_Plane> IE2_GH_Structure(IEnumerable<IEnumerable<Plane>> list, int iteration = 0)
        {
            if (list == null) return new GH_Structure<GH_Plane>();
            var tree = new GH_Structure<GH_Plane>();
            int i = 0;
            foreach (var innerList in list)
            {
                if (innerList == null) continue;
                foreach (var s in innerList)
                {
                    //if (s == Plane.Unset) continue;
                    if (s.ZAxis.X == 0 && s.ZAxis.Y == 0 && s.ZAxis.Z == 0) continue;
                    //Rhino.RhinoApp.WriteLine(s);
                    tree.Append(new GH_Plane(s), new GH_Path(new int[] { i }));//i
                }
                i++;
            }
            return tree;
        }

        public static GH_Structure<GH_Curve> IE2_GH_Structure(IEnumerable<IEnumerable<Polyline>> list, int iteration = 0)
        {
            if (list == null) return new GH_Structure<GH_Curve>();
            var tree = new GH_Structure<GH_Curve>();
            int i = 0;
            foreach (var innerList in list)
            {
                if (innerList == null) continue;
                foreach (var s in innerList)
                {
                    if (s == null) continue;
                    //Rhino.RhinoApp.WriteLine(s);
                    tree.Append(new GH_Curve(s.ToNurbsCurve()), new GH_Path(new int[] { i }));//i
                }
                i++;
            }
            return tree;
        }

        public static DataTree<T> IE3<T>(IEnumerable<IEnumerable<IEnumerable<T>>> list, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            if (list == null) return tree;
            int i = 0;
            foreach (IEnumerable<IEnumerable<T>> innerList in list)
            {
                if (innerList == null) continue;
                int j = 0;
                foreach (IEnumerable<T> innerList2 in innerList)
                {
                    if (innerList2 == null) continue;
                    foreach (var o in innerList2)
                    {
                        if (o == null) continue;
                        tree.Add(o, new GH_Path(new int[] { iteration, i, j }));
                    }
                    j++;
                }

                i++;
            }
            return tree;
        }

        public static DataTree<T> IE4<T>(IEnumerable<IEnumerable<IEnumerable<IEnumerable<T>>>> list, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            int i = 0;
            foreach (IEnumerable<IEnumerable<IEnumerable<T>>> innerList in list)
            {
                int j = 0;
                foreach (IEnumerable<IEnumerable<T>> innerList2 in innerList)
                {
                    int k = 0;
                    foreach (IEnumerable<T> innerList3 in innerList2)
                    {
                        tree.AddRange(innerList3, new GH_Path(new int[] { iteration, i, j, k }));
                        k++;
                    }

                    j++;
                }

                i++;
            }
            return tree;
        }

        public static DataTree<T> ListOfListsToTree<T>(List<List<T>> list, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            int i = 0;
            foreach (List<T> innerList in list)
            {
                if (innerList != null)
                    tree.AddRange(innerList, new GH_Path(new int[] { iteration, i }));
                i++;
            }
            return tree;
        }

        public static DataTree<T> ArrayOfListsToTree<T>(List<T>[] list, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            int i = 0;
            foreach (List<T> innerList in list)
            {
                if (innerList != null)
                    tree.AddRange(innerList, new GH_Path(new int[] { iteration, i }));
                i++;
            }
            return tree;
        }

        public static DataTree<T> JaggedArraysToTree<T>(T[][] arrays, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            int i = 0;
            foreach (var a in arrays)
            {
                if (a != null)
                    tree.AddRange(a, new GH_Path(new int[] { iteration, i }));
                i++;
            }
            return tree;
        }

        public static DataTree<T> TripleArraysToTree<T>(T[][][] arrays, int iteration = 0)
        {
            DataTree<T> tree = new DataTree<T>();
            int i = 0;

            foreach (var a in arrays)
            {
                int j = 0;
                foreach (var b in a)
                {
                    if (b != null)
                        tree.AddRange(b, new GH_Path(new[] { iteration, i, j }));
                    j++;
                }
                i++;
            }
            return tree;
        }
    }
}