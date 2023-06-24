///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEVELOPER:
// Petras Vestartas, petasvestartas@gmail.com
// Funding: NCCR Digital Fabrication and EPFL
//
// HISTORY:
// 1) The first version was written during the PhD 8928 thesis of Petras Vestartas called:
// Design-to-Fabrication Workflow for Raw-Sawn-Timber using Joinery Solver, 2017-2021
// 2) The translation from C# to C++ was started during the funding of NCCR in two steps
// A - standalone C++ version of the joinery solver and B - integration to COMPAS framework (Python Pybind11)
//
// RESTRICTIONS:
// The code cannot be used for commercial reasons
// If you would like to use or change the code for research or educational reasons,
// please contact the developer first
//
// 3RD PARTY LIBRARIES:
// Boost for reading XML files
//
// INSTRUCTIONS:
// Joint geometry is added as a list of outlines with the last item as a boundary e.g. one line or rectangle
// The number of cut types is equal to the number of outlines + 1 boundary e.g.:
//
//
// ...
// joint.f[0].emplace_back(female_moved_0);
// joint.f[1].emplace_back(female_moved_1);
// joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0].front()[0], joint.f[0].front()[3], joint.f[0].back()[2], joint.f[0].back()[1], joint.f[0].front()[0]});
// joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1].front()[0], joint.f[1].front()[3], joint.f[1].back()[2], joint.f[1].back()[1], joint.f[1].front()[0]});
// joint.f_boolean_type = std::vector<wood_cut::cut_type>(joint.f[0].size(), wood_cut::hole);
// ...
//
// ░░░for multiple holes there is one big hole░░░░for one outline there is one line or one rectangle ░
// ░░░░░░░░░not a copy of every hole░░░░░░░░░░░░░░░░░░░░██████████████████████████████████░░░░░░░░░░░░
// ░░░████████████████████████████████████████░░░░░░░░░░│░░░░░░░░░░┌──────────┐░░░░░░░░░░│░░░░░░░░░░░░
// ░░░█┌────────────────┐░┌─────────────────┐█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
// ░░░█│░░░░░░░░░░░░░░░░│░│░░░░░░░░░░░░░░░░░│█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
// ░░░█└────────────────┘░└─────────────────┘█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
// ░░░████████████████████████████████████████░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░└──────────┘░░░░░░░░░░└──────────┘░░░░░░░░░░░░
// ░░░░░░░░https://fsymbols.com/draw/░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
//
// Also do not forget that there are joint male outlines, there are two list of outlines to represent the top and the bottom outlines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WOOD_JOINT_LIB_H
#define WOOD_JOINT_LIB_H

#include "wood_element.h"
#include "wood_joint.h"

namespace wood_joint_lib
{
    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // xml file reading methods for creating joints
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Check if the file exists in the given path
         *
         * @param [in] name filepath (including the file name)
         * @return true the file exists
         */
        bool exists_file(const std::string &name);

        /**
         * Get joint from XML file using boost library
         * https://zipproth.de/cheat-sheets/cpp-boost/
         *
         * @param [out] name joint geometry in a unit box
         * @param [in] type 0, 12 - ss_e_ip_9, 11,1 - ss_e_op_9, 20,2 - ts_e_p_9, 30,3 - cr_c_ip_9, 40,4 - tt_e_p_9, 13,5 - ss_e_r_9, 60,6 - b_9
         * @return true if the file reading was successful
         */

        bool read_xml(wood::joint &joint, int type = 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in cgal_inscribe_util to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Get length of a 3D vector, taken from the OpenNURBS library: https://github.com/mcneel/opennurbs
         *
         * @param [in] x The first coordinate of the vector
         * @param [in] y The second coordinate of the vector
         * @param [in] z The third coordinate of the vector
         * @return The length of the vector
         */
        double length(double x, double y, double z);

        /**
         * Unitize the vector by dividing its coordinates by its length
         *
         * @param [in, out] vector 3D vector
         * @return true if the vector length is not equal to zero
         */
        bool unitize(IK::Vector_3 &vector);

        /**
         * Remap number from one range to another, the numbers of the intervals must be lower to higher
         *
         * @param [in] value_to_remap number to remap
         * @param [in] low0 interval, where the "value_to_remap" is, lower bound
         * @param [in] high0 interval, where the "value_to_remap" is, higher bound
         * @param [in] low1 interval target
         * @param [in] high1 interval target
         * @return the remapped number
         */
        double remap_numbers(const double &value_to_remap = 50, const double &low0 = 0, const double &high0 = 2000, const double &low1 = 0, const double &high1 = 1);

        /**
         * Linear interpolation between two numbers
         *
         * @param [in] t parameter between 0 and 1
         * @param [in] value0 start of interval
         * @param [in] value1 end of interval
         * @return the interpolated number
         */
        double lerp(const double &value0, const double &value1, const double &t);

        /**
         * Interpolate two points
         *
         * @param [in] from start point
         * @param [in] to end point
         * @param [in] steps divisions
         * @param [in] include_ends end points
         * @param [out] interpolated_points divided points
         */
        void interpolate_points(const IK::Point_3 &from, const IK::Point_3 &to, const int &steps, const bool &include_ends, std::vector<IK::Point_3> &interpolated_points);

        /**
         * Get the transformation matrix for the rotation in 3 axis
         *
         * @param [in] x_axis rotation in x axis
         * @param [in] y_axis rotation in y axis
         * @param [in] z_axis rotation in z axis
         * @param [in] include_ends end points
         * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
         */
        CGAL::Aff_transformation_3<IK> rotation_in_xy_plane(const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Custom joints
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * Unit joint - Custom - remove sides of an element
     * useful for the side of beams when an additional connector is use
     *
     * @param [in, out] jo node, to which geometry will be added
     * @param [in] elements to access properties
     * @param [in] merge_with_joint add additional joint cuts to the male element
     */
    void side_removal(wood::joint &jo, std::vector<wood::element> &elements, bool merge_with_joint = false);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge in-plane joints 1-9
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate finger
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_0(wood::joint &joint);

    /**
     * Unit joint - Side-to-side edge in-plane joints -  plate finger parametric
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_1(wood::joint &joint);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate single butterfly
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_2(wood::joint &joint, std::vector<wood::element> &elements);

        /**
     * Unit joint - Side-to-side edge in-plane joints - beam joints
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_3(wood::joint &joint, std::vector<wood::element> &elements);


        /**
     * Unit joint - Side-to-side edge in-plane joints - beam joints
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_4(wood::joint &joint, std::vector<wood::element> &elements);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_ip_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge out-of-plane joints 10-19
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate finger
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_op_0(wood::joint &joint);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate finger parametric
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_op_1(wood::joint &joint);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate finger parametric
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_op_2(wood::joint &joint);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate miter tenon-mortise
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_op_3(wood::joint &joint);


    /**
     * Unit joint - Side-to-side edge in-plane joints - plate Vidy tenon-mortise with different configurations
     * this joint is connected with methods: ss_e_op_5 and ss_e_op_6
     *
     * @param [in, out] joint node, to which geometry will be added
     * @param [in] t parameter for
     * @param [in] chamfer parameter for eanbling the chamfer
     * @param [in] female_modify_outline parameter for
     * @param [in] x0 parameter for
     * @param [in] x1 parameter for
     * @param [in] y0 parameter for
     * @param [in] y1 parameter for
     * @param [in] z_ext0 parameter for
     * @param [in] z_ext1 parameter for
     * @param [in] z_int0 parameter for
     * @param [in] z_int1 parameter for
     */
    void ss_e_op_4(wood::joint &joint, double t = 0.00, bool chamfer = true, bool female_modify_outline = true, double x0 = -0.50, double x1 = 0.5, double y0 = -0.5, double y1 = 0.5, double z_ext0 = -0.5, double z_ext1 = 0.5);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate Vidy tenon-mortise that has a specific pair linking, therefore the joint has 2 additional methods
     *
     * @param [in, out] jo node, to which geometry will be added
     * @param [in] all_joints existing joints to find the links to add the mortise and join the tenons
     * @param [in] disable_joint_divisions this property is used by "ss_e_op_5" to to merge tenons only with one side, this option is used in the Vidy Theather walls
     */
    void ss_e_op_5(wood::joint &jo, std::vector<wood::joint> &all_joints, bool disable_joint_divisions = false);

    /**
     * Unit joint - Side-to-side edge in-plane joints - plate Vidy tenon-mortise that has a specific pair linking, therefore the joint has 2 additional methods
     * Vidy Theater Wall version, where only one link is used due to the assembly sequence
     *
     * @param [in, out] joint node, to which geometry will be added
     * @param [in] all_joints existing joints to find the links to add the mortise and join the tenons
     */
    void ss_e_op_6(wood::joint &jo, std::vector<wood::joint> &all_joints);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_op_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge rotated joints 20-29
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Side-to-side edge rotated joints - bounding rectangles around each elemenet side
     *
     * @param [in, out] jo node, to which geometry will be added
     * @param [in] elements to get additional properties only stored inside elements
     */
    void ss_e_r_0(wood::joint &jo, std::vector<wood::element> &elements);

    /**
     * Unit joint - Side-to-side edge rotated joints - concical joints that were used in ZAZ exhibition
     *
     * @param [in, out] joint node, to which geometry will be added
     * @param [in] type there are two types of joints, type 1 and the default of hard-coded geometries
     */
    void ss_e_r_1(wood::joint &joint, int type = 1);

    /**
     * Unit joint - Side-to-side edge rotated joints - WARNING: unknwon
     *
     * @param [in, out] joint node, to which geometry will be added
     * @param [in] elements properties for the two elements
     * @param [in] merge_with_joint WARNING: unknwon
     */
    void side_removal_ss_e_r_1(wood::joint &jo, std::vector<wood::element> &elements, bool merge_with_joint = false);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ss_e_r_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Top-to-side edge plane joints 20-29
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Top-to-side edge plane joints - tenon-mortise joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ts_e_p_0(wood::joint &joint);

    /**
     * Unit joint - Top-to-side edge plane joints - tenon-mortise joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ts_e_p_2(wood::joint &joint);

    /**
     * Unit joint - Top-to-side edge plane joints - tenon-mortise joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ts_e_p_3(wood::joint &joint);
    /**
     * Unit joint - Top-to-side edge plane joints - tenon-mortise joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ts_e_p_4(wood::joint &joint);

    /**
     * Unit joint - Top-to-side edge plane joints - snap-fit joints
     *
     * @param [in, out] joint node, to which geometry will be added
     * @param [in] elements elements are needed to determina the joint thickness, since two plates can have different thicknesses
     */
    void ts_e_p_5(wood::joint &joint);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void ts_e_p_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Cross cutting in-plane joints 30-39
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Cross cutting in-plane joints - plate rectangle cuts
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_0(wood::joint &joint);

    /**
     * Unit joint - Cross cutting in-plane joints - beam conical cuts (nine cuts)
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_1(wood::joint &joint);
    /**
     * Unit joint - Cross cutting in-plane joints - beam simplified conical cuts from the Brussels Sports Tower project (5 cuts and no drilling joles)
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_2(wood::joint &joint);

        /**
     * Unit joint - Cross cutting in-plane joints - beam simplified conical cuts from the Brussels Sports Tower project (5 cuts and two inclined drilling joles (1:5 brussels prototype))
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_3(wood::joint &joint);

        /**
     * Unit joint - Cross cutting in-plane joints - beam simplified conical cuts from the Brussels Sports Tower project (5 cuts and 1 vertical drilling hole)
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_4(wood::joint &joint);

        /**
     * Unit joint - Cross cutting in-plane joints - beam simplified conical cuts from the Brussels Sports Tower project (5 cuts and 1 vertical drilling hole one perpendicular to create the dowel-nutl)
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_5(wood::joint &joint);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void cr_c_ip_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Top-to-top edge plane joints 40-49
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Unit joint - Top-to-top edge plane joints - plate drill for one line segment at the center of the intersection area
     * types: drill
     * center of the joint_area
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     */
    void tt_e_p_0(wood::joint &joint, std::vector<wood::element> &elements);

    /**
     * Unit joint - Top-to-top edge plane joints - plate drill in an inscribed circle
     * types: drill
     * center of the joint_area using the polylabel algorithm
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     * @param elements for the line length by element thickness, also could be done in two steps: a) first line is added, then b) intersection is computed
     */
    void tt_e_p_1(wood::joint &joint, std::vector<wood::element> &elements);
    /**
     * Unit joint - Top-to-top edge plane joints - plate drill in an inscribed rectangle
     * types: drill
     * circle of points using the polylabel algorithm
     * UI needed for the number of divisions and radius scale
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     * @param elements for the line length by element thickness, also could be done in two steps: a) first line is added, then b) intersection is computed
     */
    void tt_e_p_2(wood::joint &joint, std::vector<wood::element> &elements);

    /**
     * Unit joint - Top-to-top edge plane joints - plate drill in an inscribed line and shifted
     * types: drill
     * offset of the polygon
     * UI needed for the number of divisions and offset distance
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     * @param elements for the line length by element thickness, also could be done in two steps: a) first line is added, then b) intersection is computed
     */
    void tt_e_p_3(wood::joint &joint, std::vector<wood::element> &elements);

    /**
     * Unit joint - Top-to-top edge plane joints - plate drill the rectangle or cropped grid
     * types: drill
     * Hatch of grid points in the intersection area
     * UI needed for the number of divisions and offset distance
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     * @param elements for the line length by element thickness, also could be done in two steps: a) first line is added, then b) intersection is computed
     */
    void tt_e_p_4(wood::joint &joint, std::vector<wood::element> &elements);

    /**
     * Unit joint - Top-to-top edge plane joints - plate drill in the offset of the polygon
     * types: drill
     * Grid of points in the rectangle inscribed in the intersection area
     * UI needed for the number of divisions and offset distance
     *
     * top-to-top joints have inconsistant connection area, therefore:
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     *
     * @param joint node, to which geometry will be added
     * @param elements for the line length by element thickness, also could be done in two steps: a) first line is added, then b) intersection is computed
     */
    void tt_e_p_5(wood::joint &joint, std::vector<wood::element> &elements);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void tt_e_p_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Boundary joint 60-69
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Boundary joint for cutting one element side, this joint has no other element to connect to
     *
     * @param joint node, to which geometry will be added
     */
    void b_0(wood::joint &joint);

     /**
     * Unit joint - Custom Joint
     *
     * @param [in, out] joint node, to which geometry will be added
     */
    void b_custom(wood::joint &joint);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Key function that iterates through all joints and calls the function that constructs the joint geometry
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Get joint geometry for all joints
     *
     * @param elements all elements as plates without holes
     * @param joints all detected joints
     * @param default_parameters_for_four_types user given 3 parameters per joint type
     * @param scale joint volume scale values, for example triangle plates requires offset from edge ends
     */
    void construct_joint_by_index(std::vector<wood::element> &elements, std::vector<wood::joint> &joints, std::vector<double> &default_parameters_for_four_types, std::vector<double> &scale);
}

#endif // WOOD_JOINT_LIB_H