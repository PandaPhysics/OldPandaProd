#ifndef PANDA_PPhoton
#define PANDA_PPhoton

#include "PObject.h"
#include <vector>

namespace panda
{
  class PPhoton : public PObject
  {
    public:
      enum PhotonID {
        kVeto   = 1UL<<0,
        kLoose  = 1UL<<1,
        kMedium = 1UL<<2,
        kTight  = 1UL<<3,
        kEleVeto= 1UL<<4
      };
      PPhoton():
        PObject(),
        id(0),
        iso(0)
      {  }
    ~PPhoton(){ }
    
    unsigned int id;
    float iso;

    ClassDef(PPhoton,1)
  };

  typedef std::vector<PPhoton*> VPhoton;
}
#endif
