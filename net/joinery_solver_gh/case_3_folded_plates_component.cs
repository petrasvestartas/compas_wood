using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class case_3_folded_plates_component : GH_Component
    {
        public override GH_Exposure Exposure => GH_Exposure.tertiary;

        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add sliders
            double[] sliderValue = new double[] { 5, 2, 1, 0.5, 0.0 };
            double[] sliderMinValue = new double[] { 1, 1, 0.1, 0.01, -10 };
            double[] sliderMaxValue = new double[] { 10, 10, 10, 1, 10 };
            int[] sliderID = new int[] { 1, 2, 4, 5, 6 };

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

        public case_3_folded_plates_component()
          : base("case_3_folded_plates_component", "case_3",
              "creates a triangular mesh from nurbs surface and create plate offset",
              "compas_wood", "cases")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddSurfaceParameter("surface", "surface", "surface", GH_ParamAccess.item);
            pManager.AddNumberParameter("u_divisions", "u_divisions", "u_divisions", GH_ParamAccess.item);
            pManager.AddNumberParameter("v_divisions", "v_divisions", "v_divisions", GH_ParamAccess.item);
            pManager.AddPlaneParameter("base_planes", "base_planes", "planes to cut plates at the base, because they might be rotate based on normals", GH_ParamAccess.list);
            pManager[3].Optional = true;

            pManager.AddNumberParameter("thickness", "thickness", "thickness", GH_ParamAccess.item);
            pManager.AddNumberParameter("chamfer", "chamfer", "chamfer", GH_ParamAccess.item);
            pManager.AddNumberParameter("face_positions", "face_positions", "face_positions", GH_ParamAccess.list);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("mesh", "mesh", "Mesh", GH_ParamAccess.item);
            pManager.AddCurveParameter("plates", "plates", "Plates", GH_ParamAccess.tree);
            pManager.AddLineParameter("insertion", "insertion", "Insertion Lines", GH_ParamAccess.list);

            pManager.AddIntegerParameter("adjacency", "adjacency", "Adjacency", GH_ParamAccess.tree);
            pManager.AddBooleanParameter("flags", "flags", "Checkerboard pattern", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Surface surface = null;
            DA.GetData(0, ref surface);

            double u_divisions = 2;
            DA.GetData(1, ref u_divisions);

            double v_divisions = 2;
            DA.GetData(2, ref v_divisions);

            List<Plane> base_planes = new List<Plane>();
            DA.GetDataList(3, base_planes);

            double thickness = 1;
            DA.GetData(4, ref thickness);

            double chamfer = 0.01;
            DA.GetData(5, ref chamfer);

            List<double> face_positions = new List<double>();
            DA.GetDataList(6, face_positions);

            case_3_folded_plates mtsj = new case_3_folded_plates(surface, (int)u_divisions, (int)v_divisions, base_planes, thickness, chamfer, face_positions);

            DA.SetData(0, mtsj.m);
            DA.SetDataTree(1, rhino_util.GrasshopperUtil.IE2_GH_Structure(mtsj.f_polylines));
            DA.SetDataList(2, mtsj.insertion_lines);
            DA.SetDataTree(3, rhino_util.GrasshopperUtil.IE2(mtsj.adjacency));
            DA.SetDataList(4, mtsj.flags);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.folded_plates;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("a4039b62-df0f-4889-9e0e-f485829372bd"); }
        }
    }
}
