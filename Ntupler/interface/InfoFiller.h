#ifndef INFO_H
#define INFO_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PEvent.h"

#include <map>
#include <string>

namespace panda {
class InfoFiller : virtual public BaseFiller
{
		public:
				InfoFiller(TString n);
				~InfoFiller();
				int analyze(const edm::Event& iEvent);
				int analyzeLumi(const edm::LuminosityBlock&, TTree*);
				virtual inline string name(){return "InfoFiller";};
				void init(TTree *t); 

				edm::Handle<std::vector<long> > events_handle;
				edm::EDGetTokenT<std::vector<long> > events_token;

				edm::Handle<std::vector<float> > weights_handle;
				edm::EDGetTokenT<std::vector<float> > weights_token;

				// edm::Handle<std::vector<int> > putrue_handle;
				// edm::EDGetTokenT<std::vector<int> > putrue_token;

				TH1D *hDTotalMCWeight{0};
				TH1D *hDTotalEvents{0};
		private:
				panda::PEvent *data;
				bool isData=false;
				TString treename;
};
}


#endif
