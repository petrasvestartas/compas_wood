//#include "../../stdafx.h"

template <typename T>
void print_debug(const T &s)
{
	std::cout << "debug.h print method: " << s << "\n";
}

template <typename T>
void print_debug(const T &x, const T &y)
{
	std::cout << "debug.h print method: " << x << " " << y << "\n";
}

template <typename T>
void print_debug(const T &x, const T &y, const T &z)
{
	std::cout << "debug.h print method: " << x << " " << y << " " << z << "\n";
}

template <typename T>
void print_debug(const T &x, const T &y, const T &z, const T &w)
{
	std::cout << "debug.h print method: " << x << " " << y << " " << z << " " << w << "\n";
}
