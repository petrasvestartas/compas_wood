using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh.Resources
{
    public class case_2_vda_component : GH_Component
    {
        public override GH_Exposure Exposure => GH_Exposure.secondary;

        public case_2_vda_component()
          : base("connectors", "connectors",
              "case_2_vda_component",
               "compas_wood", "cases")
        {
        }

        //Mesh m, List<double> face_positions, double face_thickness,
        //List<int> divisions, List<double> division_len,
        //List<Line> edge_vectors, double rect_width, double rect_height, double rect_thickness, List<Brep> projection,

        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add sliders
            double[] sliderValue = new double[] { 0, 4, 2, 15, 12, 12, 1 };
            double[] sliderMinValue = new double[] { -10, 0, 1, 1, -20, -20, 1 };
            double[] sliderMaxValue = new double[] { 10, 10, 10, 100, 20, 20, 1 };
            int[] sliderID = new int[] { 1, 2, 3, 4, 6, 7, 8 };

            for (int i = 0; i < sliderValue.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Number ni = Params.Input[sliderID[i]] as Grasshopper.Kernel.Parameters.Param_Number;
                if (ni == null || ni.SourceCount > 0 || ni.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ni.Attributes.Pivot.X - 250;
                int y = (int)ni.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_NumberSlider slider = new Grasshopper.Kernel.Special.GH_NumberSlider();
                slider.SetInitCode(string.Format("{0}<{1}<{2}", sliderMinValue[i], sliderValue[i], sliderMaxValue[i]));
                slider.CreateAttributes();
                slider.Attributes.Pivot = new System.Drawing.PointF(x, y);
                slider.Attributes.ExpireLayout();
                document.AddObject(slider, false);
                ni.AddSource(slider);
            }
        }

        protected override void AfterSolveInstance()
        {
            GH_Document ghdoc = base.OnPingDocument();
            for (int i = 0; i < ghdoc.ObjectCount; i++)
            {
                IGH_DocumentObject obj = ghdoc.Objects[i];
                if (obj.Attributes.DocObject.ToString().Equals("Grasshopper.Kernel.Special.GH_Group"))
                {
                    Grasshopper.Kernel.Special.GH_Group groupp = (Grasshopper.Kernel.Special.GH_Group)obj;
                    if (groupp.ObjectIDs.Contains(this.InstanceGuid))
                        return;
                }
            }

            List<Guid> guids = new List<Guid>() { this.InstanceGuid };

            foreach (var param in base.Params.Input)
                foreach (IGH_Param source in param.Sources)
                    guids.Add(source.InstanceGuid);

            Grasshopper.Kernel.Special.GH_Group g = new Grasshopper.Kernel.Special.GH_Group();
            g.NickName = base.Name.ToString();

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 255);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("mesh", "mesh", "mesh", GH_ParamAccess.item);
            pManager.AddNumberParameter("face_positions", "face_positions", "face_positions", GH_ParamAccess.list);
            pManager.AddNumberParameter("face_thickness", "face_thickness", "face_thickness", GH_ParamAccess.item);
            pManager.AddNumberParameter("divisions", "divisions", "divisions", GH_ParamAccess.list);
            pManager.AddNumberParameter("division_len", "division_len", "division_len", GH_ParamAccess.list);

            pManager.AddLineParameter("edge_vectors", "edge_vectors", "edge_vectors", GH_ParamAccess.list);
            pManager.AddNumberParameter("rect_width", "rect_width", "rect_width", GH_ParamAccess.item);
            pManager.AddNumberParameter("rect_height", "rect_height", "rect_height", GH_ParamAccess.item);
            pManager.AddNumberParameter("rect_thickness", "rect_thickness", "rect_thickness", GH_ParamAccess.item);

            pManager.AddBrepParameter("projection", "projection", "projection", GH_ParamAccess.list);

            pManager[5].Optional = true;
            pManager[9].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddCurveParameter("f", "f", "f", GH_ParamAccess.tree);
            pManager.AddPlaneParameter("f_pl", "f_pl", "f_pl", GH_ParamAccess.tree);
            pManager.AddTextParameter("f_id", "f_id", "f_id", GH_ParamAccess.tree);

            pManager.AddCurveParameter("e", "e", "e", GH_ParamAccess.tree);
            pManager.AddPlaneParameter("e_pl", "e_pl", "e_pl", GH_ParamAccess.tree);
            pManager.AddTextParameter("e_id", "e_id", "e_id", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh m = null;
            DA.GetData(0, ref m);

            List<double> face_positions = new List<double>();
            DA.GetDataList(1, face_positions);

            double face_thickness = 1;
            DA.GetData(2, ref face_thickness);

            List<double> divisions = new List<double>();
            DA.GetDataList(3, divisions);

            List<double> division_len = new List<double>();
            DA.GetDataList(4, division_len);

            List<Line> edge_vectors = new List<Line>();
            DA.GetDataList(5, edge_vectors);

            double rect_width = 10;
            DA.GetData(6, ref rect_width);

            double rect_height = 10;
            DA.GetData(7, ref rect_height);

            double rect_thickness = 1;
            DA.GetData(8, ref rect_thickness);

            List<Brep> projection = new List<Brep>();
            DA.GetDataList(9, projection);

            case_2_vda vda = new case_2_vda(
              m, null,
              face_thickness, face_positions,
              divisions, division_len,
              edge_vectors, rect_width, rect_height, rect_thickness, projection);

            DA.SetDataTree(0, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.f_polylines));
            DA.SetDataTree(1, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.f_polylines_planes));
            DA.SetDataTree(2, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.f_polylines_index));
            DA.SetDataTree(3, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.e_polylines));
            DA.SetDataTree(4, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.e_polylines_planes));
            DA.SetDataTree(5, rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.e_polylines_index));
            //var v = rhino_util.GrasshopperUtil.IE2_GH_Structure(vda.e_polylines_index)[0][0];
            //Rhino.RhinoApp.WriteLine(v.Value);
            //Rhino.RhinoApp.WriteLine(vda.e_polylines_index[0][0]);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.case_2_cross;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("d055b371-8b64-4376-9c89-a733caebeda6"); }
        }
    }
}
