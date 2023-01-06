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
// None
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_MATH_UTIL_H
#define CGAL_MATH_UTIL_H


namespace cgal_math_util
{

    /**
     * get the length of the opposite ortho edge
     *
     * ////////////////////////////
     * /////////// ////////////////
     * /////////// / //////////////
     * /////////// // /////////////
     * /////////// /// ////////////
     * /////////// an / ///////////
     * // edge /// gle / //////////
     * /// len /// ////// /////////
     * /////////// /////// ////////
     * /////////// //////// ///////
     * /////////// ///////// //////
     * ///////////            /////
     * ///////////// x = ? ////////
     *
     * @param [in] edge_length target plane origin
     * @param [in] angle_degrees target plane x_axis
     * @return length of the opposite ortho edge
     */
     double triangle_edge_by_angle(const double &edge_length, const double &angle_degrees);

    /**
     * convert radians to degrees
     *
     * @param [in] radians angle in radians
     * @return degrees
     */
     double rad_to_deg(const double &radians);

    /**
     * convert degrees to radians
     *
     * @param [in] degrees angle in degrees
     * @return radians
     */
     double deg_to_rad(const double &degrees);

    /**
     * get number of digits in a real number
     *
     * @param [in] number numeric value that will be rounded
     * @return number of digits
     */
     int count_digits(const double &number);

    /**
     * get a unique number from two integers
     * the order does not matter
     *
     * @param [in] number0 numeric value that will be rounded
     * @param [in] number1 numeric value that will be rounded
     * @return number the unsigned integer with values ranging from 0 to 18,446,744,073,709,551,615
     */
     uint64_t unique_from_two_int(const int &number0, const int &number1);

    /**
     * get next or prev number, when the total count is known
     * in c++ consider using std::prev() and std::next()
     *
     * @param [in] index integer from 0 to n
     * @param [in] n total number indices
     * @return next or prev number
     */
     int wrap(const int &index, const int &n);
}

#endif
