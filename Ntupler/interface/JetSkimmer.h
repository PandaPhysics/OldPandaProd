#ifndef JetSkimmer_H
#define JetSkimmer_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PFatJet.h"

#include <map>
#include <string>


namespace panda {
class JetSkimmer : virtual public BaseFiller
{
		public:
				JetSkimmer(TString n);
				~JetSkimmer();
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "JetSkimmer";};
				void init(TTree *t);

				edm::Handle<pat::JetCollection> chsAK8_handle; 
				edm::EDGetTokenT<pat::JetCollection> chsAK8_token;
				edm::Handle<pat::JetCollection> puppiAK8_handle; 
				edm::EDGetTokenT<pat::JetCollection> puppiAK8_token;
				edm::Handle<pat::JetCollection> chsCA15_handle; 
				edm::EDGetTokenT<pat::JetCollection> chsCA15_token;
				edm::Handle<pat::JetCollection> puppiCA15_handle; 
				edm::EDGetTokenT<pat::JetCollection> puppiCA15_token;

				float minPt=180, maxEta=2.5, minMass=0;

		private:
				TString treename;

};
}


#endif
