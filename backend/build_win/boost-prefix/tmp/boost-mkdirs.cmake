# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost-build"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/tmp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost-stamp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/boost-prefix/src/boost-stamp${cfgdir}") # cfgdir has leading slash
endif()
