#pragma once
// #include "../../stdafx.h"
// #include "cgal_vector_util.h"

namespace cgal_box_util
{

    namespace internal
    {
        inline IK::Vector_3 vector_at(const IK::Vector_3 (&plane)[4], const double s, const double &&t, const double &c)
        {
            return (plane[0] + s * plane[1] + t * plane[2] + c * plane[3]);
        }

        inline IK::Point_3 point_at(const IK::Vector_3 (&plane)[5], const double &s, const double &t, const double &c)
        {
            return IK::Point_3(
                plane[0].x() + s * plane[1].x() + t * plane[2].x() + c * plane[3].x(),
                plane[0].y() + s * plane[1].y() + t * plane[2].y() + c * plane[3].y(),
                plane[0].z() + s * plane[1].z() + t * plane[2].z() + c * plane[3].z()

            );
        }

        inline void get_corners(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners)
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

        inline void get_corners_view(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners)
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

        inline void get_corners_box(const CGAL::Bbox_3 box, CGAL_Polyline &corners)
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

        inline void get_corners_box_view(const CGAL::Bbox_3 box, CGAL_Polyline &corners)
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

        inline bool get_separating_plane(const IK::Vector_3 &relative_position, const IK::Vector_3 &plane, const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
        {
            return (fabs(relative_position * plane) >
                    (fabs((box1[1] * box1[4].x()) * plane) +
                     fabs((box1[2] * box1[4].y()) * plane) +
                     fabs((box1[3] * box1[4].z()) * plane) +
                     fabs((box2[1] * box2[4].x()) * plane) +
                     fabs((box2[2] * box2[4].y()) * plane) +
                     fabs((box2[3] * box2[4].z()) * plane)));
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // Native Box Collision
        ///////////////////////////////////////////////////////////////////////////////////////////////
        // define the operations to be used in our 3D vertices
        struct vec3
        {
            double x, y, z;
            vec3 operator-(const vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
            double operator*(const vec3 &rhs) const { return {x * rhs.x + y * rhs.y + z * rhs.z}; }                                 // DOT PRODUCT
            vec3 operator^(const vec3 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; } // CROSS PRODUCT
            vec3 operator*(const double &rhs) const { return vec3{x * rhs, y * rhs, z * rhs}; }
        };

        // set the relevant elements of our oriented bounding box
        struct obb
        {
            vec3 Pos, AxisX, AxisY, AxisZ, Half_size;
        };

        // check if there's a separating plane in between the selected axes
        inline bool get_separating_plane(const vec3 &relative_position, const vec3 &plane, const obb &box1, const obb &box2)

        {
            // CGAL_Debug(relative_position);

            return (fabs(relative_position * plane) >
                    (fabs((box1.AxisX * box1.Half_size.x) * plane) +
                     fabs((box1.AxisY * box1.Half_size.y) * plane) +
                     fabs((box1.AxisZ * box1.Half_size.z) * plane) +
                     fabs((box2.AxisX * box2.Half_size.x) * plane) +
                     fabs((box2.AxisY * box2.Half_size.y) * plane) +
                     fabs((box2.AxisZ * box2.Half_size.z) * plane)));
        }

        // test for separating planes in all 15 axes
        inline bool get_collision(const obb &box1, const obb &box2)
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

        inline bool get_collision_native(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
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

    /**
     * transform a plane as a vector array
     *
     * @param plane c array that is composed of 4 IK::Vector_3
     * @param xform CGAL 4_4 transformation matrix
     */
    inline void transform_plane_as_vector_array(IK::Vector_3 *plane, const CGAL::Aff_transformation_3<IK> &xform)
    {
        IK::Point_3 p(plane[0].x(), plane[0].y(), plane[0].z());
        p = xform.transform(p);

        plane[0] = IK::Vector_3(p.x(), p.y(), p.z());

        for (int i = 1; i < 4; i++)
            plane[i] = xform.transform(plane[i]);
    }

    /**
     * assign c style array for boxes
     * 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     *
     * @param source 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     * @param target 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     */
    inline void assign(const IK::Vector_3 *source, IK::Vector_3 *target, const int &n)
    {
        for (int i = 0; i < n; i++)
            target[i] = source[i];
    }

    /**
     * check if two boxes are colliding
     *
     * @param box1 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     * @param box2 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     * @return bool value, if boxes are colliding
     */
    inline bool get_collision(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5])
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
