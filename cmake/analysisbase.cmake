project( queryosity-hep )

find_package( ROOT REQUIRED COMPONENTS Core Imt RIO Net Hist Graf Graf3d Gpad ROOTVecOps Tree TreePlayer Postscript Matrix Physics MathCore Thread MultiProc ROOTDataFrame )
find_package( AnalysisBase QUIET )

atlas_subdir( queryosity-hep )

atlas_add_root_dictionary ( qhepLib qhepDictSource
                           ROOT_HEADERS qhep/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT
)

atlas_add_library( qhepLib 
  qhep/*.h Root/*.cxx ${qhepDictSource}
  PUBLIC_HEADERS qhep
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop xAODBase xAODRootAccess xAODCutFlow xAODMuon xAODEventInfo queryosity::queryosity )

# atlas_add_dictionary (qhepDict
#   qhep/qhepDict.h
#   qhep/selection.xml
#   LINK_LIBRARIES qhepLib queryosity::queryosity )

atlas_add_executable( phys_example examples/phys_example.cxx
  LINK_LIBRARIES qhepLib ) 