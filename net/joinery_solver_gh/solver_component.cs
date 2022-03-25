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
            pManager.AddNumberParameter("join_p", "join_p", "joint parameters", GH_ParamAccess.list);
            pManager.AddNumberParameter("scale", "scale", "scale joint parameters", GH_ParamAccess.list);
            pManager.AddIntegerParameter("find", "find", "searc types 0 - face-to-face, 1 - plane-to-face (cross), 2 - all", GH_ParamAccess.item, 0);
            pManager.AddIntegerParameter("get", "get", "0 - get top outlines and one joint_area \n 1 - get top outlines and two joint lines \n    2 - top outlines and joint volumes first and third \n  3 - get top outlines and joint polylines \n  4 - get  joints geometry as closed polylines performing intersection", GH_ParamAccess.item, 4);
            pManager[1].Optional = true;
            pManager[2].Optional = true;
            pManager[3].Optional = true;
            pManager[4].Optional = true;
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

                var scale = new List<double>(18);
                DA.GetDataList(2, scale);

                int search_type = 0;
                DA.GetData(3, ref search_type);
                int output_type = 1;
                DA.GetData(4, ref output_type);

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

                if (scale.Count < 3)
                {
                    scale = new List<double> { 1, 1, 1 };
                }

                //var watch = new System.Diagnostics.Stopwatch();
                //watch.Start();
                joinery_solver_net.Test.pinvoke_get_connection_zones(ref out_polylines, (joinery_solver_net.Data)(data), joint_params, scale, search_type, output_type);
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
