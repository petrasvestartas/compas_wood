#pragma once
#include "cgal.h"
#include <string>
#include <sstream>

enum cut_types : char {
    nothing = '0',

    //plates
    edge_insertion = '1',
    hole = '2',
    insert_between_multiple_edges = '3',

    //beams
    slice = '4', //project and make rectangle
    mill = '5',
    mill_project = '6', //project
    cut = '7',
    cut_project = '8',		//project
    binary_slice_mill = '9' //project and make rectangle
};

//Container for cuts
class joint {
    //Methods

    bool change_basis(CGAL_Polyline& rect0, CGAL_Polyline& rect1, CGAL::Aff_transformation_3<IK>& xform); //first get 2x ChangeBasis transformation matrices

public:
    /////////////////////////////////////////////////////////////////////////////////////////
    //Parameters from Search method v-volume f-face
    /////////////////////////////////////////////////////////////////////////////////////////
    int id, v0, v1, f0_0, f1_0, f0_1, f1_1, type; //10 - SS Rotate 11 - SS OUT OF PLANE 12 - SS IN Plane,  20 Top-Side, 30 - Cross
    CGAL_Polyline joint_area;					  //delete
    CGAL_Polyline joint_lines[2];				  //delete
    //CGAL_Polyline joint_quads[2];//delete
    CGAL_Polyline joint_volumes[4]; //mostly 2, but can be 4 e.g. in-plane side-side

    /////////////////////////////////////////////////////////////////////////////////////////
    //Detailed parameters for geometry transfer from library or custom made
    /////////////////////////////////////////////////////////////////////////////////////////
    std::string name = "";
    int id_of_global_joint_list = -1;	 //Directs which joint applies where, -1 all cases
    std::vector<double> tile_parameters; //For rebuilding

    std::array< std::vector<CGAL_Polyline>, 2> m;
    std::vector<char> m_boolean_type; //0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

    std::array < std::vector<CGAL_Polyline>, 2> f;
    std::vector<char> f_boolean_type; //0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

    //if this property is enable, joint volume rectangles are moved within unit_scale_distance, this property is equal to first element thickness
    bool unit_scale = false;
    double unit_scale_distance = 0;
    bool orient = true;
    std::array<double, 3> scale = { 1,1,1 };

    //Geometrical divisions
    double shift = 0.5;
    int divisions = 1;
    double length = 1;
    bool compute_geometrical_divisions = true;

    /////////////////////////////////////////////////////////////////////////////////////////
    //Constructors
    /////////////////////////////////////////////////////////////////////////////////////////
    joint();
    joint(int, int, int, int, int, int, int, std::array<CGAL_Polyline, 4>&);
    joint(int, int, int, int, int, int, int, CGAL_Polyline(&), std::array<CGAL_Polyline, 2>&, std::array<CGAL_Polyline, 4>&, int);

    //Operators
    // IK::Vector_3(&input)[4]
    std::vector<CGAL_Polyline>& operator()(bool male_or_female, bool first_or_second) {
        if (male_or_female) {
            if (first_or_second)
                return m[0];
            else
                return m[1];
        }
        else {
            if (first_or_second)
                return f[0];
            else
                return f[1];
        }
    }

    void get_edge_ids(bool male_or_female, int& fA, int& fB) {
        if (male_or_female) {
            fA = f0_0;
            fB = f0_1;
        }
        else {
            fA = f1_0;
            fB = f1_1;
        }
    }

    char& get_first_cutting_type(bool male_or_female) {
        if (male_or_female) {
            return m_boolean_type[0];
        }
        else {
            return f_boolean_type[0];
        }
    }

    std::vector<char>& operator()(bool male_or_female) {
        if (male_or_female)
            return m_boolean_type;
        else
            return f_boolean_type;
    }

    void reverse(bool male_or_female) {
        if (male_or_female)
            std::swap(m[0], m[1]);
        else
            std::swap(f[0], f[1]);
    }

    std::string get_key() {
        std::string key_0 = name;
        std::string key_1 = std::to_string((shift + 0.000000001));
        key_1 = key_1.substr(0, key_1.find(".") + 3);
        std::string key_2 = std::to_string((divisions + 0.000000001));
        key_2 = key_2.substr(0, key_2.find(".") + 3);
        std::string joining = ";";
        std::string key;

        key.reserve(key_0.size() + joining.size() + key_1.size() + joining.size() + key_2.size() + 1);
        key += key_0;
        key += joining;
        key += key_1;
        key += joining;
        key += key_2;

        //std::stringstream ss;
        //ss << key_0 << key_1 << key_2;
        //std::string key = ss.str();

        //printf("\n%s", key_0);
        //printf("\n%s", key_1);
        //printf("\n%s", key_2);
        //printf("\n%s\n", key.c_str());

        return key;
    }

    void transform(CGAL::Aff_transformation_3<IK>& xform0, CGAL::Aff_transformation_3<IK>& xform1); //Custom user transformation

    bool orient_to_connection_area();	//Orient to connection area if rectangles are set

    void duplicate_geometry(joint&);

    void transfer_geometry(joint&);

    void get_divisions(double& division_distance) {
        if (compute_geometrical_divisions) {
            if (joint_lines[0].size() > 0) {//if joint line exists, we can use that line for the length of the joint
                length = CGAL::squared_distance(joint_lines[0][0], joint_lines[0][1]); // Math.Abs(500);
                //printf("\n EDGE LENGTH\n");
                //CGAL_Debug(length);
                //printf("\n EDGE LENGTH\n");
                //CGAL_Debug(length);
                divisions = (int)std::ceil(length / (division_distance * division_distance));
                //CGAL_Debug(divisions);
                divisions = (int)std::max(1, std::min(100, divisions));
                //CGAL_Debug(divisions);
            }
        }
    }
};

inline joint::joint() {
}

inline joint::joint(int _id, int _v0, int _v1, int _f0_0, int _f1_0, int _f0_1, int _f1_1, std::array<CGAL_Polyline, 4>& _joint_volumes) : id(_id), v0(_v0), v1(_v1), f0_0(_f0_0), f1_0(_f1_0), f0_1(_f0_1), f1_1(_f1_1), type(-1) {
    for (int i = 0; i < 4; i++)
        this->joint_volumes[i] = _joint_volumes[i];
}

inline joint::joint(int _id, int _v0, int _v1, int _f0_0, int _f1_0, int _f0_1, int _f1_1, CGAL_Polyline(&_joint_area), std::array<CGAL_Polyline, 2>& _joint_lines, std::array<CGAL_Polyline, 4>& _joint_volumes, int _type) : id(_id), v0(_v0), v1(_v1), f0_0(_f0_0), f1_0(_f1_0), f0_1(_f0_1), f1_1(_f1_1), type(_type) {
    for (int i = 0; i < 4; i++)
        this->joint_volumes[i] = _joint_volumes[i];

    for (int i = 0; i < 2; i++)
        this->joint_lines[i] = _joint_lines[i];

    this->joint_area = _joint_area;
}

//Methods

//https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp //Line 2153
inline bool joint::change_basis(CGAL_Polyline& rect0, CGAL_Polyline& rect1, CGAL::Aff_transformation_3<IK>& xform) {
    IK::Point_3 O1(-0.5, -0.5, -0.5);
    IK::Vector_3 X1(1, 0, 0); // final frame X (X,Y,Z = arbitrary basis)
    IK::Vector_3 Y1(0, 1, 0); // final frame Y
    IK::Vector_3 Z1(0, 0, 1); // final frame Z

    IK::Point_3 O0 = rect0[0];
    IK::Vector_3 X0 = (rect0[1] - rect0[0]); // initial frame X (X,Y,Z = arbitrary basis)
    IK::Vector_3 Y0 = (rect0[3] - rect0[0]); // initial frame Y
    IK::Vector_3 Z0 = (rect1[0] - rect0[0]); // initial frame Z

    // Q = a0*X0 + b0*Y0 + c0*Z0 = a1*X1 + b1*Y1 + c1*Z1
    // then this transform will map the point (a0,b0,c0) to (a1,b1,c1)

    //*this = ON_Xform::ZeroTransformation;

    double a, b, c, d;
    a = X1 * Y1;
    b = X1 * Z1;
    c = Y1 * Z1;
    double R[3][6] = { {X1 * X1, a, b, X1 * X0, X1 * Y0, X1 * Z0},
                      {a, Y1 * Y1, c, Y1 * X0, Y1 * Y0, Y1 * Z0},
                      {b, c, Z1 * Z1, Z1 * X0, Z1 * Y0, Z1 * Z0} };

    // row reduce R
    int i0 = (R[0][0] >= R[1][1]) ? 0 : 1;
    if (R[2][2] > R[i0][i0])
        i0 = 2;
    int i1 = (i0 + 1) % 3;
    int i2 = (i1 + 1) % 3;
    if (R[i0][i0] == 0.0)
        return false;
    d = 1.0 / R[i0][i0];
    R[i0][0] *= d;
    R[i0][1] *= d;
    R[i0][2] *= d;
    R[i0][3] *= d;
    R[i0][4] *= d;
    R[i0][5] *= d;
    R[i0][i0] = 1.0;
    if (R[i1][i0] != 0.0) {
        d = -R[i1][i0];
        R[i1][0] += d * R[i0][0];
        R[i1][1] += d * R[i0][1];
        R[i1][2] += d * R[i0][2];
        R[i1][3] += d * R[i0][3];
        R[i1][4] += d * R[i0][4];
        R[i1][5] += d * R[i0][5];
        R[i1][i0] = 0.0;
    }
    if (R[i2][i0] != 0.0) {
        d = -R[i2][i0];
        R[i2][0] += d * R[i0][0];
        R[i2][1] += d * R[i0][1];
        R[i2][2] += d * R[i0][2];
        R[i2][3] += d * R[i0][3];
        R[i2][4] += d * R[i0][4];
        R[i2][5] += d * R[i0][5];
        R[i2][i0] = 0.0;
    }

    if (fabs(R[i1][i1]) < fabs(R[i2][i2])) {
        int i = i1;
        i1 = i2;
        i2 = i;
    }
    if (R[i1][i1] == 0.0)
        return false;
    d = 1.0 / R[i1][i1];
    R[i1][0] *= d;
    R[i1][1] *= d;
    R[i1][2] *= d;
    R[i1][3] *= d;
    R[i1][4] *= d;
    R[i1][5] *= d;
    R[i1][i1] = 1.0;
    if (R[i0][i1] != 0.0) {
        d = -R[i0][i1];
        R[i0][0] += d * R[i1][0];
        R[i0][1] += d * R[i1][1];
        R[i0][2] += d * R[i1][2];
        R[i0][3] += d * R[i1][3];
        R[i0][4] += d * R[i1][4];
        R[i0][5] += d * R[i1][5];
        R[i0][i1] = 0.0;
    }
    if (R[i2][i1] != 0.0) {
        d = -R[i2][i1];
        R[i2][0] += d * R[i1][0];
        R[i2][1] += d * R[i1][1];
        R[i2][2] += d * R[i1][2];
        R[i2][3] += d * R[i1][3];
        R[i2][4] += d * R[i1][4];
        R[i2][5] += d * R[i1][5];
        R[i2][i1] = 0.0;
    }

    if (R[i2][i2] == 0.0)
        return false;
    d = 1.0 / R[i2][i2];
    R[i2][0] *= d;
    R[i2][1] *= d;
    R[i2][2] *= d;
    R[i2][3] *= d;
    R[i2][4] *= d;
    R[i2][5] *= d;
    R[i2][i2] = 1.0;
    if (R[i0][i2] != 0.0) {
        d = -R[i0][i2];
        R[i0][0] += d * R[i2][0];
        R[i0][1] += d * R[i2][1];
        R[i0][2] += d * R[i2][2];
        R[i0][3] += d * R[i2][3];
        R[i0][4] += d * R[i2][4];
        R[i0][5] += d * R[i2][5];
        R[i0][i2] = 0.0;
    }
    if (R[i1][i2] != 0.0) {
        d = -R[i1][i2];
        R[i1][0] += d * R[i2][0];
        R[i1][1] += d * R[i2][1];
        R[i1][2] += d * R[i2][2];
        R[i1][3] += d * R[i2][3];
        R[i1][4] += d * R[i2][4];
        R[i1][5] += d * R[i2][5];
        R[i1][i2] = 0.0;
    }

    CGAL::Aff_transformation_3<IK> m_xform(
        R[0][3], R[0][4], R[0][5],
        R[1][3], R[1][4], R[1][5],
        R[2][3], R[2][4], R[2][5]);
    //m_xform.cartesian(0,0) = R[0][3];
    //m_xform[0][1] = R[0][4];
    //m_xform[0][2] = R[0][5];

    //m_xform[1][0] = R[1][3];
    //m_xform[1][1] = R[1][4];
    //m_xform[1][2] = R[1][5];

    //m_xform[2][0] = R[2][3];
    //m_xform[2][1] = R[2][4];
    //m_xform[2][2] = R[2][5];

    CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O1.x(), 0 - O1.y(), 0 - O1.z()));
    CGAL::Aff_transformation_3<IK> T2(CGAL::TRANSLATION, IK::Vector_3(O0.x(), O0.y(), O0.z()));

    xform = (T2 * m_xform * T0);

    return true;
}

inline void joint::transform(CGAL::Aff_transformation_3<IK>& xform0, CGAL::Aff_transformation_3<IK>& xform1) {
    for (int i = 0; i < m[0].size(); i++)
        for (auto it = m[0][i].begin(); it != m[0][i].end(); ++it)
            *it = it->transform(xform0);

    for (int i = 0; i < m[1].size(); i++)
        for (auto it = m[1][i].begin(); it != m[1][i].end(); ++it)
            *it = it->transform(xform0);

    for (int i = 0; i < f[0].size(); i++)
        for (auto it = f[0][i].begin(); it != f[0][i].end(); ++it)
            *it = it->transform(xform1);

    for (int i = 0; i < f[1].size(); i++)
        for (auto it = f[1][i].begin(); it != f[1][i].end(); ++it)
            *it = it->transform(xform1);
}

inline bool joint::orient_to_connection_area() {
    //CGAL_Debug(thickness);
    //Change the distance between two rectangles
    //CGAL_Debug(unit_scale);
    //CGAL_Debug(unit_scale_distance);
    if (unit_scale && unit_scale_distance > 0) {//
        IK::Segment_3 volume_segment(joint_volumes[0][0], joint_volumes[1][0]);
        IK::Vector_3 vec = volume_segment.to_vector() * 0.5;
        IK::Vector_3 vec_unit = volume_segment.to_vector();
        cgal_vector_util::Unitize(vec_unit);
        vec_unit *= (unit_scale_distance * 0.5);
        //CGAL_Debug(thickness);
        cgal_polyline_util::move(joint_volumes[0], vec);
        cgal_polyline_util::move(joint_volumes[1], -vec);
        cgal_polyline_util::move(joint_volumes[0], -vec_unit);
        cgal_polyline_util::move(joint_volumes[1], vec_unit);

        if (joint_volumes[2].size() > 0) {
            IK::Segment_3 volume_segment(joint_volumes[2][0], joint_volumes[3][0]);
            vec = volume_segment.to_vector() * 0.5;
            vec_unit = volume_segment.to_vector();
            cgal_vector_util::Unitize(vec_unit);
            vec_unit *= (unit_scale_distance * 0.5);
            //CGAL_Debug(thickness);
            cgal_polyline_util::move(joint_volumes[2], vec);
            cgal_polyline_util::move(joint_volumes[3], -vec);
            cgal_polyline_util::move(joint_volumes[2], -vec_unit);
            cgal_polyline_util::move(joint_volumes[3], vec_unit);
        }
    }

    CGAL::Aff_transformation_3<IK> xform0;
    bool flag0 = change_basis(joint_volumes[0], joint_volumes[1], xform0);

    CGAL::Aff_transformation_3<IK> xform1;
    bool flag1 = joint_volumes[2].size() > 0 ? change_basis(joint_volumes[2], joint_volumes[3], xform1) : change_basis(joint_volumes[0], joint_volumes[1], xform1);

    transform(xform0, xform1);

    return flag0 && flag1;
}

inline void joint::duplicate_geometry(joint& new_joint) {
    //Transfer geometrical information from current to new_joint
    //Duplicate is needed to store geometrical data, not adjacency, to reduce creation time
    new_joint.name = this->name;
    new_joint.shift = this->shift;
    new_joint.divisions = this->divisions;
    new_joint.length = this->length;

    new_joint.m = this->m;
    new_joint.f = this->f;
    new_joint.m_boolean_type = this->m_boolean_type;
    new_joint.f_boolean_type = this->f_boolean_type;
    new_joint.unit_scale = this->unit_scale;
    new_joint.orient = this->orient;

    /* for (int i = 0; i < m.size(); i++) {
         for (int j = 0; j < m[i].size(); j++) {
             new_joint.m[i].emplace_back(CGAL_Polyline());
             new_joint.m[i].back().reserve(this->m[i].size());
             for (auto& p : this->m[i][j])
                 new_joint.m[i].back().emplace_back(p);
         }
     }

     for (int i = 0; i < f.size(); i++) {
         for (int j = 0; j < f[i].size(); j++) {
             new_joint.f[i].emplace_back(CGAL_Polyline());
             new_joint.f[i].back().reserve(this->f[i].size());
             for (auto& p : this->f[i][j])
                 new_joint.f[i].back().emplace_back(p);
         }
     }

     new_joint.m_boolean_type.reserve(m_boolean_type.size());
     for (size_t i = 0; i < m_boolean_type.size(); i++)
         new_joint.m_boolean_type.emplace_back(this->m_boolean_type[i]);

     new_joint.f_boolean_type.reserve(f_boolean_type.size());
     for (size_t i = 0; i < f_boolean_type.size(); i++)
         new_joint.f_boolean_type.emplace_back(this->f_boolean_type[i]);*/
}

inline void joint::transfer_geometry(joint& geo_joint) {
    //Transfer geometrical information from geo_joint to this joint
    this->name = geo_joint.name;
    this->shift = geo_joint.shift;
    this->divisions = geo_joint.divisions;
    this->length = geo_joint.length;
    this->m = geo_joint.m;
    this->f = geo_joint.f;
    this->m_boolean_type = geo_joint.m_boolean_type;
    this->f_boolean_type = geo_joint.f_boolean_type;
    this->unit_scale = geo_joint.unit_scale;
    this->orient = geo_joint.orient;
    /*for (int i = 0; i < geo_joint.m.size(); i++) {
        for (int j = 0; j < geo_joint.m[i].size(); j++) {
            this->m[i].emplace_back(CGAL_Polyline());
            this->m[i].back().reserve(geo_joint.m[i].size());
            for (auto& p : geo_joint.m[i][j])
                this->m[i].back().emplace_back(p);
        }
    }

    for (int i = 0; i < geo_joint.f.size(); i++) {
        for (int j = 0; j < geo_joint.f[i].size(); j++) {
            this->f[i].emplace_back(CGAL_Polyline());
            this->f[i].back().reserve(geo_joint.f[i].size());
            for (auto& p : geo_joint.f[i][j])
                this->f[i].back().emplace_back(p);
        }
    }

    this->m_boolean_type.reserve(geo_joint.m_boolean_type.size());
    for (size_t i = 0; i < geo_joint.m_boolean_type.size(); i++)
        this->m_boolean_type.emplace_back(geo_joint.m_boolean_type[i]);

    this->f_boolean_type.reserve(geo_joint.f_boolean_type.size());
    for (size_t i = 0; i < geo_joint.f_boolean_type.size(); i++)
        this->f_boolean_type.emplace_back(geo_joint.f_boolean_type[i]);*/
}
