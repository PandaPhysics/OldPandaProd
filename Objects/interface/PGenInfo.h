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
	   mcWeights_syst_id = new std::vector<std::string>;
	 }
    ~PGenInfo(){ delete mcWeights_syst; delete mcWeights_syst_id; }
    
    std::vector<float> *mcWeights_syst;
    std::vector<std::string> *mcWeights_syst_id;
    ClassDef(PGenInfo,1)
  };
}
#endif
