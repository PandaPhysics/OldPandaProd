#ifndef PANDA_PMET
#define PANDA_PMET

#include "PObject.h"
#include <vector>

namespace panda
{
  class PMET : public PObject
  {
    public:
      PMET():
        PObject()
      {  }
    ~PMET(){ }
    
    float sumETRaw;
    float raw_pt, raw_phi;
    float calo_pt, calo_phi;
    float noMu_pt, noMu_phi;
    float noHF_pt, noHF_phi;
    float trk_pt, trk_phi;
    float neutral_pt, neutral_phi;
    float photon_pt, photon_phi;
    float hf_pt, hf_phi;

    ClassDef(PMET,1)
  };

}
#endif
