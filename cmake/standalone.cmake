
project(qhep)

file(GLOB QHEP_HEADERS qhep/*.h)
list(FILTER QHEP_HEADERS EXCLUDE REGEX ".*qhep/Event\.h$")
list(FILTER QHEP_HEADERS EXCLUDE REGEX ".*qhep/qhepDict\.h$")

file(GLOB QHEP_SOURCES Root/*.cxx)
list(FILTER QHEP_SOURCES EXCLUDE REGEX ".*Root/Event\.cxx$")
list(FILTER QHEP_HEADERS EXCLUDE REGEX ".*Root/LinkDef\.h$")

add_library(qhep SHARED ${QHEP_SOURCES} ${QHEP_HEADERS} )

target_include_directories(qhep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(qhep PUBLIC cxx_std_17)

target_compile_options(
  qhep PRIVATE 
  -Wall -Wextra 
  $<$<CONFIG:Release>:-O3> 
  $<$<CONFIG:RelWithDebInfo>:-O3> 
  $<$<CONFIG:Debug>:-O0> $<$<CONFIG:Debug>:-ggdb3> 
)

find_package(ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_library(ROOT_TREEPLAYER_LIBRARY TreePlayer HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
find_library(ROOT_RDATAFRAME_LIBRARY ROOTDataFrame HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
target_link_libraries(qhep queryosity::queryosity ROOT::Core ROOT::RIO ROOT::Hist ROOT::Tree ROOT::TreePlayer ROOT::Imt ROOT::ROOTVecOps ROOT::ROOTDataFrame ROOT::Physics)

ROOT_GENERATE_DICTIONARY(
  qhep_dict ${QHEP_HEADERS}
  LINKDEF Root/LinkDef.h
  MODULE qhep
)

export(PACKAGE qhep)
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/qhep" )
set(CONF_LIBRARY_DIRS "${PROJECT_BINARY_DIR}")
set(CONF_LIBRARIES qhep)
configure_file(qhep-config.cmake.in
  "${PROJECT_BINARY_DIR}/config.cmake" @ONLY)

set(QHEP_PYTHONPATH ${CMAKE_CURRENT_BINARY_DIR})
set(QHEP_LD_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR})

install(DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/qhep"
  EXPORT qhepConfig
  DESTINATION lib
  )

add_executable(tree_example examples/tree_example.cxx)
target_compile_features(tree_example PUBLIC cxx_std_17)
target_include_directories(tree_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${ROOT_LIBRARY_DIR})
target_link_libraries(tree_example qhep ${ROOT_LIBRARIES})

set(SETUP ${CMAKE_CURRENT_BINARY_DIR}/setup.sh)
file(WRITE ${SETUP} "#!/bin/bash\n")
file(APPEND ${SETUP} "ulimit -S -s unlimited\n" )
file(APPEND ${SETUP} "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${QHEP_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${QHEP_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${QHEP_LD_LIBRARY_PATH}\n")

