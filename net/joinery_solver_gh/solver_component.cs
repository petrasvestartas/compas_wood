using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using joinery_solver_net;

namespace joinery_solver_gh
{
    public class solver_component : GH_Component
    {
        private Polyline[][] out_polylines = null;
        private BoundingBox bbox = BoundingBox.Unset;

        protected override void BeforeSolveInstance()
        {
            out_polylines = null;
            bbox = BoundingBox.Unset;
        }

        public override BoundingBox ClippingBox => bbox;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            var col = Attributes.Selected ? args.WireColour_Selected : args.WireColour;
            var lineWeight = args.DefaultCurveThickness;

            if (!base.Hidden && !base.Locked)
            {
                if (out_polylines != null)
                {
                    foreach (var plines in out_polylines)
                        foreach (Polyline pline in plines)
                            args.Display.DrawPolyline(pline, col, lineWeight);
                }
            }
        }

        public solver_component()
          : base("solver", "solver",
              "generated timber joints",
              "compas_wood", "solvers")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.item);
            pManager.AddNumberParameter("join", "join", "joint parameters", GH_ParamAccess.list);
            pManager[1].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.item);
            pManager.HideParameter(0);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            try
            {
                joinery_solver_net.Data data = new joinery_solver_net.Data();
                DA.GetData(0, ref data);

                var joint_params = new List<double>(18);
                DA.GetDataList(1, joint_params);

                if (joint_params.Count != 18)
                {
                    double division_length = 300;
                    joint_params = new List<double>{
        division_length, 0.5, 9,
        division_length * 1.5,0.65,10,
        division_length * 1.5, 0.5,21,
        division_length, 0.5,30,
        division_length, 0.5,40,
        division_length, 0.5,50
        };
                }

                //var watch = new System.Diagnostics.Stopwatch();
                //watch.Start();
                joinery_solver_net.Test.pinvoke_get_connection_zones(ref out_polylines, (joinery_solver_net.Data)(data), joint_params, 0);
                joinery_solver_net.Data output_data = new Data() { polylines = out_polylines };
                DA.SetData(0, output_data);
                //watch.Stop();

                if (out_polylines.Length == 0) return;

                //Display
                this.bbox = out_polylines[0][0].BoundingBox;
                foreach (var plines in out_polylines)
                    foreach (Polyline pline in plines)
                        this.bbox.Union(pline.BoundingBox);
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
                return Properties.Resources.solver_1;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("f78aa53a-910f-4c7e-92b2-29f3712e054a"); }
        }
    }
}
