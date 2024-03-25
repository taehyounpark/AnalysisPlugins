#include "queryosity/queryosity.h"

#include "qhep/RDS.h"
#include "qhep/Tree.h"

#include "qhep/Hist.h"

#ifdef __CINT__

#pragma link off all functions;
#pragma link off all classes;
#pragma link off all globals;

#pragma link C++ class queryosity::dataflow + ;

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
