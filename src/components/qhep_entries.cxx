#include <qhep/Tree.h>
#include <qhep/Hist.h>
#include <qhep/RDS.h>
#include <queryosity/qureyosity.h>

DECLARE_COMPONENT (Tree)
DECLARE_COMPONENT (RDS)
DECLARE_COMPONENT (Hist<1,float>)
DECLARE_COMPONENT (queryosity::dataflow)