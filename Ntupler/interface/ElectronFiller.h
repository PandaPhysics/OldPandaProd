#ifndef Electron_H
#define Electron_H

#include "BaseFiller.h"
#include "EventFiller.h"
#include "PandaProd/Objects/interface/PElectron.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include <map>
#include <string>
#include <memory>

namespace panda {
class ElectronFiller : virtual public BaseFiller
{
		public:
				ElectronFiller(TString n);
				~ElectronFiller();
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "ElectronFiller";};
				void init(TTree *t);

				edm::EDGetTokenT<pat::ElectronCollection> el_token;
				edm::Handle<pat::ElectronCollection> el_handle;

				edm::EDGetTokenT<edm::ValueMap<bool> >	el_vetoid_token;
				edm::Handle<edm::ValueMap<bool> > el_vetoid_handle;

				edm::EDGetTokenT<edm::ValueMap<bool> >	el_looseid_token;
				edm::Handle<edm::ValueMap<bool> > el_looseid_handle;

				edm::EDGetTokenT<edm::ValueMap<bool> >	el_mediumid_token;
				edm::Handle<edm::ValueMap<bool> > el_mediumid_handle;

				edm::EDGetTokenT<edm::ValueMap<bool> >	el_tightid_token;
				edm::Handle<edm::ValueMap<bool> > el_tightid_handle;

				edm::EDGetTokenT<edm::ValueMap<bool> >	el_hltid_token;
				edm::Handle<edm::ValueMap<bool> > el_hltid_handle;

				std::unique_ptr<EffectiveAreas> effArea;

				const EventFiller *evt=0;

				float minPt=10, maxEta=2.5;

		private:
				// TClonesArray *data;
				panda::VElectron *data;
				TString treename;

};
}


#endif
