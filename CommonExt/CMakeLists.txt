aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} Root_src)

file(GLOB Root_header *.h *.hpp) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(CommonExt SHARED  ${Root_src} ${Root_header})

source_group("Source Files" FILES ${Root_src})

source_group("Header Files" FILES ${Root_header})


ADD_DEFINITIONS(-D_AFXDLL)

SET(CMAKE_MFC_FLAG 2)
add_definitions(-DCommonExt_EXPORTS)
add_definitions(-DDCICOMMONEXT_MODULE)
set_target_properties(CommonExt PROPERTIES OUTPUT_NAME "toolbox")
