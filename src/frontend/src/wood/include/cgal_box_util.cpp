#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is
 
#include "cgal_box_util.h"

namespace cgal_box_util
{

    namespace internal
    {

        IK::Vector_3 vector_at(const IK::Vector_3 (&plane)[4], const double &x_axis, const double &y_axis, const double &z_axis)
        {
            return (plane[0] + x_axis * plane[1] + y_axis * plane[2] + z_axis * plane[3]);
        }


        IK::Point_3 point_at(const IK::Vector_3 (&box)[5], const double &x_axis, const double &y_axis, const double &z_axis)
        {
            return IK::Point_3(
                box[0].x() + x_axis * box[1].x() + y_axis * box[2].x() + z_axis * box[3].x(),
                box[0].y() + x_axis * box[1].y() + y_axis * box[2].y() + z_axis * box[3].y(),
                box[0].z() + x_axis * box[1].z() + y_axis * box[2].z() + z_axis * box[3].z()

            );
        }


        void get_corners(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners)
        {
            corners = CGAL_Polyline(8);

            corners[0] = point_at(box, box[4].x(), box[4].y(), -box[4].z());
            corners[1] = point_at(box, -box[4].x(), box[4].y(), -box[4].z());
            corners[3] = point_at(box, box[4].x(), -box[4].y(), -box[4].z());
            corners[2] = point_at(box, -box[4].x(), -box[4].y(), -box[4].z());

            corners[4] = point_at(box, box[4].x(), box[4].y(), box[4].z());
            corners[5] = point_at(box, -box[4].x(), box[4].y(), box[4].z());
            corners[7] = point_at(box, box[4].x(), -box[4].y(), box[4].z());
            corners[6] = point_at(box, -box[4].x(), -box[4].y(), box[4].z());
        }


        void get_two_box_rectangles(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners)
        {
            corners = CGAL_Polyline(10);

            corners[0] = point_at(box, box[4].x(), box[4].y(), -box[4].z());
            corners[1] = point_at(box, -box[4].x(), box[4].y(), -box[4].z());
            corners[2] = point_at(box, -box[4].x(), -box[4].y(), -box[4].z());
            corners[3] = point_at(box, box[4].x(), -box[4].y(), -box[4].z());
            corners[4] = point_at(box, box[4].x(), box[4].y(), -box[4].z());

            corners[5] = point_at(box, box[4].x(), box[4].y(), box[4].z());
            corners[6] = point_at(box, -box[4].x(), box[4].y(), box[4].z());
            corners[7] = point_at(box, -box[4].x(), -box[4].y(), box[4].z());
            corners[8] = point_at(box, box[4].x(), -box[4].y(), box[4].z());
            corners[9] = point_at(box, box[4].x(), box[4].y(), box[4].z());
        }


        void get_corners_box(const CGAL::Bbox_3& box, CGAL_Polyline &corners)
        {
            corners = CGAL_Polyline(8);

            corners[0] = IK::Point_3(box.xmin(), box.ymin(), box.zmin());
            corners[1] = IK::Point_3(box.xmin(), box.ymin(), box.zmax());
            corners[2] = IK::Point_3(box.xmin(), box.ymax(), box.zmax());
            corners[3] = IK::Point_3(box.xmin(), box.ymax(), box.zmin());

            corners[4] = IK::Point_3(box.xmax(), box.ymin(), box.zmin());
            corners[5] = IK::Point_3(box.xmax(), box.ymin(), box.zmax());
            corners[6] = IK::Point_3(box.xmax(), box.ymax(), box.zmax());
            corners[7] = IK::Point_3(box.xmax(), box.ymax(), box.zmin());
        }

        void get_two_box_rectangles(const CGAL::Bbox_3& box, CGAL_Polyline &corners)
        {
            corners = CGAL_Polyline(10);

            corners[0] = IK::Point_3(box.xmin(), box.ymin(), box.zmin());
            corners[1] = IK::Point_3(box.xmin(), box.ymin(), box.zmax());
            corners[2] = IK::Point_3(box.xmin(), box.ymax(), box.zmax());
            corners[3] = IK::Point_3(box.xmin(), box.ymax(), box.zmin());
            corners[4] = IK::Point_3(box.xmin(), box.ymin(), box.zmin());

            corners[5] = IK::Point_3(box.xmax(), box.ymin(), box.zmin());
            corners[6] = IK::Point_3(box.xmax(), box.ymin(), box.zmax());
            corners[7] = IK::Point_3(box.xmax(), box.ymax(), box.zmax());
            corners[8] = IK::Point_3(box.xmax(), box.ymax(), box.zmin());
            corners[9] = IK::Point_3(box.xmax(), box.ymin(), box.zmin());
        }


        bool get_separating_plane(const IK::Vector_3 &relative_position, const IK::Vector_3 &axis, const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
        {
            return (fabs(relative_position * axis) >
                    (fabs((box1[1] * box1[4].x()) * axis) +
                     fabs((box1[2] * box1[4].y()) * axis) +
                     fabs((box1[3] * box1[4].z()) * axis) +
                     fabs((box2[1] * box2[4].x()) * axis) +
                     fabs((box2[2] * box2[4].y()) * axis) +
                     fabs((box2[3] * box2[4].z()) * axis)));
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // Native Box Collision
        ///////////////////////////////////////////////////////////////////////////////////////////////


        //struct vec3

        //struct obb

        bool get_separating_plane(const vec3 &relative_position, const vec3 &axis, const obb &box1, const obb &box2)
        {

            return (fabs(relative_position * axis) >
                    (fabs((box1.AxisX * box1.Half_size.x) * axis) +
                     fabs((box1.AxisY * box1.Half_size.y) * axis) +
                     fabs((box1.AxisZ * box1.Half_size.z) * axis) +
                     fabs((box2.AxisX * box2.Half_size.x) * axis) +
                     fabs((box2.AxisY * box2.Half_size.y) * axis) +
                     fabs((box2.AxisZ * box2.Half_size.z) * axis)));
        }


        bool get_collision(const obb &box1, const obb &box2)
        {
            static vec3 relative_position;
            relative_position = box2.Pos - box1.Pos;

            return !(get_separating_plane(relative_position, box1.AxisX, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisY, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisZ, box1, box2) ||
                     get_separating_plane(relative_position, box2.AxisX, box1, box2) ||
                     get_separating_plane(relative_position, box2.AxisY, box1, box2) ||
                     get_separating_plane(relative_position, box2.AxisZ, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisX ^ box2.AxisX, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisX ^ box2.AxisY, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisX ^ box2.AxisZ, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisY ^ box2.AxisX, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisY ^ box2.AxisY, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisY ^ box2.AxisZ, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisZ ^ box2.AxisX, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisZ ^ box2.AxisY, box1, box2) ||
                     get_separating_plane(relative_position, box1.AxisZ ^ box2.AxisZ, box1, box2));
        }


        bool get_collision_native(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
        {
            // Test
            //  create two obbs
            obb A, B;

            // set the first obb's properties
            A.Pos = {box1[0].x(), box1[0].y(), box1[0].z()}; // set its center position

            // set the half size
            A.Half_size.x = box1[4].x();
            A.Half_size.y = box1[4].y();
            A.Half_size.z = box1[4].z();

            // set the axes orientation
            A.AxisX = {box1[1].x(), box1[1].y(), box1[1].z()};
            A.AxisY = {box1[2].x(), box1[2].y(), box1[2].z()};
            A.AxisZ = {box1[3].x(), box1[3].y(), box1[3].z()};

            // set the second obb's properties
            B.Pos = {box2[0].x(), box2[0].y(), box2[0].z()}; // set its center position

            // set the half size
            B.Half_size.x = box2[4].x();
            B.Half_size.y = box2[4].y();
            B.Half_size.z = box2[4].z();

            // set the axes orientation
            B.AxisX = {box2[1].x(), box2[1].y(), box2[1].z()};
            B.AxisY = {box2[2].x(), box2[2].y(), box2[2].z()};
            B.AxisZ = {box2[3].x(), box2[3].y(), box2[3].z()};

            // run the code and get the result as a message
            return get_collision(A, B);
        }
    }


    void transform_plane_as_vector_array(IK::Vector_3 *plane, const CGAL::Aff_transformation_3<IK> &xform)
    {
        IK::Point_3 p(plane[0].x(), plane[0].y(), plane[0].z());
        p = xform.transform(p);

        plane[0] = IK::Vector_3(p.x(), p.y(), p.z());

        for (int i = 1; i < 4; i++)
            plane[i] = xform.transform(plane[i]);
    }


    void assign(const IK::Vector_3 *source, IK::Vector_3 *target, const int &n)
    {
        for (int i = 0; i < n; i++)
            target[i] = source[i];
    }


    bool get_collision(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
    {
        IK::Vector_3 relative_position = box2[0] - box1[0];

        return !(internal::get_separating_plane(relative_position, box1[1], box1, box2) ||
                 internal::get_separating_plane(relative_position, box1[2], box1, box2) ||
                 internal::get_separating_plane(relative_position, box1[3], box1, box2) ||
                 internal::get_separating_plane(relative_position, box2[1], box1, box2) ||
                 internal::get_separating_plane(relative_position, box2[2], box1, box2) ||
                 internal::get_separating_plane(relative_position, box2[3], box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[1], box2[1]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[1], box2[2]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[1], box2[3]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[2], box2[1]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[2], box2[2]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[2], box2[3]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[3], box2[1]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[3], box2[2]), box1, box2) ||
                 internal::get_separating_plane(relative_position, CGAL::cross_product(box1[3], box2[3]), box1, box2));
    }

}
