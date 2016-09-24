#ifndef PANDA_PElectron
#define PANDA_PElectron

#include "PObject.h"
#include <vector>

namespace panda
{
  class PElectron : public PObject
  {
    public:
      enum ElectronID {
        kVeto   = 1UL<<0,
        kLoose  = 1UL<<1,
        kMedium = 1UL<<2,
        kTight  = 1UL<<3
      };
      PElectron():
        PObject(),
        q(0),
        id(0),
        iso(0)
      {  }
    ~PElectron(){ }
    
    int q;
    unsigned int id;
    float iso;

    ClassDef(PElectron,1)
  };

  typedef std::vector<PElectron*> VElectron;
}
#endif
