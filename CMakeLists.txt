################################################################################
# qhep
################################################################################

if($ENV{AnalysisBase_SET_UP})
  set(STANDALONE_BUILD 0)
  set(ANALYSISBASE_BUILD 1)  
else()
  set(STANDALONE_BUILD 1)
  set(ANALYSISBASE_BUILD 0)
endif()

if(ANALYSISBASE_BUILD OR NOT STANDALONE_BUILD)
  include(./cmake/analysisbase.cmake)
endif()

if(STANDALONE_BUILD OR NOT ANALYSISBASE_BUILD)
  include(./cmake/standalone.cmake)
endif()