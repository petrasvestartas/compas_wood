# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/auxiliary"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src/cgal_dependencies-build"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/tmp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src/cgal_dependencies-stamp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src/cgal_dependencies-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src/cgal_dependencies-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cgal_dependencies-prefix/src/cgal_dependencies-stamp${cfgdir}") # cfgdir has leading slash
endif()
