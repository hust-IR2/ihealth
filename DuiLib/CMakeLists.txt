aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} Root_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Control Control_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Core Core_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Layout Layout_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils Utils_src)


file(GLOB Root_header *.h *.hpp) 
file(GLOB Control_header ${CMAKE_CURRENT_SOURCE_DIR}/Control/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Control/*.hpp)
file(GLOB Core_header ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp)
file(GLOB Layout_header ${CMAKE_CURRENT_SOURCE_DIR}/Layout/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Layout/*.hpp)
file(GLOB Utils_header ${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Control)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Layout)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Utils)

add_library(duilib SHARED ${Control_src} ${Core_src} ${Layout_src} ${Utils_src} ${Root_src}
	${Root_header} ${Control_header} ${Core_header} ${Layout_header} ${Utils_header})

source_group("Source Files" FILES ${Root_src})
source_group("Source Files\\Control" FILES ${Control_src})
source_group("Source Files\\Core" FILES ${Core_src})
source_group("Source Files\\Layout" FILES ${Layout_src})
source_group("Source Files\\Utils" FILES ${Utils_src})

source_group("Header Files" FILES ${Root_header})
source_group("Header Files\\Control" FILES ${Control_header})
source_group("Header Files\\Core" FILES ${Core_header})
source_group("Header Files\\Layout" FILES ${Layout_header})
source_group("Header Files\\Utils" FILES ${Utils_header})

add_definitions(-DUILIB_EXPORTS)
target_link_libraries(duilib comctl32)
set_target_properties(duilib PROPERTIES OUTPUT_NAME "duilib")
