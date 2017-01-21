#ifndef PANDA_PGENINFO
#define PANDA_PGENINFO

#include <TObject.h>
#include <TClonesArray.h>


namespace panda
{
  class PGenInfo : public TObject
  {
    public:
      PGenInfo()
			{
				mcWeights_syst = new std::vector<float>;
			}
			~PGenInfo(){ delete mcWeights_syst; }
    
			std::vector<float> *mcWeights_syst;
			ClassDef(PGenInfo,1)
  };
}
#endif
