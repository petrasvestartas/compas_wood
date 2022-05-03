// cpp_console_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <string>

enum Direction {
    A,
    B
};

std::ostream& operator<<(std::ostream& out, const Direction value) {
    static std::map<Direction, std::string> strings;
    if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(A);
        INSERT_ELEMENT(B);
#undef INSERT_ELEMENT
    }

    return out << strings[value];
}


int main()
{
    int a = 0;
    Direction e = static_cast<Direction>(a);
    std::cout << e;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
