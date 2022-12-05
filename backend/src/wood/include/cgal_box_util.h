#pragma once
//#include "../../stdafx.h"
//#include "cgal_vector_util.h"


namespace cgal_box_util {
    inline IK::Vector_3 VectorAt(IK::Vector_3(&plane)[4], double s, double t, double c) {
        return (plane[0] + s * plane[1] + t * plane[2] + c * plane[3]);
    }

    inline IK::Point_3 PointAt(IK::Vector_3(&plane)[5], double s, double t, double c) {
        return IK::Point_3(
            plane[0].x() + s * plane[1].x() + t * plane[2].x() + c * plane[3].x(),
            plane[0].y() + s * plane[1].y() + t * plane[2].y() + c * plane[3].y(),
            plane[0].z() + s * plane[1].z() + t * plane[2].z() + c * plane[3].z()

        );

        // IK::Point_3 v = plane[0] + s * plane[1] + t * plane[2] + c * plane[3];

        //CGAL_Debug(plane[0]);
        //CGAL_Debug(plane[1]);
        //CGAL_Debug(plane[2]);
        //CGAL_Debug(plane[3]);
        //CGAL_Debug(v);

        //return IK::Point_3(v.x(), v.y(), v.z());
    }

    inline void GetCorners(IK::Vector_3(&box)[5], CGAL_Polyline& corners) {
        corners = CGAL_Polyline(8);

        //corners[0] = PointAt(box, -box[4].x(), -box[4].y(), -box[4].z());
        //corners[1] = PointAt(box, -box[4].x(), -box[4].y(), box[4].z());
        //corners[3] = PointAt(box, -box[4].x(), box[4].y(), box[4].z());
        //corners[2] = PointAt(box, -box[4].x(), box[4].y(), -box[4].z());

        //corners[4] = PointAt(box, box[4].x(), -box[4].y(), -box[4].z());
        //corners[5] = PointAt(box, box[4].x(), -box[4].y(), box[4].z());
        //corners[7] = PointAt(box, box[4].x(), box[4].y(), box[4].z());
        //corners[6] = PointAt(box, box[4].x(), box[4].y(), -box[4].z());

        corners[0] = PointAt(box, box[4].x(), box[4].y(), -box[4].z());
        corners[1] = PointAt(box, -box[4].x(), box[4].y(), -box[4].z());
        corners[3] = PointAt(box, box[4].x(), -box[4].y(), -box[4].z());
        corners[2] = PointAt(box, -box[4].x(), -box[4].y(), -box[4].z());

        corners[4] = PointAt(box, box[4].x(), box[4].y(), box[4].z());
        corners[5] = PointAt(box, -box[4].x(), box[4].y(), box[4].z());
        corners[7] = PointAt(box, box[4].x(), -box[4].y(), box[4].z());
        corners[6] = PointAt(box, -box[4].x(), -box[4].y(), box[4].z());
    }

    inline void GetCornersView(IK::Vector_3(&box)[5], CGAL_Polyline& corners) {
        corners = CGAL_Polyline(10);

        corners[0] = PointAt(box, box[4].x(), box[4].y(), -box[4].z());
        corners[1] = PointAt(box, -box[4].x(), box[4].y(), -box[4].z());
        corners[2] = PointAt(box, -box[4].x(), -box[4].y(), -box[4].z());
        corners[3] = PointAt(box, box[4].x(), -box[4].y(), -box[4].z());
        corners[4] = PointAt(box, box[4].x(), box[4].y(), -box[4].z());

        corners[5] = PointAt(box, box[4].x(), box[4].y(), box[4].z());
        corners[6] = PointAt(box, -box[4].x(), box[4].y(), box[4].z());
        corners[7] = PointAt(box, -box[4].x(), -box[4].y(), box[4].z());
        corners[8] = PointAt(box, box[4].x(), -box[4].y(), box[4].z());
        corners[9] = PointAt(box, box[4].x(), box[4].y(), box[4].z());
    }

    inline void GetCornersBox(CGAL::Bbox_3 box, CGAL_Polyline& corners) {
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

    inline void GetCornersBoxView(CGAL::Bbox_3 box, CGAL_Polyline& corners) {
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

    inline void TransformPlaneOrPlane(IK::Vector_3* plane, CGAL::Aff_transformation_3<IK> t) {
        IK::Point_3 p(plane[0].x(), plane[0].y(), plane[0].z());
        p = t.transform(p);

        plane[0] = IK::Vector_3(p.x(), p.y(), p.z());

        for (int i = 1; i < 4; i++) {
            plane[i] = t.transform(plane[i]);
        }
    }

    inline void Assign(IK::Vector_3* source, IK::Vector_3* target, int n) {
        for (int i = 0; i < n; i++)
            target[i] = source[i];
    }

    inline bool getSeparatingPlane(const IK::Vector_3& RPos, const IK::Vector_3& Plane, const IK::Vector_3(&box1)[5], const IK::Vector_3(&box2)[5]) {
        return (fabs(RPos * Plane) >
            (fabs((box1[1] * box1[4].x()) * Plane) +
                fabs((box1[2] * box1[4].y()) * Plane) +
                fabs((box1[3] * box1[4].z()) * Plane) +
                fabs((box2[1] * box2[4].x()) * Plane) +
                fabs((box2[2] * box2[4].y()) * Plane) +
                fabs((box2[3] * box2[4].z()) * Plane)));
    }

    inline bool GetCollision(const IK::Vector_3(&box1)[5], const IK::Vector_3(&box2)[5]) {
        IK::Vector_3 RPos = box2[0] - box1[0];

        return !(getSeparatingPlane(RPos, box1[1], box1, box2) ||
            getSeparatingPlane(RPos, box1[2], box1, box2) ||
            getSeparatingPlane(RPos, box1[3], box1, box2) ||
            getSeparatingPlane(RPos, box2[1], box1, box2) ||
            getSeparatingPlane(RPos, box2[2], box1, box2) ||
            getSeparatingPlane(RPos, box2[3], box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[1], box2[1]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[1], box2[2]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[1], box2[3]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[2], box2[1]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[2], box2[2]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[2], box2[3]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[3], box2[1]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[3], box2[2]), box1, box2) ||
            getSeparatingPlane(RPos, CGAL::cross_product(box1[3], box2[3]), box1, box2));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //Native Box Collision
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // define the operations to be used in our 3D vertices
    struct vec3 {
        double x, y, z;
        vec3 operator-(const vec3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
        double operator*(const vec3& rhs) const { return { x * rhs.x + y * rhs.y + z * rhs.z }; }									// DOT PRODUCT
        vec3 operator^(const vec3& rhs) const { return { y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x }; } // CROSS PRODUCT
        vec3 operator*(const double& rhs) const { return vec3{ x * rhs, y * rhs, z * rhs }; }
    };

    // set the relevant elements of our oriented bounding box
    struct OBB {
        vec3 Pos, AxisX, AxisY, AxisZ, Half_size;
    };

    // check if there's a separating plane in between the selected axes
    inline bool getSeparatingPlane(const vec3& RPos, const vec3& Plane, const OBB& box1, const OBB& box2)

    {
        //CGAL_Debug(RPos);

        return (fabs(RPos * Plane) >
            (fabs((box1.AxisX * box1.Half_size.x) * Plane) +
                fabs((box1.AxisY * box1.Half_size.y) * Plane) +
                fabs((box1.AxisZ * box1.Half_size.z) * Plane) +
                fabs((box2.AxisX * box2.Half_size.x) * Plane) +
                fabs((box2.AxisY * box2.Half_size.y) * Plane) +
                fabs((box2.AxisZ * box2.Half_size.z) * Plane)));
    }

    // test for separating planes in all 15 axes
    inline bool getCollision(const OBB& box1, const OBB& box2) {
        static vec3 RPos;
        RPos = box2.Pos - box1.Pos;

        return !(getSeparatingPlane(RPos, box1.AxisX, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisY, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisZ, box1, box2) ||
            getSeparatingPlane(RPos, box2.AxisX, box1, box2) ||
            getSeparatingPlane(RPos, box2.AxisY, box1, box2) ||
            getSeparatingPlane(RPos, box2.AxisZ, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisX ^ box2.AxisX, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisX ^ box2.AxisY, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisX ^ box2.AxisZ, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisY ^ box2.AxisX, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisY ^ box2.AxisY, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisY ^ box2.AxisZ, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisZ ^ box2.AxisX, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisZ ^ box2.AxisY, box1, box2) ||
            getSeparatingPlane(RPos, box1.AxisZ ^ box2.AxisZ, box1, box2));
    }

    //inline crossProduct
    //vec3 operator^ (const vec3& rhs) const {
    //    return{ y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x };

    inline bool GetCollisionNative(const IK::Vector_3(&box1)[5], const IK::Vector_3(&box2)[5]) {
        //Test
        // create two obbs
        OBB A, B;

        // set the first obb's properties
        A.Pos = { box1[0].x(), box1[0].y(), box1[0].z() }; // set its center position

        // set the half size
        A.Half_size.x = box1[4].x();
        A.Half_size.y = box1[4].y();
        A.Half_size.z = box1[4].z();

        // set the axes orientation
        A.AxisX = { box1[1].x(), box1[1].y(), box1[1].z() };
        A.AxisY = { box1[2].x(), box1[2].y(), box1[2].z() };
        A.AxisZ = { box1[3].x(), box1[3].y(), box1[3].z() };

        // set the second obb's properties
        B.Pos = { box2[0].x(), box2[0].y(), box2[0].z() }; // set its center position

        // set the half size
        B.Half_size.x = box2[4].x();
        B.Half_size.y = box2[4].y();
        B.Half_size.z = box2[4].z();

        // set the axes orientation
        B.AxisX = { box2[1].x(), box2[1].y(), box2[1].z() };
        B.AxisY = { box2[2].x(), box2[2].y(), box2[2].z() };
        B.AxisZ = { box2[3].x(), box2[3].y(), box2[3].z() };

        // run the code and get the result as a message
        return getCollision(A, B);
    }

    //PINVOKE inline bool OBBCollide(
    //    float APosX, float APosY, float APosZ,//set the first obb's properties
    //    float AHalfSizeX, float AHalfSizeY, float AHalfSizeZ,//set the half size
    //    float AAxis0X, float AAxis0Y, float AAxis0Z,//set XAxis
    //    float AAxis1X, float AAxis1Y, float AAxis1Z,//set YAxis
    //    float AAxis2X, float AAxis2Y, float AAxis2Z,//set ZAxis
    //
    //    float BPosX, float BPosY, float BPosZ,//set the first obb's properties
    //    float BHalfSizeX, float BHalfSizeY, float BHalfSizeZ,//set the half size
    //    float BAxis0X, float BAxis0Y, float BAxis0Z,//set XAxis
    //    float BAxis1X, float BAxis1Y, float BAxis1Z,//set YAxis
    //    float BAxis2X, float BAxis2Y, float BAxis2Z//set ZAxis
    //
    //
    //) {
    //
    //    //Test
    //    // create two obbs
    //    OBB A, B;
    //
    //    // set the first obb's properties
    //    A.Pos = { APosX, APosY, APosZ }; // set its center position
    //
    //    // set the half size
    //    A.Half_size.x = AHalfSizeX;
    //    A.Half_size.y = AHalfSizeY;
    //    A.Half_size.z = AHalfSizeZ;
    //
    //    // set the axes orientation
    //    A.AxisX = { AAxis0X, AAxis0Y, AAxis0Z };
    //    A.AxisY = { AAxis1X, AAxis1Y, AAxis1Z };
    //    A.AxisZ = { AAxis2X, AAxis2Y, AAxis2Z };
    //
    //    // set the second obb's properties
    //    B.Pos = { BPosX, BPosY, BPosZ }; // set its center position
    //
    //    // set the half size
    //    B.Half_size.x = BHalfSizeX;
    //    B.Half_size.y = BHalfSizeY;
    //    B.Half_size.z = BHalfSizeZ;
    //
    //    // set the axes orientation
    //    B.AxisX = { BAxis0X, BAxis0Y, BAxis0Z };
    //    B.AxisY = { BAxis1X, BAxis1Y, BAxis1Z };
    //    B.AxisZ = { BAxis2X, BAxis2Y, BAxis2Z };
    //
    //    // run the code and get the result as a message
    //    return getCollision(A, B);
    //
    //
    //}
}
