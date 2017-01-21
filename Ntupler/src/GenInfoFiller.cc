#include "PandaProd/Ntupler/interface/GenInfoFiller.h"

#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"


using namespace panda;

GenInfoFiller::GenInfoFiller(TString n):
    BaseFiller()
{
  data = new PGenInfo();
  treename = n;
}

GenInfoFiller::~GenInfoFiller(){
  delete data;
}

void GenInfoFiller::init(TTree *t) {
  t->Branch(treename.Data(),&data,99);
}

int GenInfoFiller::analyze(const edm::Event& iEvent){
  
  if (skipEvent!=0 && *skipEvent) {
    return 0;
  }
  
  if (iEvent.isRealData()) return 0;
  
  iEvent.getByToken(lhe_token,lhe_handle);
  
  if (lhe_handle.isValid() and lhe_handle->weights().size() >0){
		if (nsyst<0)
			nsyst = lhe_handle->weights().size();
    data->mcWeights_syst->resize(nsyst); 
    for( unsigned int iweight = 0 ; iweight<(unsigned)nsyst ;iweight++){
      data->mcWeights_syst->at(iweight) = float(lhe_handle -> weights() . at(iweight) . wgt );
			if (firstEvent) {
				systTable += TString::Format("\n%u:%s",
                           						iweight,
                           						lhe_handle -> weights() . at(iweight) . id . c_str());
			}
    }
  }

	if (firstEvent)
		firstEvent=false;

  return 0;
}

