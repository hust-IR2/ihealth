cmake_minimum_required(VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(OpenCV REQUIRED)
find_package (Boost REQUIRED)
find_package(EIGEN)
find_package(TBB)
find_package(NI)
find_package(PUPILTRACKER)
find_package(APS)
find_package(MYSQL)
find_package(MSC)
find_package(WKE)
find_package(NI)
find_package(ZPLAY)
include_directories(
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ 
${PUPILTRACKER_INCLUDE_DIRS} 
${APS_INCLUDE_DIRS} 
${MYSQL_INCLUDE_DIRS} 
${APS_INCLUDE_DIRS} 
${MSC_INCLUDE_DIRS} 
${WKE_INCLUDE_DIRS} 
${OpenCV_INCLUDE_DIRS} 
${Boost_INCLUDE_DIRS}
${EIGEN_INCLUDE_DIRS} 
${NI_INCLUDE_DIRS}
${ZPLAY_INCLUDE_DIRS}
${TBB_INCLUDE_DIRS})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} Root_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ ATI_src)

file(GLOB Root_header ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB ATI_header ${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/*.h ${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/*.hpp)

set(ATI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ)
list(REMOVE_ITEM ATI_src "${ATI_SRC_DIR}/events.c" "${ATI_SRC_DIR}/xmltok_impl.c" "${ATI_SRC_DIR}/xmltok_ns.c")
list(REMOVE_ITEM ATI_header "${ATI_SRC_DIR}/ascii.h" "${ATI_SRC_DIR}/asciitab.h" "${ATI_SRC_DIR}/ftsharedrt.h" "${ATI_SRC_DIR}/iasciitab.h" "${ATI_SRC_DIR}/latin1tab.h" 
"${ATI_SRC_DIR}/nametab.h" "${ATI_SRC_DIR}/utf8tab.h" "${ATI_SRC_DIR}/xmldef.h" "${ATI_SRC_DIR}/xmltok_impl.h")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /sdl-")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /sdl-")

ADD_DEFINITIONS(-D_AFXDLL)
add_executable(ihealth ${Root_src} ${ATI_src} ${Root_header} ${ATI_header})
add_dependencies(ihealth duilib CommonExt)

source_group("Source Files" FILES ${Root_src})
source_group("Source Files\\ATI" FILES ${ATI_src})
source_group("Header Files" FILES ${Root_header})
source_group("Header Files\\ATI" FILES ${ATI_header})

target_link_libraries(ihealth duilib CommonExt ${PUPILTRACKER_LIBRARY} ${APS_LIBRARY} ${MYSQL_LIBRARY} ${MSC_LIBRARY} ${WKE_LIBRARY} ${NI_LIBRARY} ${OpenCV_LIBRARIES} 
${TBB_LIBRARY} 
${ZPLAY_LIBRARY} 
)

if(WIN32)
   set_target_properties(ihealth PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
   set_target_properties(ihealth PROPERTIES COMPILE_DEFINITIONS_DEBUG "/SUBSYSTEM:windows")
   set_target_properties(ihealth PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")
   set_target_properties(ihealth PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "/SUBSYSTEM:windows")
   set_target_properties(ihealth PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
   set_target_properties(ihealth PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
endif(WIN32)
