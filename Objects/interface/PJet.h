#ifndef PANDA_PJET
#define PANDA_PJET

#include "PObject.h"
#include "PPFCand.h"
#include <vector>

namespace panda
{
  class PJet : public PObject
  {
    public:
      enum JetID {
        kLoose   = 1UL<<0,
        kTight   = 1UL<<1
      };
      PJet():
        PObject(),
        rawPt(0),
        csv(-1),
        qgl(-1),
        constituents(0),
        id(-1)
      {  }
    ~PJet(){ delete constituents; }
    
    float rawPt,csv,qgl;
    // std::vector<PPFCand> constituents;
    // TClonesArray *constituents=0;
    //VPFCand *constituents;
    std::vector<UShort_t> *constituents;
    unsigned int id;

    PPFCand *getPFCand(unsigned int ipf, VPFCand *vpf) { return vpf->at(constituents->at(ipf)); }

    ClassDef(PJet,1)
  };

  typedef std::vector<PJet*> VJet;
}
#endif