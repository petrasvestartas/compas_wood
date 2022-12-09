#pragma once

#include <polylabel/polylabel.h>

namespace cgal_polylabel
{
    /**
     * inscribe circle in a polygon (with or without holes)
     * the polyline is oriented to XY plane using the input plane
     * then oriented back to 3D
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * EXAMPLE:
     *   std::vector<CGAL_Polyline> polylines;
     *
     *    CGAL_Polyline p0{
     *        IK::Point_3(-51.268164698738, 124.403259575835, 0),
     *        IK::Point_3(-49.9053069162896, 125.207270883064, 0),
     *        IK::Point_3(-49.2387999055025, 123.598641360433, 0),
     *        IK::Point_3(-51.268164698738, 124.403259575835, 0),
     *    };
     *
     *    CGAL_Polyline p1{
     *        IK::Point_3(-80.6608706869908, 113.550451407233, 0),
     *        IK::Point_3(-105.295437496349, 124.537127782527, 0),
     *        IK::Point_3(-76.4480884206953, 152.30682138802, 0),
     *        IK::Point_3(-23.6934094178782, 139.390066049447, 0),
     *        IK::Point_3(-40.1282334662578, 100.282298806669, 0),
     *        IK::Point_3(-69.6350088663798, 87.7972305312764, 0),
     *        IK::Point_3(-74.8538706538141, 108.464053813429, 0),
     *        IK::Point_3(-80.6608706869908, 113.550451407233, 0),
     *    };
     *
     *    CGAL_Polyline p2{
     *        IK::Point_3(-73.4808769486925, 119.997690099402, 0),
     *        IK::Point_3(-90.6424155869293, 123.188472670033, 0),
     *        IK::Point_3(-77.6978215210285, 136.742511096638, 0),
     *        IK::Point_3(-73.4808769486925, 119.997690099402, 0),
     *    };
     *
     *    polylines.push_back(p0);
     *    polylines.push_back(p1);
     *    polylines.push_back(p2);
     *
     *    std::tuple<IK::Point_3, IK::Plane_3, double> result = get_polylabel(polylines, 1.0);
     *    std::cout << std::get<0>(result).hx() << " " << std::get<0>(result).hy() << " " << std::get<0>(result).hz() << std::endl;
     *    std::cout << std::get<1>(result).orthogonal_vector().hx() << " " << std::get<1>(result).orthogonal_vector().hy() << " " << std::get<1>(result).orthogonal_vector().hz() << std::endl;
     *    std::cout << std::get<2>(result) << std::endl;
     *
     * RESULT:
     * Center -58.9292 109.972 -0
     * ZAxis 0 0 -8.18464
     * Radius 15.8093
     *
     *
     * @param polylines planar polylines rotated in space
     * @param precision precision, the smaller the number the less precise the result, default 1.0
     * @return a tuple that represents the circle (center, plane, radius)
     */
    std::tuple<IK::Point_3, IK::Plane_3, double> get_polylabel(std::vector<CGAL_Polyline> polylines, double precision = 1.0)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // copy polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines_copy = polylines;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation to XY plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 origin;
        IK::Plane_3 plane;
        cgal_polyline_util::get_fast_plane(polylines_copy[0], origin, plane);

        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(origin, plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        for (auto &polyline : polylines_copy)
            cgal_polyline_util::Transform(polyline, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get bounding box length of polylines_copy
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<double> len = std::vector<double>(polylines_copy.size());

        for (int i = 0; i < polylines_copy.size(); i++)
        {
            CGAL::Bbox_3 AABB = CGAL::bbox_3(polylines_copy[i].begin(), polylines_copy[i].end(), IK());
            auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
            auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
            len[i] = CGAL::squared_distance(p0, p1);
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sort polylines_copy indices by the bounding box length
        // WARNING orientation is not checked, which should be fine with the Polylabel algorithm
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<int> ids(len.size());
        std::iota(begin(ids), end(ids), 0);
        std::sort(begin(ids), end(ids), [&](int ia, int ib)
                  { return len[ia] < len[ib]; });
        std::reverse(begin(ids), end(ids));

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert polylines_copy to Polylabel datastructure, be aware that the polyline considers all polygons closed, so skip the last point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        mapbox::geometry::polygon<double> polygon_with_holes(polylines_copy.size());

        for (int i = 0; i < polylines_copy.size(); i++)
        {
            for (int j = 0; j < polylines_copy[ids[i]].size() - 1; j++)
            {
                polygon_with_holes[i].emplace_back(polylines_copy[ids[i]][j].x(), polylines_copy[ids[i]][j].y());
                // std::cout << polylines_copy[ids[i]][j].x() << " " << polylines_copy[ids[i]][j].y() << std::endl;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Run the Polylabel algorithm
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::array<double, 3> center_and_radius = mapbox::polylabel(polygon_with_holes, 1.0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert from the Polylabel datastructure to CGAL
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 center(center_and_radius[0], center_and_radius[1], 0);
        center = center.transform(xform_toXY_Inv);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return std::tuple<IK::Point_3, IK::Plane_3, double>(center, plane, center_and_radius[2]);
    }
}