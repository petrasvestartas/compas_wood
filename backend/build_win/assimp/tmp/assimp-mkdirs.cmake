# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/assimp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src/assimp-build"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/tmp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src/assimp-stamp"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src"
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src/assimp-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src/assimp-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/assimp/src/assimp-stamp${cfgdir}") # cfgdir has leading slash
endif()
