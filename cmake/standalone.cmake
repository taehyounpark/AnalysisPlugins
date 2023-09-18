
project(AnalysisPlugins)

file(GLOB AnalysisPluginsHeaders AnalysisPlugins/*.h)
list(FILTER AnalysisPluginsHeaders EXCLUDE REGEX ".*AnalysisPlugins/Event\.h$")

file(GLOB AnalysisPluginsSources Root/*.cxx)
list(FILTER AnalysisPluginsSources EXCLUDE REGEX ".*Root/Event\.cxx$")
list(FILTER AnalysisPluginsHeaders EXCLUDE REGEX ".*Root/LinkDef\.h$")

add_library( AnalysisPlugins SHARED ${AnalysisPluginsSources} ${AnalysisPluginsHeaders} )

set(ANALOGICAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/analogical/include/)
target_include_directories(AnalysisPlugins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ANALOGICAL_INCLUDE_DIR})

target_compile_features(AnalysisPlugins PUBLIC cxx_std_17)

find_package(ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Rint Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_library(ROOT_TREEPLAYER_LIBRARY TreePlayer HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
find_library(ROOT_RDATAFRAME_LIBRARY ROOTDataFrame HINTS ${ROOT_LIBRARY_DIR} REQUIRED)
target_link_libraries(AnalysisPlugins analogical ROOT::Core ROOT::RIO ROOT::Hist ROOT::Tree ROOT::TreePlayer ROOT::Imt ROOT::ROOTVecOps ROOT::ROOTDataFrame ROOT::Physics)

ROOT_GENERATE_DICTIONARY(
  AnalysisPlugins_dict ${AnalysisPluginsHeaders}
  LINKDEF Root/LinkDef.h
  MODULE AnalysisPlugins
)

export(PACKAGE AnalysisPlugins)
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/AnalysisPlugins" )
set(CONF_LIBRARY_DIRS "${PROJECT_BINARY_DIR}")
set(CONF_LIBRARIES AnalysisPlugins)
configure_file(AnalysisPlugins-config.cmake.in
  "${PROJECT_BINARY_DIR}/config.cmake" @ONLY)

set(ROOTANA_PYTHONPATH ${CMAKE_CURRENT_BINARY_DIR})
set(ROOTANA_LD_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR})

install(DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/AnalysisPlugins"
  EXPORT AnalysisPluginsConfig
  DESTINATION lib
  )

add_executable(tree_example examples/tree_example.cxx)
target_compile_features(tree_example PUBLIC cxx_std_17)
target_include_directories(tree_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ANALOGICAL_INCLUDE_DIR} ${ROOT_INCLUDE_DIR} ${ROOT_LIBRARY_DIR})
target_link_libraries(tree_example AnalysisPlugins ${ROOT_LIBRARIES})

set(SETUP ${CMAKE_CURRENT_BINARY_DIR}/setup.sh)
file(WRITE ${SETUP} "#!/bin/bash\n")
file(APPEND ${SETUP} "ulimit -S -s unlimited\n" )
file(APPEND ${SETUP} "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${ROOTANA_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${ROOTANA_LD_LIBRARY_PATH}\n")
file(APPEND ${SETUP} "export DYLD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${ROOTANA_LD_LIBRARY_PATH}\n")

