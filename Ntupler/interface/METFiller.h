#ifndef MET_H
#define MET_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PMET.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"

#include <map>

namespace panda {
class METFiller : virtual public BaseFiller
{
		public:
				enum pfcandtype {
					kPat,
					kRecoPF
				};

				METFiller(TString n);
				~METFiller();
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "METFiller";};
				void init(TTree *t);

				edm::EDGetTokenT<pat::METCollection> met_token;
				edm::Handle<pat::METCollection> met_handle;

				edm::EDGetTokenT<pat::METCollection> cleanmu_met_token;
				edm::Handle<pat::METCollection> cleanmu_met_handle;

				edm::EDGetTokenT<pat::METCollection> cleaneg_met_token;
				edm::Handle<pat::METCollection> cleaneg_met_handle;

				edm::EDGetTokenT<pat::METCollection> unclean_met_token;
				edm::Handle<pat::METCollection> unclean_met_handle;

				// pf cands - used to recalc some raw METs
				edm::EDGetTokenT<pat::PackedCandidateCollection> pat_token;
				edm::Handle<pat::PackedCandidateCollection> pat_handle;

				edm::EDGetTokenT<reco::PFCandidateCollection> recopf_token;
				edm::Handle<reco::PFCandidateCollection> recopf_handle;
				
				pfcandtype which_cand=kPat;

				bool minimal=true;

		private:
				panda::PMET *data;
				TString treename;

};
}


#endif
