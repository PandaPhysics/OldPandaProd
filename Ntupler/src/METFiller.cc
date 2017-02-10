#include "PandaProd/Ntupler/interface/METFiller.h"

using namespace panda;

METFiller::METFiller(TString n):
		BaseFiller()
{
	data = new PMET();
	treename = n;
}

METFiller::~METFiller(){
	delete data;
}

void METFiller::init(TTree *t) {
	t->Branch(treename.Data(),&data);
}


int METFiller::analyze(const edm::Event& iEvent){
		if (skipEvent!=0 && *skipEvent) {
			return 0;
		}

		iEvent.getByToken(met_token,met_handle);
		const pat::MET &met = met_handle->front();
		data->pt = met.pt();
		data->phi = met.phi();
		data->sumETRaw = met.uncorSumEt();
		data->raw_pt = met.uncorPt();
		data->raw_phi = met.uncorPhi(); 

		if (minimal)
			return 0;

		data->calo_pt = met.caloMETPt();
		data->calo_phi = met.caloMETPhi();

		iEvent.getByToken(cleanmu_met_token,cleanmu_met_handle);
		const pat::MET &cleanmu_met = cleanmu_met_handle->front();
		data->cleanMu_pt = cleanmu_met.pt();
		data->cleanMu_phi = cleanmu_met.phi();

		if (iEvent.isRealData()) {
			iEvent.getByToken(cleaneg_met_token,cleaneg_met_handle);
			const pat::MET &cleaneg_met = cleaneg_met_handle->front();
			data->cleanEG_pt = cleaneg_met.pt();
			data->cleanEG_phi = cleaneg_met.phi();

			iEvent.getByToken(unclean_met_token,unclean_met_handle);
			const pat::MET &unclean_met = unclean_met_handle->front();
			data->unclean_pt = unclean_met.pt();
			data->unclean_phi = unclean_met.phi();
		}

		std::vector<const reco::Candidate*> pfcands;
		if (which_cand==kPat) {
			iEvent.getByToken(pat_token,pat_handle);
			const pat::PackedCandidateCollection *pfCol = pat_handle.product();
			for(pat::PackedCandidateCollection::const_iterator iPF = pfCol->begin(); 
						iPF!=pfCol->end(); ++iPF) {
				pfcands.push_back( (const reco::Candidate*)&(*iPF) );
			}
		} else if (which_cand==kRecoPF) {
			iEvent.getByToken(recopf_token,recopf_handle);
			const reco::PFCandidateCollection *pfCol = recopf_handle.product();
			for (reco::PFCandidateCollection::const_iterator iPF=pfCol->begin();
						iPF!=pfCol->end(); ++iPF) {
				pfcands.push_back( (const reco::Candidate*)&(*iPF) );
			}
		}

		TVector2 metnomu, tkmet, ntrlmet, phomet, hfmet;
		metnomu.SetMagPhi(data->pt,data->phi);
		for (auto *cand : pfcands) {
			
			if (std::abs(cand->pdgId())==13)
				metnomu += TVector2(cand->px(),cand->py());

			if (cand->charge()!=0)
				tkmet += TVector2(-cand->px(),-cand->py());

			if (cand->pdgId()==130)
				ntrlmet += TVector2(-cand->px(),-cand->py());

			if (cand->pdgId()==22) 
				phomet += TVector2(-cand->px(),-cand->py());

			if (cand->pdgId()==1||cand->pdgId()==2)
				hfmet += TVector2(-cand->px(),-cand->py());

		}

		data->noMu_pt = metnomu.Mod(); data->noMu_phi = metnomu.Phi();
		data->hf_pt = hfmet.Mod(); data->hf_phi = hfmet.Phi();
		data->trk_pt = tkmet.Mod(); data->trk_phi = tkmet.Phi();
		data->neutral_pt = ntrlmet.Mod(); data->neutral_phi = ntrlmet.Phi();
		data->photon_pt = phomet.Mod(); data->photon_phi = phomet.Phi(); 


		return 0;
}

