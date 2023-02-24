using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rhino.Geometry;

namespace joinery_solver_net

{
    public enum element_type
    {
        plate = 0,
        rect_beam = 1,
        round_beam = 2,
        glulam = 3
    }
    public class element
    {
        public element_type type;// "plate" | "rect_beam" | "round_beam" | "glulam"        else if (element_type == "1")
        public int id = -1;

        public Plane base_plane = Plane.Unset;

        //plate
        public List<Polyline> top = new List<Polyline>();//outline is first and the other ones are curves
        public List<Polyline> bot = new List<Polyline>();

        //beam
        public List<GeometryBase> geometry = new List<GeometryBase>();
        public Polyline axis = new Polyline();
        public List<double> axis_radius = new List<double>();
        public List<Plane> axis_planes = new List<Plane>();

        //fabrication
        public List<cut> cuts = new List<cut>();

        public element()
        {
        }

        public element(element e)
        {
            this.type = e.type;
            this.id = e.id;
            this.base_plane = e.base_plane;

            //plate
            this.top = new List<Polyline>();
            foreach (var pline in e.top)
                this.top.Add(pline.Duplicate());

            this.bot = new List<Polyline>();
            foreach (var pline in e.bot)
                this.bot.Add(pline.Duplicate());

            //beam
            this.geometry = new List<GeometryBase>();
            for (int i = 0; i < e.geometry.Count; i++)
                this.geometry.Add(e.geometry[i].Duplicate());

            this.axis = e.axis.Duplicate();
            this.axis_radius = e.axis_radius;
            this.axis_planes = new List<Plane>();
            for (int i = 0; i < e.axis_planes.Count; i++)
                this.axis_planes.Add(new Plane(e.axis_planes[i]));


            //fabrication cuts

            this.cuts = new List<cut>();
            for (int i = 0; i < e.cuts.Count; i++)
            {
                this.cuts.Add(e.cuts[i].duplicate());
                //Rhino.RhinoApp.WriteLine(this.cuts.Last().geometry.ToString());
            }

        }


        public element duplicate()
        {
            return new element(this);
        }

        public void transform(Transform xform)
        {

            this.base_plane.Transform(xform);

            //plate
            for (int i = 0; i < top.Count; i++)
                top[i].Transform(xform);

            for (int i = 0; i < bot.Count; i++)
                bot[i].Transform(xform);

            //beam
            for (int i = 0; i < geometry.Count; i++)
                geometry[i].Transform(xform);

            this.axis.Transform(xform);

            for (int i = 0; i < this.axis_planes.Count; i++)
                this.axis_planes[i].Transform(xform);


            //fabrication cuts
            for (int i = 0; i < cuts.Count; i++)
                this.cuts[i].transform(xform);
        }

        public void compute_base_plane()
        {
            if (type == element_type.plate)
            {
                Plane plane = rhino_util.PolylineUtil.plane(this.bot[0]);
                Point3d center = rhino_util.PolylineUtil.CenterPoint(this.top[0]);
                if (plane.DistanceTo(center) < 0)
                    plane.Flip();
                base_plane = plane;
            }
            else if (type == element_type.round_beam || type == element_type.rect_beam || type == element_type.glulam)
            {
                base_plane = new Plane(axis[0], axis.SegmentAt(0).Direction);
            }


        }

        public void transform(Plane target_plane)
        {
            if (base_plane == Plane.Unset || base_plane.ZAxis.X < -1E+100)
                compute_base_plane();
            transform(Transform.plane_to_plane(this.base_plane, target_plane));
        }

        public override string ToString()
        {
            return this.type.ToString() + "_" + this.id.ToString() + "_geometry_" + geometry.Count().ToString() + "_cuts_" + cuts.Count().ToString();
        }

    }
}
