#ifndef Muon_H
#define Muon_H

#include "BaseFiller.h"
#include "EventFiller.h"
#include "PandaProd/Objects/interface/PMuon.h"

#include <map>
#include <string>

namespace panda {
class MuonFiller : virtual public BaseFiller
{
		public:
				MuonFiller(TString n);
				~MuonFiller();
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "MuonFiller";};
				void init(TTree *t);

				edm::EDGetTokenT<pat::MuonCollection> mu_token;
				edm::Handle<pat::MuonCollection> mu_handle;

				const EventFiller *evt=0;

				float minPt=10, maxEta=2.5;

		private:
				// TClonesArray *data;
				panda::VMuon *data;
				TString treename;

};
}


#endif
