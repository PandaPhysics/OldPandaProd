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
        kVeto      = 1UL<<0,
        kLoose     = 1UL<<1,
        kMedium    = 1UL<<2,
        kTight     = 1UL<<3,
        kHLTPresel = 1UL<<4
      };
      PElectron():
        PObject(),
        q(0),
        id(0),
        iso(0),
        chiso(0),
        nhiso(0),
        phoiso(0),
        puiso(0)
      {  }
    ~PElectron(){ }
    
    int q;
    unsigned int id;
    float iso, chiso, nhiso, phoiso, puiso;

    ClassDef(PElectron,1)
  };

  typedef std::vector<PElectron*> VElectron;
}
#endif
