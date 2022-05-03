using System;
using System.Collections.Generic;

namespace rhino_util.graphs {

    public class UndirectedGraph {
        public readonly int MaxVertices;

        protected int N;
        protected int E;
        public bool[,] Adj;
        protected Vertex[] VertexList;
        protected object Attributes;
        public List<Edge> EdgesList;

        //Additional Geometric Properties
        public Dictionary<string, int> EdgesDict = new Dictionary<string, int>();

        public List<Rhino.Geometry.Point3d> points = new List<Rhino.Geometry.Point3d>();
        public List<Rhino.Geometry.Plane> planes = new List<Rhino.Geometry.Plane>();

        public UndirectedGraph(int maxVertices) {
            MaxVertices = maxVertices;
            Adj = new bool[maxVertices, maxVertices];
            VertexList = new Vertex[maxVertices];
            EdgesList = new List<Edge>();
        }

        public int Vertices() {
            return N;
        }

        public int Edges() {
            return E;
        }

        public List<String> GetVertices() {
            List<String> v = new List<string>();

            foreach (Vertex n in VertexList)
                v.Add(n.name);

            return v;
        }

        public String GetVertex(int i) {
            return VertexList[i].name;
        }

        public void Display() {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++)

                    if (Adj[i, j])
                        Rhino.RhinoApp.Write("1 ");
                    else
                        Rhino.RhinoApp.Write("0 ");
                Rhino.RhinoApp.WriteLine();
            }
        }

        public void InsertVertex(String name) {
            VertexList[N++] = new Vertex(name);
        }

        protected int GetIndex(String s) {
            for (int i = 0; i < N; i++)
                if (s.Equals(VertexList[i].name))
                    return i;
            throw new System.InvalidOperationException("Invalid Vertex");
        }

        public int GetEdge(String s1, String s2, out int flip) {
            //string e0 = s1 + s2;
            //string e1 = s2 + s1;

            int u = GetIndex(s1);
            int v = GetIndex(s2);
            string e0 = MathUtil.Key(u, v).ToString();
            string e1 = MathUtil.Key(v, u).ToString();

            if (EdgesDict.ContainsKey(e0)) {
                flip = 0;
                return EdgesDict[e0];
            } else if (EdgesDict.ContainsKey(e1)) {
                flip = 1;
                return EdgesDict[e1];
            } else
                flip = -1;
            return -1;
        }

        //Returns true if edge(s1,s2) exists
        public bool EdgeExists(String s1, String s2) {
            return IsAdjacent(GetIndex(s1), GetIndex(s2));
        }

        protected bool IsAdjacent(int u, int v) {
            return Adj[u, v];
        }

        //Insert an edge(s1,s2)
        public void InsertEdge(String s1, String s2) {
            int u = GetIndex(s1);
            int v = GetIndex(s2);

            if (Adj[u, v]) {
                // Rhino.RhinoApp.Write("Edge already present");
            } else {
                Edge edge = new Edge(u, v, 1);

                EdgesDict.Add(MathUtil.Key(u, v).ToString(), EdgesList.Count);

                EdgesList.Add(edge);

                if (points.Count != 0) {
                    edge.ln = new Rhino.Geometry.Line(points[u], points[v]);
                }

                Adj[u, v] = true;
                Adj[v, u] = true;
                E++;
            }
        }

        //Delete the edge (s1,s2)
        public void DeleteEdge(String s1, String s2) {
            int u = GetIndex(s1);
            int v = GetIndex(s2);

            if (Adj[u, v] == false) {
                // Rhino.RhinoApp.WriteLine("Edge not present in the graph");
            } else {
                Adj[u, v] = false;
                Adj[v, u] = false;
                E--;
            }
        }

        //Returns number of edges going out from a vertex
        public int Degree(String s) {
            int u = GetIndex(s);
            int deg = 0;
            for (int v = 0; v < N; v++) {
                if (Adj[u, v])
                    deg++;
            }
            return deg;
        }

        public List<int> GetNeighbours(String s) {
            List<int> neighbours = new List<int>();
            int u = GetIndex(s);

            for (int v = 0; v < N; v++)
                if (Adj[u, v])
                    neighbours.Add(v);

            return neighbours;
        }

        public List<Edge> GetEdges() {
            return EdgesList;
        }

        public void SetAttribute(object data) {
            Attributes = data;
        }

        public object GetAttribute() {
            return Attributes;
        }

        public List<Rhino.Geometry.Line> GetLines() {
            List<Rhino.Geometry.Line> lines = new List<Rhino.Geometry.Line>();
            List<Edge> edges = GetEdges();

            for (int i = 0; i < edges.Count; i++) {
                lines.Add(edges[i].ln);
            }

            return lines;
        }

        public Grasshopper.DataTree<int> GetAdj() {
            Grasshopper.DataTree<int> dataTreeA = new Grasshopper.DataTree<int>();

            List<String> vertices = GetVertices();
            for (int i = 0; i < vertices.Count; i++) {
                //Rhino.RhinoApp.WriteLine(vertices[i]);
                dataTreeA.AddRange(GetNeighbours(vertices[i]), new Grasshopper.Kernel.Data.GH_Path(i));
            }

            return dataTreeA;
        }
    }
}