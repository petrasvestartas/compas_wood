using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util.graphs {

    public class Edge {
        public int u;
        public int v;
        public int wt;

        public Rhino.Geometry.Line lnOriginal;
        public Rhino.Geometry.Line ln;

        public Edge(int u, int v, int wt) {
            this.u = u;
            this.v = v;
            this.wt = wt;
        }
    }
}