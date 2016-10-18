#ifndef PANDA_PGENPARTICLE
#define PANDA_PGENPARTICLE

#include "PObject.h"


namespace panda
{
  class PGenParticle : public PObject
  {
    public:
      PGenParticle():
        PObject(),
        pdgid(0),
        parent(-1)
      {}
    ~PGenParticle(){}
    
    int pdgid, parent;
    ClassDef(PGenParticle,1)
  };

  typedef std::vector<PGenParticle*> VGenParticle;
}
#endif
