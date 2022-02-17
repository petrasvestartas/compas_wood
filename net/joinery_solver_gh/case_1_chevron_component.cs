using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class case_1_chevron_component : GH_Component
    {
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add sliders
            double[] sliderValue = new double[] { 4, 100, 0, 0.05799, };
            double[] sliderMinValue = new double[] { 1, 900, 0.5, 0.05, };
            double[] sliderMaxValue = new double[] { 20, 1000, 1.0, 0.06, };
            int[] sliderID = new int[] { 1, 2, 3, 4 };

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

        public case_1_chevron_component()
          : base("chevron", "chevron",
              "chevron, NURBS subdivision into a quad mesh",
              "compas_wood", "cases")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddSurfaceParameter("surface", "surface", "surface", GH_ParamAccess.item);
            pManager.AddNumberParameter("u_divisions", "u_divisions", "u_divisions", GH_ParamAccess.item);
            pManager.AddNumberParameter("v_division_dist", "v_division_dist", "v_division_dist", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "shift", "shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("scale", "scale", "scale", GH_ParamAccess.item);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("mesh", "mesh", "mesh", GH_ParamAccess.item);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Surface surface = null;
            double u_divisions = 4;
            double v_division_dist = 900;
            double shift = 0.5;
            double scale = 0.05599;

            DA.GetData(0, ref surface);
            DA.GetData(1, ref u_divisions);
            DA.GetData(2, ref v_division_dist);
            DA.GetData(3, ref shift);
            DA.GetData(4, ref scale);

            chevron annen = new chevron();
            Mesh mesh = annen.chevron_grid(surface, (int)u_divisions, v_division_dist, shift, scale);

            DA.SetData(0, mesh);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.annen_chevron;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("9deb91eb-6e97-1234-174a-7b0926605f30"); }
        }
    }
}
