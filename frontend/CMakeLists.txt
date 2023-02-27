
########################################################################
# PROPERTIES
########################################################################
cmake_minimum_required(VERSION 3.24.2)

project(wood_pybind11 LANGUAGES C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
cmake_policy(SET CMP0097 NEW)
# Set global property (all targets are impacted)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# debug
set(RELEASE_DEBUG "" CACHE STRING "DEBUG to true ")
if(RELEASE_DEBUG)
  message(AUTHOR_WARNING "RELEASE IS BEING CONFIGURED" )
  set(CMAKE_BUILD_TYPE_INIT "Release")
else()
message(AUTHOR_WARNING "DEBUG IS BEING CONFIGURED" )
  set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
endif()


########################################################################
#
# START SUPERBUILD - DOWNLOAD LIBRARIES FROM GITHUB REPOSIROTRIES
#
########################################################################
cmake_policy(SET CMP0135 NEW)
include(ExternalProject)

SET(GET_LIBS "" CACHE STRING "Set option to download dependencies")
if (GET_LIBS)
  message(AUTHOR_WARNING "GET_LIBS_" ${GET_LIBS})

  #######################################################################
  # CLIPPER2
  ####################################################################### 
  ExternalProject_Add(clipper_2
      GIT_REPOSITORY https://github.com/AngusJohnson/Clipper2.git
      GIT_TAG main
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        #-DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/install"
      SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/clipper_2"
        #INSTALL_DIR   "${CMAKE_INSTALL_PREFIX}/install"
      CONFIGURE_COMMAND "" #do not configure
      BUILD_COMMAND "" #do not buld
      INSTALL_COMMAND "" #installer for now is empty
  )

  ########################################################################
  # CGAL
  ######################################################################## 

  ExternalProject_Add(cgal
    URL https://github.com/CGAL/cgal/releases/download/v5.5.1/CGAL-5.5.1-library.zip
    #GIT_REPOSITORY https://github.com/CGAL/cgal.git
    #GIT_TAG v5.4.1
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      #-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/cgal"
    BUILD_ALWAYS "" #do not buld
    INSTALL_COMMAND "" #installer for now is empty
  )


  ExternalProject_Add(cgal_dependencies
    URL https://github.com/CGAL/cgal/releases/download/v5.5/CGAL-5.5-win64-auxiliary-libraries-gmp-mpfr.zip
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      #-DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/install"
    SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/cgal/auxiliary"
      #INSTALL_DIR   "${CMAKE_INSTALL_PREFIX}/install"
    CONFIGURE_COMMAND "" #do not configure
    BUILD_COMMAND "" #do not buld
    INSTALL_COMMAND "" #installer for now is empty
  )


  ########################################################################
  # BOOST
  ######################################################################## 
  if (WIN32)
    set(BOOST_BOOTSTRAP_CMD ${CMAKE_CURRENT_BINARY_DIR}/boost-prefix/src/boost/bootstrap.bat)
    set(BOOST_BUILD_CMD ${CMAKE_CURRENT_BINARY_DIR}/boost-prefix/src/boost/b2.exe)
  elseif (UNIX)
    set(BOOST_BOOTSTRAP_CMD ${CMAKE_CURRENT_BINARY_DIR}/boost-prefix/src/boost/bootstrap.sh)
    set(BOOST_BUILD_CMD ${CMAKE_CURRENT_BINARY_DIR}/boost-prefix/src/boost/b2)
  endif ()

  ExternalProject_Add(boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.78.0
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_CMD}
    BUILD_COMMAND
      ${BOOST_BUILD_CMD}
      --prefix=${CMAKE_BINARY_DIR}/install/boost
      -sZLIB_LIBRARY_PATH=${CMAKE_BINARY_DIR}/install/boost/lib
      --without-python
      address-model=64
      variant=debug,release
      link=shared
      runtime-link=shared
      threading=multi
      install
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    #SOURCE_DIR "${CMAKE_BINARY_DIR}/install/boost" #install directory is in build/install/eigen
  )

  ########################################################################
  # EIGEN
  ######################################################################## 
  ExternalProject_Add(eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/eigen #has not effect
    SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/eigen" #install directory is in build/install/eigen
    BUILD_COMMAND "" #do not build
    INSTALL_COMMAND "" #do not install
  )

  ########################################################################
  # PYBIND11
  ######################################################################## 

  ExternalProject_Add(pybind11
    URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.3.zip
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/pybind11"
    BUILD_ALWAYS "" #do not build
    INSTALL_COMMAND "" #installer for now is empty
  )

endif ()



########################################################################
#
# CONFIGURE THE PROJECT
#
########################################################################

###############################################################################
#
# PYBIND11 MODULE
#
###############################################################################
SET(BUILD_MY_PROJECTS "" CACHE STRING "Build Project")
if (BUILD_MY_PROJECTS)
  message(AUTHOR_WARNING "BUILD_MY_PROJECTS_" ${BUILD_MY_PROJECTS})


  ###############################################################################
  # PCH support
  ###############################################################################
  #option(KDV_BUILD_USING_PCH "Build using pre-compile header support" ON)
  #if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.16 AND KDV_BUILD_USING_PCH) # support for PCHs in CMake was added in 3.16
  message(AUTHOR_WARNING "Compiling using pre-compiled header support")
  target_precompile_headers(${PROJECT_NAME} PRIVATE  stdafx.h) # With PUBLIC they will be used by targets using this target
  #target_precompile_headers(${PROJECT_NAME} PRIVATE   "C:/IBOIS57/_Code/Software/CPP/CMAKE/super_build/compas_wood/build_win/CMakeFiles/compas_wood.dir/Release/cmake_pch.hxx")
  set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)


  ###############################################################################
  # CGAL
  ###############################################################################
  #target_link_libraries(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/" ) # WINDOWS
  if(COMPILER_NINJA)
    target_link_directories(${PROJECT_NAME} PUBLIC "C:/msys64/mingw64/lib/" )
  else()
    target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/" )
  endif()

  ###############################################################################
  # WRAPPER LIBRARY FOR PYBIND11 - https://www.youtube.com/watch?v=-eIkUnCLMFc&t=75s
  # My own created tutorial: https://github.com/pybind/pybind11/discussions/4530
  # PYBIND11 - requires python installation besides the python11
  # WARNING - your conda environment must not have the module with the same name installed, 
  # SOLUTION - check by "conda list" and "pip uninstall compas_wood"
  ###############################################################################
  #give the python path to pybind11
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

  # find python do not use find_package(Python) this would fail if a few python versions are installed, better to use one from Anaconda
  find_package(Python 3.6 REQUIRED COMPONENTS Interpreter Development ${_pybind11_quiet}) 

  # add pybind11 to enamle CMake command: pybind11_add_module
  add_subdirectory("${CMAKE_BINARY_DIR}/install/pybind11")

  # module - this is a special cmake command from the pybind11 package
  pybind11_add_module (
    "${${PROJECT_NAME}}" # name of the module
    #source pybind11
    "${CMAKE_SOURCE_DIR}/src/compas_wood/include/stdafx_pybind11.cpp" # includes
    "${CMAKE_SOURCE_DIR}/src/compas_wood/include/python_to_cpp__cpp_to_python.cpp" # converters
    "${CMAKE_SOURCE_DIR}/src/compas_wood/include/compas_wood.cpp" # wrapper of the wood library
    #clipper
    "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/src/clipper.engine.cpp"
    "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/src/clipper.offset.cpp"
    #wood
    "${CMAKE_SOURCE_DIR}/src/wood_pybind11/include_cpp/wood_globals.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_box_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/rtree_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_inscribe_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_intersection_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_xform_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_math_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_plane_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_polyline_mesh_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_polyline_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_rectangle_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_vector_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/clipper_util.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/wood_element.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/wood_joint.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/wood_joint_lib.cpp"
    # #"${CMAKE_SOURCE_DIR}/src/wood/include/wood_test.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/wood_main.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/wood_xml.cpp"
    # "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_mesh_boolean.cpp"


  )


  # include directories for header files
  target_include_directories(
    "${${PROJECT_NAME}}" # name of the module
    #source pybind11
    PRIVATE "${CMAKE_SOURCE_DIR}/src/wood_pybind11/include_cpp/"
    #wood
    PRIVATE "${CMAKE_SOURCE_DIR}/src/wood/include/"
    #pybind11
    PRIVATE "${CMAKE_BINARY_DIR}/install/pybind11/"
    #eigen
    PRIVATE "${CMAKE_BINARY_DIR}/install/eigen/"
    #clipper
    PRIVATE "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/"
    #boost
    PRIVATE "${CMAKE_BINARY_DIR}/install/boost/include/boost-1_78/"
    #eigen
    PRIVATE "${CMAKE_BINARY_DIR}/install/eigen/"
    #cgal
    PRIVATE "${CMAKE_BINARY_DIR}/install/cgal/include"
    PRIVATE "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/include"
  )

endif()


########################################################################
#
# COMPILE LIBRARIES TO REDUCE BUILD TIME
#
########################################################################
SET(COMPILE_LIBS "" CACHE STRING "Set option to compile dependencies")
if (COMPILE_LIBS)

  ###############################################################################
  # CLIPPER
  ###############################################################################
  set(CLIPPER2_INC
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/clipper2/clipper.core.h"
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/clipper2/clipper.engine.h"
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/clipper2/clipper.h"
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/clipper2/clipper.minkowski.h"
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/clipper2/clipper.offset.h"
 )

  set(CLIPPER2_SRC
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/src/clipper.engine.cpp"
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/src/clipper.offset.cpp"
  )

  add_library(Clipper2 STATIC ${CLIPPER2_INC} ${CLIPPER2_SRC}  )
  target_include_directories(Clipper2  PUBLIC "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/") #link headers to clipper library


  ###############################################################################
  # CGAL
  ###############################################################################
  #target_link_libraries(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/" ) # WINDOWS
  if(COMPILER_NINJA)
    target_link_directories(${PROJECT_NAME} PUBLIC "C:/msys64/mingw64/lib/" )
  else()
    target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/" )
  endif()

  
  if(NOT COMPILER_NINJA)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
      COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
          "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/libgmp-10.dll"      # <--this is in-file
          "${CMAKE_BINARY_DIR}/Release")                 # <--this is out-file path

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
      COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
          "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/lib/libmpfr-4.dll"      # <--this is in-file
          "${CMAKE_BINARY_DIR}/Release")                 # <--this is out-file path
  endif()

 


  message(AUTHOR_WARNING "Part 1 - endif" ${BUILD_MY_PROJECTS})
endif ()