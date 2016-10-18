#ifndef PANDA_PPFCAND
#define PANDA_PPFCAND

#include "PObject.h"


namespace panda
{
  class PPFCand : public PObject
  {
    public:
      PPFCand():
        PObject(),
        e(0),
        q(-2),
        weight(-1),
        pftype(-1)
      {}
    ~PPFCand(){}
    
    float e, q, weight;
    int pftype;
    ClassDef(PPFCand,1)
  };

  typedef std::vector<PPFCand*> VPFCand;
}
#endif
