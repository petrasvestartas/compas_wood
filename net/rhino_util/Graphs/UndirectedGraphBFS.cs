using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace rhino_util.graphs {

    public static class UndirectedGraphBfsRhino {

        public static bool[][] CheckerEdges(this Mesh M) {
            //Mark opposite Edge
            string start = "0";
            int n = M.Ngons.Count;
            for (int i = 0; i < n; i++) {
                if (M.Ngons[i].BoundaryVertexCount % 2 == 0) {
                    start = i.ToString();
                    break;
                }
            }

            Tuple<List<List<int>>, List<List<int[]>>> bfs = rhino_util.graphs.UndirectedGraphBfsRhino.MeshBFS(M, start);
            //take starting point as regular polygon

            var V = bfs.Item1[0];
            var E = bfs.Item2[0];

            //Dictionary<int, bool> mesFlags = new Dictionary<int, bool>();
            bool[][] ngonEdgeFlags = new bool[n][];

            var irregularNGons = new List<int>();

            for (int i = 0; i < n; i++) {
                int f0 = V[i];
                int f1 = (i != 0) ? E[i - 1][0] : -1;
                int[] mes = M._fe(f0);
                int[] mesNei = (i != 0) ? M._fe(f1) : new int[0];

                if (mes.Length % 2 == 1) {
                    Rhino.RhinoApp.WriteLine("irregular" + mes.Length.ToString() + " " + mesNei.Length.ToString());
                    ngonEdgeFlags[f0] = new bool[mes.Length];
                    irregularNGons.Add(f0);
                    continue;
                }

                if (i == 0) {
                    ngonEdgeFlags[f0] = new bool[mes.Length];

                    for (int j = 0; j < mes.Length; j++) {
                        ngonEdgeFlags[f0][j] = j % 2 == 0;

                        if (M.TopologyEdges.GetConnectedFaces(mes[MathUtil.Wrap(j, mes.Length)]).Length == 1) {
                            ngonEdgeFlags[f0][MathUtil.Wrap(j, mes.Length)] = false;
                        }
                    }

                    if (ngonEdgeFlags[f0][0] == ngonEdgeFlags[f0][ngonEdgeFlags[f0].Length - 1]) {
                        ngonEdgeFlags[f0][ngonEdgeFlags[f0].Length - 1] = false;
                    }
                } else {
                    //Find common edge
                    int me = M._CommonEdge(f0, f1);

                    //Find local edge id
                    int feNei = Array.IndexOf(mesNei, me);
                    int fe = Array.IndexOf(mes, me);

                    //Check its initial flag of neighbour face
                    bool startFlag = ngonEdgeFlags[f1][feNei];

                    ngonEdgeFlags[f0] = new bool[mes.Length];

                    for (int j = 0; j < mes.Length; j++) {
                        startFlag = !startFlag;
                        ngonEdgeFlags[f0][MathUtil.Wrap(j + fe, mes.Length)] = startFlag;

                        if (M.TopologyEdges.GetConnectedFaces(mes[MathUtil.Wrap(j + fe, mes.Length)]).Length == 1) {
                            ngonEdgeFlags[f0][MathUtil.Wrap(j + fe, mes.Length)] = false;
                        }
                    }
                    /*
                    if (ngonEdgeFlags[f0][MathUtil.Wrap(0 + fe, mes.Length)] == ngonEdgeFlags[f0][MathUtil.Wrap(-1 + fe, mes.Length)]) {
                        ngonEdgeFlags[f0][MathUtil.Wrap(-1 + fe, mes.Length)] = !ngonEdgeFlags[f0][MathUtil.Wrap(0 + fe, mes.Length)];
                    }
                    */

                    //if (ngonEdgeFlags[f0][0] == ngonEdgeFlags[f0][ngonEdgeFlags[f0].Length - 1]) {
                    //    ngonEdgeFlags[f0][ngonEdgeFlags[f0].Length - 1] = false;
                    //}

                    ////Iterate from that edge
                    //for (int j = 0; j < mes.Length; j++) {
                    //    int nextMeshEdge = mes[NGonsCore.MathUtil.Wrap(j + fe + 1, mes.Length)];

                    //    //if (M.TopologyEdges.GetConnectedFaces(nextMeshEdge).Length == 1) continue;
                    //    startFlag = !startFlag;

                    //    //Rhino.RhinoApp.WriteLine(i.ToString() + " "  + j.ToString() + "MeshEdge" + nextMeshEdge);

                    //    if (!mesFlags.ContainsKey(nextMeshEdge))
                    //        mesFlags.Add(nextMeshEdge, startFlag);
                    //    else
                    //        mesFlags[nextMeshEdge] = startFlag;
                    //}//for j
                }//if not first
            }// for j

            /*
            for (int i = 0; i < irregularNGons.Count; i++) {
                int f0 = irregularNGons[i];
                int[] mes = M._fe(f0);

                for (int j = 0; j < mes.Length; j++) {
                    int[] fe = M._OppositeFE(f0, j);
                    int fnei = fe[0];
                    int enei = fe[1];
                    if (fe[0]!=-1 && M._fe(fe[0]).Length % 2 != 1) {//
                        ngonEdgeFlags[f0][j] = !ngonEdgeFlags[fnei][enei];
                    }
                }
            }
            */

            return ngonEdgeFlags;
        }

        public static bool[] Checker(this Mesh mesh) {
            Tuple<List<List<int>>, List<List<int[]>>> bfs = rhino_util.graphs.UndirectedGraphBfsRhino.MeshBFS(mesh, "0");

            bool[] checker = System.Linq.Enumerable.ToArray(System.Linq.Enumerable.Repeat(false, mesh.Ngons.Count));
            checker[0] = false;

            for (int i = 0; i < bfs.Item2[0].Count; i++) {
                int v0 = bfs.Item2[0][i][0];
                int v1 = bfs.Item2[0][i][1];

                checker[v1] = !checker[v0];
            }
            return checker;
        }

        public static Tuple<List<List<int>>, List<List<int[]>>> BrepBFS(Brep brep, string startIndex = "0") {
            UndirectedGraphBfs g = new UndirectedGraphBfs(brep.Faces.Count);

            for (int i = 0; i < brep.Faces.Count; i++)
                g.InsertVertex(i.ToString());

            int[][] ef = brep.GetEdgeFaces();

            for (int i = 0; i < ef.Length; i++) {
                int[] f = ef[i];
                if (f.Length == 2) {
                    g.InsertEdge(f[0].ToString(), f[1].ToString());
                    //Rhino.RhinoApp.WriteLine("Hi");
                }
            }

            //Output
            List<List<int>> gNodes = g.BfsTraversal_All(startIndex);
            List<List<int[]>> gEdges = g.EdgesAll;

            return new Tuple<List<List<int>>, List<List<int[]>>>(gNodes, gEdges);
        }

        public static Tuple<List<List<int>>, List<List<int[]>>> MeshBFS(Mesh mesh, string startIndex = "0") {
            UndirectedGraphBfs g = new UndirectedGraphBfs(mesh.Ngons.Count);

            if (mesh.Ngons.Count > 0) {
                for (int i = 0; i < mesh.Ngons.Count; i++)
                    g.InsertVertex(i.ToString());

                List<int>[] adj = mesh.GetNGonFaceAdjacency();

                for (int i = 0; i < adj.Length; i++) {
                    foreach (int j in adj[i])
                        g.InsertEdge(i.ToString(), j.ToString());
                }
            } else {
                for (int i = 0; i < mesh.Faces.Count; i++)
                    g.InsertVertex(i.ToString());

                for (int i = 0; i < mesh.Faces.Count; i++) {
                    int[] adj = mesh.Faces.AdjacentFaces(i);
                    foreach (int j in adj)
                        g.InsertEdge(i.ToString(), j.ToString());
                }
            }

            //Output
            List<List<int>> gNodes = g.BfsTraversal_All(startIndex);
            List<List<int[]>> gEdges = g.EdgesAll;

            return new Tuple<List<List<int>>, List<List<int[]>>>(gNodes, gEdges);
        }
    }

    public class UndirectedGraphBfs : UndirectedGraph {
        private List<int[]> _edges;
        public List<List<int[]>> EdgesAll;

        private const int Initial = 0;
        private const int Waiting = 1;
        private const int Visited = 2;

        private const int Nil = 0;
        private const int Infinity = 99999;

        public UndirectedGraphBfs(int maxVertices) : base(maxVertices) {
        }

        public List<int> BfsTraversal(String s) {
            for (int v = 0; v < N; v++)
                VertexList[v].state = Initial; //All vertices at start are in the same state

            return Bfs(GetIndex(s));
        }

        private List<int> Bfs(int v) {
            List<int> sequence = new List<int>(); //Get vertices list
            _edges = new List<int[]>(); //Get edges list

            Queue<int> qu = new Queue<int>();
            qu.Enqueue(v); //Add element to queue and change its state
            VertexList[v].state = Waiting;
            VertexList[v].distance = 0;//shortest path
            VertexList[v].predecessor = Nil;//shortest path

            while (qu.Count != 0) //Count until no elements i queue
            {
                v = qu.Dequeue();
                sequence.Add(v);
                VertexList[v].state = Visited;

                for (int i = 0; i < N; i++) {
                    if (IsAdjacent(v, i) && VertexList[i].state == Initial) {
                        qu.Enqueue(i);
                        _edges.Add(new[] { v, i });
                        VertexList[i].state = Waiting;
                        VertexList[i].predecessor = v;
                        VertexList[i].distance = VertexList[v].distance + 1;
                    }
                }
            }
            return sequence;
        }

        public List<List<int>> BfsTraversal_All(String s) {
            List<List<int>> sequences = new List<List<int>>();
            EdgesAll = new List<List<int[]>>(); //Get edges list

            int v;
            for (v = 0; v < N; v++)
                VertexList[v].state = Initial;

            sequences.Add(Bfs(GetIndex(s)));
            EdgesAll.Add(_edges);

            for (v = 0; v < N; v++)
                if (VertexList[v].state == Initial) {
                    sequences.Add(Bfs(v));
                    EdgesAll.Add(_edges);
                }

            return sequences;
        }

        public List<List<int>> FindShortestPath(String s) {
            List<List<int>> paths = new List<List<int>>();

            for (int v = 0; v < N; v++) {
                VertexList[v].state = Initial;
                VertexList[v].predecessor = Nil;
                VertexList[v].distance = Infinity;
            }

            int index = GetIndex(s);
            Bfs(index);

            for (int v = 0; v < N; v++) {
                if (VertexList[v].distance == Infinity)
                    Rhino.RhinoApp.WriteLine("No path from vertex " + s + " to vertex " + VertexList[v].name);
                else {
                    //Rhino.RhinoApp.WriteLine("Shortest distance from " + s + " to vertex " + VertexList[v].name + " is " + VertexList[v].distance);

                    //Store the full path in an array path
                    List<int> outPath = new List<int>();

                    int[] path = new int[N];
                    int count = 0;
                    int x, y = v;

                    while (y != Nil) {
                        count++;
                        path[count] = y;
                        x = VertexList[y].predecessor;
                        y = x;
                    }

                    //Console.Write("Shortest Path is : ");
                    int i;
                    outPath.Add(index);
                    for (i = count; i > 1; i--)
                        //Console.Write(VertexList[path[i]].name + "->");
                        outPath.Add(path[i]);
                    //Console.WriteLine(VertexList[path[i]].name);
                    outPath.Add(path[i]);

                    paths.Add(outPath);
                }
            }
            return paths;
        }

        public bool IsConnected() {
            for (int v = 0; v < N; v++)
                VertexList[v].state = Initial;

            int cN = 1;
            BfsC(0, cN);

            for (int v = 0; v < N; v++)
                if (VertexList[v].state == Initial) {
                    cN++;
                    BfsC(v, cN);
                }

            if (cN == 1) {
                //Rhino.RhinoApp.WriteLine("Graph is connected\n");
                return true;
            } else {
                //Rhino.RhinoApp.WriteLine("Graph is not connected, is has " + cN + " connected components ");
                //for (int v = 0; v < n; v++)
                //Rhino.RhinoApp.WriteLine(VertexList[v].name + " " + VertexList[v].componentNumber);
                return false;
            }
        }

        private void BfsC(int v, int cN) {
            Queue<int> qu = new Queue<int>();

            qu.Enqueue(v);
            VertexList[v].state = Waiting;

            while (qu.Count != 0) {
                v = qu.Dequeue();
                VertexList[v].state = Visited;
                VertexList[v].componentNumber = cN;

                for (int i = 0; i < N; i++) {
                    if (IsAdjacent(v, i) && VertexList[i].state == Initial) {
                        qu.Enqueue(i);
                        VertexList[i].state = Waiting;
                    }
                }
            }
        }
    }
}