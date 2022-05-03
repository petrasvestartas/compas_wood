using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util.graphs {

    public class Vertex {
        public string name;
        public int state; // BFS / DFS
        public int predecessor; // BFS / DFS
        public int distance; // BFS shortest path
        public int discoveryTime; //DFS
        public int finishingTime; //DFS
        public int pathLength; //Dijkstra
        public int length;//prisms
        public int father; //kruskal

        public int componentNumber;//Only for Undirected graph for checking disconnected graphs

        public Vertex(string name) {
            this.name = name;
        }
    }
}