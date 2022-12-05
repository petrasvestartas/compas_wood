# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/clipper_2"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src/clipper_2-build"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/tmp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src/clipper_2-stamp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src/clipper_2-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src/clipper_2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/clipper_2-prefix/src/clipper_2-stamp${cfgdir}") # cfgdir has leading slash
endif()
