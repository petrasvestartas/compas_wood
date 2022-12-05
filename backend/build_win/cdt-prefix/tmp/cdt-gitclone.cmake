# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitclone-lastrun.txt" AND EXISTS "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitinfo.txt" AND
  "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitclone-lastrun.txt" IS_NEWER_THAN "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe" 
            clone --no-checkout --config "advice.detachedHead=false" "https://github.com/artem-ogre/CDT.git" "cdt"
    WORKING_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/artem-ogre/CDT.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe" 
          checkout "master" --
  WORKING_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitinfo.txt" "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/cdt-prefix/src/cdt-stamp/cdt-gitclone-lastrun.txt'")
endif()
