using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class input_set_component : GH_Component
    {
        public input_set_component()
          : base("input_set", "input_set",
              "input_set",
              "compas_wood", "data")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("plines", "plines", "plines", GH_ParamAccess.tree);
            pManager.AddVectorParameter("insertion_vectors", "insertion_vectors", "insertion_vectors", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("joints_per_face", "joints_per_face", "joints_per_face", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("three_valence", "three_valence", "three_valence", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("adjacency", "adjacency", "adjacency", GH_ParamAccess.tree);
            for (int i = 1; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            try
            {
                var plines_dt = new GH_Structure<GH_Curve>();
                var insertion_vectors_dt = new GH_Structure<GH_Vector>();
                var joints_per_face_dt = new GH_Structure<GH_Integer>();
                var three_valence_dt = new GH_Structure<GH_Integer>();
                var adjacency_dt = new GH_Structure<GH_Integer>();

                DA.GetDataTree(0, out plines_dt);
                DA.GetDataTree(1, out insertion_vectors_dt);
                DA.GetDataTree(2, out joints_per_face_dt);
                DA.GetDataTree(3, out three_valence_dt);
                DA.GetDataTree(4, out adjacency_dt);

                var data = new joinery_solver_net.Data();
                data.polylines = new Polyline[plines_dt.PathCount][];
                data.face_vectors = new Vector3d[insertion_vectors_dt.PathCount][];
                data.joints_types = new int[joints_per_face_dt.PathCount][];
                data.three_valence = new List<int[]>(three_valence_dt.PathCount);
                data.adjacency = new List<int[]>(adjacency_dt.PathCount);

                int n = plines_dt.PathCount;
                for (int i = 0; i < n; i++)
                {
                    data.polylines[i] = new Polyline[plines_dt[i].Count];
                    for (int j = 0; j < plines_dt[i].Count; j++)
                    {
                        plines_dt[i][j].Value.TryGetPolyline(out Polyline pline);
                        data.polylines[i][j] = pline;
                    }

                    if (insertion_vectors_dt.PathCount == n)
                    {
                        data.face_vectors[i] = new Vector3d[insertion_vectors_dt[i].Count];
                        for (int j = 0; j < insertion_vectors_dt[i].Count; j++)
                            data.face_vectors[i][j] = insertion_vectors_dt[i][j].Value;
                    }

                    if (joints_per_face_dt.PathCount == n)
                    {
                        data.joints_types[i] = new int[joints_per_face_dt[i].Count];
                        for (int j = 0; j < joints_per_face_dt[i].Count; j++)
                            data.joints_types[i][j] = joints_per_face_dt[i][j].Value;
                    }
                }

                for (int i = 0; i < three_valence_dt.PathCount; i++)
                {
                    int[] id = new int[three_valence_dt[i].Count];
                    for (int j = 0; j < three_valence_dt[i].Count; j++)
                        id[j] = three_valence_dt[i][j].Value;
                    data.three_valence.Add(id);
                }

                for (int i = 0; i < adjacency_dt.PathCount; i++)
                {
                    int[] id = new int[adjacency_dt[i].Count];
                    for (int j = 0; j < adjacency_dt[i].Count; j++)
                        id[j] = adjacency_dt[i][j].Value;
                    data.adjacency.Add(id);
                }

                DA.SetData(0, data);
            }
            catch (Exception e)
            {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.data_set;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("9deb91eb-6e97-4234-974a-7b0926605f11"); }
        }
    }
}
