#cmake file for ihealth
#Author Can HUANG
#Created: 2017/12/14
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
set(AIT_src 
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/DAQSys.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/dom.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/expatls.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/ftconfig.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/ftrt.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/FTWrapper.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/node.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/stack.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/xmlparse.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/xmlrole.c
${CMAKE_CURRENT_SOURCE_DIR}/ATI/ATIDAQ/xmltok.c
)
ADD_DEFINITIONS(-D_AFXDLL)
add_executable(ihealthDemo ${Root_src} ${AIT_src})
add_dependencies(ihealthDemo duilib CommonExt)

target_link_libraries(ihealthDemo duilib CommonExt ${PUPILTRACKER_LIBRARY} ${APS_LIBRARY} ${MYSQL_LIBRARY} ${MSC_LIBRARY} ${WKE_LIBRARY} ${NI_LIBRARY} ${OpenCV_LIBRARIES} 
${TBB_LIBRARY} 
${ZPLAY_LIBRARY} 
)

if(WIN32)
   set_target_properties(ihealthDemo PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows")
   set_target_properties(ihealthDemo PROPERTIES COMPILE_DEFINITIONS_DEBUG "/SUBSYSTEM:windows")
   set_target_properties(ihealthDemo PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows")
   set_target_properties(ihealthDemo PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "/SUBSYSTEM:windows")
   set_target_properties(ihealthDemo PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
   set_target_properties(ihealthDemo PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
endif(WIN32)
