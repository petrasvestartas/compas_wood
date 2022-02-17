using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Drawing;

namespace joinery_solver_gh
{
    public class display_loft_mesh_component : GH_Component
    {
        public display_loft_mesh_component()
          : base("loft", "loft",
              "display_loft_mesh_component",
                   "compas_wood", "view")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("polylines0", "p0", "Polylines, If this DataTree is supplied then c<0.5", GH_ParamAccess.list);
            pManager.AddCurveParameter("polylines1", "p1", "Polylines, If this DataTree is supplied then i%2", GH_ParamAccess.list);
            pManager[0].Optional = true;
            pManager[1].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("mesh", "m", "Mesh Walls", GH_ParamAccess.item);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            List<Curve> curves0 = new List<Curve>();
            List<Curve> curves1 = new List<Curve>();
            bool f0 = DA.GetDataList(0, curves0);
            bool f1 = DA.GetDataList(1, curves1);

            if (!f0 && !f1) return;

            DA.SetData(0, rhino_util.MeshLoftUtil.loft_polylines_with_holes(curves0, curves1));
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.loft;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("4db308b8-6dc7-4713-b6ea-57c039bcc0bf"); }
        }
    }
}
