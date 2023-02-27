
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_math_util.h"

namespace cgal_math_util
{

    double triangle_edge_by_angle(const double &edge_length, const double &angle_degrees)
    {
        return edge_length * std::tan(angle_degrees * 3.14159265358979323846 / 180.0);
    }

    double rad_to_deg(const double &radians)
    {
        return radians * 180.0 / 3.14159265358979323846;
    }

    double deg_to_rad(const double &degrees)
    {
        return degrees * 3.14159265358979323846 / 180.0;
    }

    int count_digits(const double &number)
    {
        int n = (int)std::ceil(std::abs(number));

        int count = 0;
        while (n != 0)
        {
            n = n / 10;
            ++count;
        }
        return count;
    }

    uint64_t unique_from_two_int(const int &number0, const int &number1)
    {
        if (number1 > number0)
            return (uint64_t)number1 << 32 | number0;
        else
            return (uint64_t)number0 << 32 | number1;
    }

    int wrap(const int &index, const int &n)
    {
        if (n == 0)
            return 0;
        return ((index % n) + n) % n;
    }
}
