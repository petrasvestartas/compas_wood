#pragma once

#include "stdafx.h"


namespace cgal_math_util {
    inline int count_digits(double number) {
        int n = (int)std::ceil(std::abs(number));

        int count = 0;
        while (n != 0) {
            n = n / 10;
            ++count;
        }
        return count;
    }

    inline uint64_t unique_from_two_int(int& low, int& high) {
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
