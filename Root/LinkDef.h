#include "queryosity/queryosity.h"

#include "AnalysisPlugins/RDS.h"
#include "AnalysisPlugins/Tree.h"

#include "AnalysisPlugins/Hist.h"

#ifdef __CINT__

#pragma link off all functions;
#pragma link off all classes;
#pragma link off all globals;

#pragma link C++ class queryosity::dataflow + ;

#pragma link C++ class queryosity::multithread + ;
#pragma link C++ class queryosity::action + ;

#pragma link C++ class queryosityterm < float> + ;
#pragma link C++ class queryositycell < float> + ;
#pragma link C++ class queryosity::column::observable < float> + ;
#pragma link C++ class queryosityvariable < float> + ;

#pragma link C++ class queryosity::selection + ;
#pragma link C++ class queryosity::selection::cut + ;
#pragma link C++ class queryosity::selection::weight + ;
#pragma link C++ class queryosity::selection::cutflow + ;

#pragma link C++ class queryosity::query + ;
#pragma link C++ class queryosity::query::experiment + ;

#pragma link C++ class Hist < 1, float> + ;
#pragma link C++ class Hist < 2, float> + ;
#pragma link C++ class Hist < 3, float> + ;
#pragma link C++ class Hist < 1, ROOT::RVec < float>> + ;
#pragma link C++ class Hist < 2, ROOT::RVec < float>> + ;
#pragma link C++ class Hist < 3, ROOT::RVec < float>> + ;

#pragma link C++ class Tree + ;
#pragma link C++ class RDS + ;
#pragma link C++ class Event + ;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif
