project( AnalysisPlugins )

find_package( ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_package( AnalysisBase QUIET )

atlas_subdir( AnalysisPlugins )

# build a dictionary for the library
atlas_add_root_dictionary ( AnalysisPluginsLib AnalysisPluginsDictSource
                            ROOT_HEADERS AnalysisPlugins/*.h Root/LinkDef.h
                            EXTERNAL_PACKAGES ROOT
)

file(GLOB AnalysisPluginsHeaders AnalysisPlugins/*.h)
file(GLOB AnalysisPluginsSources Root/*.cxx)

atlas_add_library( AnalysisPluginsLib AnalysisPlugins/*.h Root/*.h Root/*.cxx ${AnalysisPluginsDictSource}
                   PUBLIC_HEADERS AnalysisPlugins
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop xAODBase xAODRootAccess xAODCutFlow xAODMuon
                   xAODEventInfo queryosity::queryosity )

atlas_add_executable( phys_example examples/phys_example.cxx
  LINK_LIBRARIES AnalysisPluginsLib ) 