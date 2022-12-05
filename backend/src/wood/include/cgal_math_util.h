#pragma once

//#include "../../stdafx.h"


namespace cgal_math_util {

    ////////////////////////////
    /////////// ////////////////
    /////////// / //////////////
    /////////// // /////////////
    /////////// /// ////////////
    /////////// an / ///////////
    // edge /// gle / //////////
    //// len // ////// /////////
    /////////// /////// ////////
    /////////// //////// ///////
    /////////// ///////// //////
    ///////////            /////
    ///////////// x = ? ////////
    
    inline double triangle_edge_by_angle(const double& edge_length, const double& angle_degrees) {

        return edge_length * std::tan(angle_degrees * 3.14159265358979323846 / 180.0);
    }


    inline double rad_to_deg(const double& radians) {
        return radians * 180.0 / 3.14159265358979323846;
    }
    
    inline double deg_to_rad(const double& deg) {
        return deg * 3.14159265358979323846 / 180.0;
    }

    
    inline int count_digits(double number) {
        int n = (int)std::ceil(std::abs(number));

        int count = 0;
        while (n != 0) {
            n = n / 10;
            ++count;
        }
        return count;
    }

    inline uint64_t unique_from_two_int(const int& low, const int& high) {
        if (high > low) {
            return (uint64_t)high << 32 | low;
        }
        else {
            return (uint64_t)low << 32 | high;
        }
    }

    inline int wrap(const int& index, const int& n) {
        if (n == 0)
            return 0;
        return ((index % n) + n) % n;
    }
}
