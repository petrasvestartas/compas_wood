using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class input_get_component : GH_Component
    {
        public input_get_component()
          : base("input_get", "input_get",
              "input_get",
              "compas_wood", "data")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.item);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddCurveParameter("plines", "plines", "plines", GH_ParamAccess.tree);
            pManager.AddVectorParameter("insertion_vectors", "insertion_vectors", "insertion_vectors", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("joints_per_face", "joints_per_face", "joints_per_face", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("three_valence", "three_valence", "three_valence", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("adjacency", "adjacency", "adjacency", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            var data = new joinery_solver_net.Data();
            DA.GetData(0, ref data);

            DA.SetDataTree(0, rhino_util.GrasshopperUtil.IE2(data.polylines));

            DA.SetDataTree(1, rhino_util.GrasshopperUtil.IE2(data.face_vectors));
            DA.SetDataTree(2, rhino_util.GrasshopperUtil.IE2(data.joints_types));
            DA.SetDataTree(3, rhino_util.GrasshopperUtil.IE2(data.three_valence));
            DA.SetDataTree(4, rhino_util.GrasshopperUtil.IE2(data.adjacency));
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.data_get;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("9deb91eb-6e97-4231-974a-7b0586605f14"); }
        }
    }
}
