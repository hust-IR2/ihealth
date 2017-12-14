# CMake root file for duilib related apps
#Date 2012-09-16
#Author: Qi Gao(monkgau@gmail.com)
#
#  CMake files for duilib
#  file for each app is under the related sub-dir
#

cmake_minimum_required(VERSION 2.8)

project(ihealth)

message(STATUS," CMake project files for ihealth")
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# this line is for UNICODE release,which is required by DuiDesigner 
add_definitions(-DUNICODE -D_UNICODE)


# add each CMake file
add_subdirectory(duilib)
add_subdirectory(CommonExt)
