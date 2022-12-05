# Install script for directory: C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/AdolcForward"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/AlignedVector3"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/ArpackSupport"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/AutoDiff"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/BVH"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/EulerAngles"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/FFT"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/IterativeSolvers"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/KroneckerProduct"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/LevenbergMarquardt"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/MatrixFunctions"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/MoreVectorization"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/MPRealSupport"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/NonLinearOptimization"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/NumericalDiff"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/OpenGLSupport"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/Polynomials"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/Skyline"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/SparseExtra"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/SpecialFunctions"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/Splines"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/eigen-prefix/src/eigen-build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

