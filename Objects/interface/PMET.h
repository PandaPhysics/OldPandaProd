#ifndef PANDA_PMET
#define PANDA_PMET

#include "PObject.h"

namespace panda
{
  class PMET : public TObject
  {
    public:
      PMET() { }
      ~PMET(){ }
    
    float pt=0, phi=0;                 //!< type-1 corrected MET
    float sumETRaw;                    //!< sum E_T, uncorrected
    float raw_pt=0, raw_phi=0;         //!< raw, uncorrected
    float calo_pt=0, calo_phi=0;       //!< calorimeter-only
    float noMu_pt=0, noMu_phi=0;       //!< without muons
    float noHF_pt=0, noHF_phi=0;       //!< without HF
    float trk_pt=0, trk_phi=0;         //!< tracks only
    float neutral_pt=0, neutral_phi=0; //!< neutral particles only
    float photon_pt=0, photon_phi=0;   //!< photons only
    float hf_pt=0, hf_phi=0;           //!< HF only

    ClassDef(PMET,1)
  };

}
#endif
