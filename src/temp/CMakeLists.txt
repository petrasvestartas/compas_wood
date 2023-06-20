
cmake_minimum_required(VERSION 3.24.2)
project(MyCGALProject LANGUAGES C CXX)


# Find CGAL package
find_package(CGAL REQUIRED)

# Set C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

# Add your source files
add_executable(MyCGALExecutable main.cpp)

# Link CGAL library to your executable
target_link_libraries(MyCGALExecutable PRIVATE CGAL::CGAL)