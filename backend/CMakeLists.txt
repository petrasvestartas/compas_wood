########################################################################
# EXAMPLE RUNNING NINJA
# 
# Windows
# https://github.com/ninja-build/ninja/releases
# add ninja to "path" ninja -> C:\Program Files\Ninja\ninja.exe
# restart computer
# run VS code always with aministrative rights
# execute "vcvars64.bat" from C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build 
#
# To install ninja on WSL2/Ubuntu (Linux):
# sudo apt install ninja-build
# If apt does not work, use apt-get.
# 
# To install ninja on macOS using Homebrew.
# brew upgrade
# brew install ninja
# 
# remove long paths
# New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
########################################################################
##### Step 1: Create a build directory and cd into it
# cd C:\IBOIS57\_Code\Software\Python\compas_wood\backend\build_win
##### Step 2: Run CMake to download libraries
# cmake --fresh -DGET_LIBS=ON -DCOMPILE_LIBS=OFF -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 3: Run CMake to build 3rd-party libraries
# cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 4: Run CMake to build the code
# cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 5 - Run the code - you can use this as a default VScode task:
# cmake  --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 4 &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe
########################################################################
#mkdir build && cd build

#cmake   -DGET_LIBS=ON -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release" -G "Ninja" .. && cmake  --build . --config Release
#cmake  --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release" -G "Ninja" .. && cmake  --build . --config Release
#cmake --fresh -DGET_LIBS=ON -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 ..\..\ && cmake --build . --config Release
#cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 ..\..\ && cmake --build . --config Release
#cmake  --build . --config Release -- -j 8
#cmake -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -B release -G "Ninja" -DCMAKE_MAKE_PROGRAM=C:\Ninja\ninja.exe && cmake --build . --config Release
########################################################################
# PROPERTIES
########################################################################
cmake_minimum_required(VERSION 3.24.2)

set(COMPILER_NINJA OFF)

if(COMPILER_NINJA)
 set(CMAKE_MAKE_PROGRAM "C:\\Ninja\\ninja.exe" CACHE FILEPATH "" FORCE)
endif()

project(wood LANGUAGES C CXX)

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
#cmake_policy(SET CMP0097 NEW)
cmake_policy(SET CMP0135 NEW)
include(ExternalProject)



SET(GET_LIBS "" CACHE STRING "Set option to download dependencies")
if (GET_LIBS)
  message(AUTHOR_WARNING "GET_LIBS_" ${GET_LIBS})

  #######################################################################
  # CDT
  ####################################################################### 
  ExternalProject_Add(cdt
  GIT_REPOSITORY    https://github.com/artem-ogre/CDT.git
  GIT_TAG           master
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        #-DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/install"
      SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/cdt/"
        #INSTALL_DIR   "${CMAKE_INSTALL_PREFIX}/install"
      CONFIGURE_COMMAND "" #do not configure
      BUILD_COMMAND "" #do not buld
      INSTALL_COMMAND "" #installer for now is empty
  )

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

  #######################################################################
  #GLFW https://learnopengl.com/Getting-started/Hello-Window
  #######################################################################
  ExternalProject_Add(glfw
  URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX="${CMAKE_BINARY_DIR}/install/glfw"
    #-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    #-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    #-GLFW_BUILD_DOCS = ON
    #-GLFW_BUILD_EXAMPLES = ON
    #-GLFW_BUILD_TESTS = ON
    #-GLFW_INSTALL = ON
    #-USE_MSVC_RUNTIME_LIBRARY_DLL = ON
    #-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
  UPDATE_COMMAND ""
  SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/glfw"
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND "" #installer for now is empty

  #BUILD_ALWAYS ON #do not build
  #INSTALL_COMMAND ON #installer for now is empty
  )

########################################################################
# GLM
######################################################################## 
  ExternalProject_Add(glm
  URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.7z
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    #-DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/install"
  SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/glm"
    #INSTALL_DIR   "${CMAKE_INSTALL_PREFIX}/install"
  CONFIGURE_COMMAND "" #do not configure
  BUILD_COMMAND "" #do not buld
  INSTALL_COMMAND "" #installer for now is empty
)

########################################################################
# GLAD
######################################################################## 
ExternalProject_Add(glad
URL "https://github.com/Dav1dde/glad/archive/v0.1.29.zip"
CMAKE_ARGS 
  -DCMAKE_INSTALL_PREFIX="${CMAKE_BINARY_DIR}/install/glad"
  #-DGLAD_INSTALL=OFF
  -DGLAD_PROFILE="core"
  -DGLAD_GENERATOR="c"
UPDATE_COMMAND ""
SOURCE_DIR "${CMAKE_BINARY_DIR}/install/glad"
#CONFIGURE_COMMAND "" #do not configure
BUILD_IN_SOURCE 1
#BUILD_COMMAND "ON" #do not build
INSTALL_COMMAND "" #installer for now is empty
)




########################################################################
# IMGUI
######################################################################## 
ExternalProject_Add(imgui
URL https://github.com/ocornut/imgui/archive/refs/tags/v1.88.zip
CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  #-DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/install"
SOURCE_DIR   "${CMAKE_BINARY_DIR}/install/imgui"
  #INSTALL_DIR   "${CMAKE_INSTALL_PREFIX}/install"
CONFIGURE_COMMAND "" #do not configure
BUILD_COMMAND "" #do not buld
INSTALL_COMMAND "" #installer for now is empty
)

    #https://stackoverflow.com/questions/68824024/detecting-compiler-configuration-in-cmake-file
    #https://www.microsoft.com/en-us/download/details.aspx?id=6812
    ExternalProject_Add(assimp
    PREFIX assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG master
    CMAKE_ARGS     
    -DCMAKE_INSTALL_PREFIX="${CMAKE_BINARY_DIR}/install/assimp"
    -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT:BOOL=OFF
    -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DASSIMP_BUILD_ASSIMP_TOOLS:BOOL=OFF
    -DASSIMP_BUILD_TESTS:BOOL=OFF
    -DASSIMP_BUILD_FBX_IMPORTER:BOOL=ON
    -DASSIMP_BUILD_OBJ_IMPORTER:BOOL=ON
    -DASSIMP_BUILD_OBJ_EXPORTER:BOOL=ON
    -DASSIMP_BUILD_ZLIB:BOOL=ON
    -DASSIMP_LIBRARY_SUFFIX:STRING=
    -DLIBRARY_SUFFIX:STRING=
    -DCMAKE_DEBUG_POSTFIX:STRING=
    -DASSIMP_INJECT_DEBUG_POSTFIX:BOOL=OFF
        UPDATE_COMMAND ""
        SOURCE_DIR "${CMAKE_BINARY_DIR}/install/assimp"
        #CONFIGURE_COMMAND "" #do not configure
        BUILD_IN_SOURCE 1
        #BUILD_COMMAND "ON" #do not build
        INSTALL_COMMAND "" #installer for now is empty
    )


########################################################################
# CGAL
######################################################################## 
ExternalProject_Add(cgal
  URL https://github.com/CGAL/cgal/releases/download/v5.5/CGAL-5.5.zip
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
# GOOGLETEST ///googletest/include
######################################################################## 
set(GOOGLE_TEST_OS "")
if (WIN32)
    set(GOOGLE_TEST_OS "$<$<PLATFORM_ID:Windows>:-Dgtest_force_shared_crt:BOOL=ON>")
endif (WIN32)

        unset(GTEST_ROOT CACHE)
         unset(GMOCK_ROOT CACHE)
         ExternalProject_Add(googletest
             GIT_REPOSITORY https://github.com/google/googletest.git
             GIT_TAG 58d77fa8070e8cec2dc1ed015d66b454c8d78850
             PREFIX ${CMAKE_BINARY_DIR}/install/googletest
             INSTALL_DIR ${CMAKE_BINARY_DIR}/install/googletest
             CMAKE_ARGS
                 -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
		 ${GOOGLE_TEST_OS} #$<$<PLATFORM_ID:Windows>:-Dgtest_force_shared_crt:BOOL=ON>
             BUILD_COMMAND
                 COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --target install
                # COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Debug --target install
             INSTALL_COMMAND  ""
             )
         set(GTEST_ROOT ${CMAKE_BINARY_DIR}/install/googletest CACHE PATH "" FORCE)
         set(GMOCK_ROOT ${CMAKE_BINARY_DIR}/install/googletest CACHE PATH "" FORCE)


  ###############################################################################
  # CDT
  ###############################################################################
  # set(CDT_INC
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/CDT.h"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/CDT.hpp"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/CDTUtils.h"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/CDTUtils.hpp"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/KDTree.h"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/LocatorKDTree.h"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/predicates.h"
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/remove_at.hpp"
  # )

  # set(CDT_SRC
  # "${CMAKE_BINARY_DIR}/install/cdt/CDT/src/CDT.cpp"
  # )

  # add_library(CDT STATIC ${CDT_INC} ${CDT_SRC}  )
  # target_include_directories(CDT  PUBLIC "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/") #link headers to clipper library
endif ()



########################################################################
#
# CONFIGURE THE PROJECT
#
########################################################################

###############################################################################
#EXECUTABLE LINK HEADER ONLY DIRECTORY
###############################################################################
SET(BUILD_MY_PROJECTS "" CACHE STRING "Build Project")
if (BUILD_MY_PROJECTS)
  message(AUTHOR_WARNING "BUILD_MY_PROJECTS_" ${BUILD_MY_PROJECTS})

  # if (MSVC)
  #   # warning level 4 and all warnings as errors
  #   add_compile_options()#  /Wall /W4/WX
  # else()
  #     # lots of warnings and all warnings as errors
  #     add_compile_options(-Wall -Wextra -pedantic -Werror)
  # endif()


  ###############################################################################
  # CREATE EXE
  ###############################################################################
  add_executable(${PROJECT_NAME} main.cpp 
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_globals.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/rtree_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_inscribe_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_xform_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_math_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_rectangle_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/clipper_util.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_element.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_joint.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_joint_lib.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_test.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_main.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/viewer_wood.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/wood_xml.cpp"
  "${CMAKE_SOURCE_DIR}/src/wood/include/cgal_mesh_boolean.cpp"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/opengl/opengl_globals_geometry.cpp"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/opengl/opengl_globals.cpp"
  
  ) 
  
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
  # LIBRARIES
  ###############################################################################

  ###############################################################################
  # OPENGL
  ###############################################################################
  find_package(OpenGL REQUIRED)


  ###############################################################################
  # LINK HEADERS TO EXE
  # check path:
  # message(AUTHOR_WARNING "${CMAKE_SOURCE_DIR}/viewer/include/stb")
  ###############################################################################
  list(APPEND petras_include_paths 
  #my_code
  "${CMAKE_SOURCE_DIR}/src/viewer/include/"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/stb/"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/pipe/"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/opengl/"
  "${CMAKE_SOURCE_DIR}/src/viewer/include/imgui/"
  "${CMAKE_SOURCE_DIR}/src/wood/include/"
  #cdt
  "${CMAKE_BINARY_DIR}/install/cdt/CDT/include/"
  #clipper
  "${CMAKE_BINARY_DIR}/install/clipper_2/CPP/Clipper2Lib/include/"
  #imgui
  "${CMAKE_BINARY_DIR}/install/imgui/"
  "${CMAKE_BINARY_DIR}/install/imgui/backends/"
  #glfw
  "${CMAKE_BINARY_DIR}/install/glfw/include/"
  #glad
  "${CMAKE_BINARY_DIR}/install/glad/include/"
  "${CMAKE_BINARY_DIR}/install/glad/include/glad"
  #glm
  "${CMAKE_BINARY_DIR}/install/glm/"
  #assimp
  "${CMAKE_BINARY_DIR}/install/assimp/include/"
  "${CMAKE_BINARY_DIR}/install/assimp/contrib/zlib/"# WINDOWS
  #boost
  "${CMAKE_BINARY_DIR}/install/boost/include/boost-1_78/"
  #eigen
  "${CMAKE_BINARY_DIR}/install/eigen/"
  #cgal
  "${CMAKE_BINARY_DIR}/install/cgal/include"
  "${CMAKE_BINARY_DIR}/install/cgal/auxiliary/gmp/include"
  #"C:/msys64/mingw64/lib/" #GCC
  #googletest
  "${CMAKE_BINARY_DIR}/install/googletest/include/"

  )


  target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${petras_include_paths}>") #header for the library

  ###############################################################################
  # LINK LIBRARIES TO EXE, STATIC LIBRARIES PATHS MUST BE AT THE END
  ###############################################################################
  if(COMPILER_NINJA)
      message(STATUS "COMPILER_NINJA ON")
      target_link_libraries(${PROJECT_NAME} PUBLIC 
      Clipper2 
      opengl32 
      imgui_lib 
      glfw3 
      glad 
      assimp 
      stb 
      zlibstatic 
      gmp
      mpfr
      gtest
      gmock
      ) #gmp mpfr link library | zlibstatic WINDOWS

  else()
    message(STATUS "COMPILER_NINJA OFF")
    target_link_libraries(${PROJECT_NAME} PUBLIC 
      Clipper2 
      opengl32 
      imgui_lib 
      glfw3 
      glad 
      assimp 
      stb 
      zlibstatic 
      libgmp-10
      libmpfr-4
      gtest
      gmock
    ) #gmp mpfr link library | zlibstatic WINDOWS
  endif()

  ###############################################################################
  # GLFW
  ###############################################################################
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/glfw/src/" )

  ###############################################################################
  # GLAD
  ###############################################################################
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/glad/")

  ###############################################################################
  # ASSIMP
  ###############################################################################
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/assimp/lib/" )
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/assimp/contrib/zlib/" ) # WINDOWS

  ###############################################################################
  # STB
  ###############################################################################
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Release" )

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
  # GOOGLETEST
  ###############################################################################
  target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/install/googletest/lib/" )

  message(AUTHOR_WARNING "${CMAKE_BINARY_DIR}/Release" ${BUILD_MY_PROJECTS})




endif ()


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
  # IMGUI
  ###############################################################################

  list(APPEND IMGUI_INC 
  "${CMAKE_BINARY_DIR}/install/glfw/include/"
  "${CMAKE_BINARY_DIR}/install/imgui/"
  "${CMAKE_BINARY_DIR}/install/imgui/backends/"
  )

  set(IMGUI_SRC

  "${CMAKE_BINARY_DIR}/install/imgui/imgui_demo.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/imgui_draw.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/imgui_tables.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/imgui_widgets.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/imgui.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/backends/imgui_impl_opengl3.cpp"
  "${CMAKE_BINARY_DIR}/install/imgui/backends/imgui_impl_glfw.cpp"
  )

  add_library(imgui_lib STATIC ${IMGUI_INC} ${IMGUI_SRC}  )
  target_include_directories(imgui_lib  PUBLIC ${IMGUI_INC}) #link headers to clipper library

  ###############################################################################
  # STB
  ###############################################################################
  set(STB_INC
  "${CMAKE_SOURCE_DIR}/src/viewer/include/stb/stb_image.h"
    "${CMAKE_SOURCE_DIR}/src/viewer/include/stb/stb_image_write.h"
 )

  set(STB_SRC
    "${CMAKE_SOURCE_DIR}/src/viewer/include/stb/stb_image.cpp"
    "${CMAKE_SOURCE_DIR}/src/viewer/include/stb/stb_image_write.cpp"
  )

  add_library(STB STATIC ${STB_INC} ${STB_SRC}  )
  target_include_directories(STB  PUBLIC "${CMAKE_SOURCE_DIR}/viewer/include/stb/") #link headers to stb library

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

  ###############################################################################
  # BOOST
  ###############################################################################

  ###############################################################################
  # EIGEN
  ###############################################################################


  message(AUTHOR_WARNING "Part 1 - endif" ${BUILD_MY_PROJECTS})
endif ()


