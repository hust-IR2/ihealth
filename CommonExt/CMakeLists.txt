#cmake file for CommonExt
#Author Can HUANG
#Created: 2017/12/14

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} Root_src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(CommonExt SHARED  ${Root_src})
ADD_DEFINITIONS(-D_AFXDLL)

SET(CMAKE_MFC_FLAG 2)
add_definitions(-DCommonExt_EXPORTS)
add_definitions(-DDCICOMMONEXT_MODULE)
set_target_properties(CommonExt PROPERTIES OUTPUT_NAME "toolbox")
