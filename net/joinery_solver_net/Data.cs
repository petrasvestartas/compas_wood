using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace joinery_solver_net {

    public struct Data {
        public List<Polyline> polylines;
        public Vector3d[][] face_vectors;
        public int[][] joints_types;
        public List<int[]> three_valence;
        public List<int[]> adjacency;
    }
}
