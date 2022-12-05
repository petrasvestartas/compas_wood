# Install script for directory: C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/CGAL")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "CGAL_Qt5" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CGAL/Qt" TYPE DIRECTORY FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/include/CGAL/Qt/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/CGAL" TYPE FILE FILES
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/AUTHORS"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/CHANGES.md"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/LICENSE"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/LICENSE.GPL"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/LICENSE.LGPL"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/include/CGAL" REGEX "/\\.svn$" EXCLUDE REGEX "/qt$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/scripts/cgal_create_CMakeLists"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/scripts/cgal_create_cmake_script"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/scripts/cgal_make_macosx_app"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CGAL" TYPE DIRECTORY FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/cmake/modules/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CGAL" TYPE FILE FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/cmake/modules/UseCGAL.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/CGAL" TYPE FILE FILES
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/lib/cmake/CGAL/CGALConfig.cmake"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/lib/cmake/CGAL/CGALConfigBuildVersion.cmake"
    "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/lib/cmake/CGAL/CGALConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE FILES "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/auxiliary/cgal_create_cmake_script.1")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal-prefix/src/cgal-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
