#ifndef PANDA_POBJECT
#define PANDA_POBJECT

#include "PandaUtilities/Common/interface/Common.h"
#include <TObject.h>
#include <TClonesArray.h>


namespace panda
{
  class PObject : public TObject
  {
    public:
      PObject():
        pt(0),
        eta(0),
        phi(0),
        m(0)
        {}
    ~PObject(){}
    
    float pt,eta, phi, m;
    ClassDef(PObject,1)
  };

  inline bool SortPObjects(PObject *o1, PObject *o2) {
    return o1->pt > o2->pt;
  }
}
#endif
