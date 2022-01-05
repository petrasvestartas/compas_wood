#pragma once
//#include "compas.h"
#include <cmath>

namespace CGAL_MathUtil {

	inline int count_digits(double number) {

		int n = std::ceil(std::abs(number));


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
		} else {
			return (uint64_t)low << 32 | high;
		}


	}

}