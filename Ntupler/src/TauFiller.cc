#include "PandaProd/Ntupler/interface/TauFiller.h"

using namespace panda;

TauFiller::TauFiller(TString n):
		BaseFiller()
{
	data = new VTau();
	treename = n;
}

TauFiller::~TauFiller(){
	delete data;
}

void TauFiller::init(TTree *t) {
	t->Branch(treename.Data(),&data);
}

int TauFiller::analyze(const edm::Event& iEvent){
		for (auto d : *data)
			delete d;
		data->clear(); 

		if (skipEvent!=0 && *skipEvent) {
			return 0;
		}

		iEvent.getByToken(tau_token, tau_handle);

		for (const pat::Tau& tau : *tau_handle) {
			if (tau.pt()<minPt || fabs(tau.eta())>maxEta) 
				continue;

			if (!(tau.tauID("decayModeFindingNewDMs")))
				continue;

			float isodbcorr = tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");

			if (isodbcorr>maxIso)
				continue;

			float phoiso = 0.; for(auto cand : tau.isolationGammaCands() ) phoiso += cand->pt();
			float chiso	= 0.; for(auto cand : tau.isolationChargedHadrCands() ) chiso += cand->pt();
			float nhiso	= 0.; for(auto cand : tau.isolationNeutrHadrCands() ) nhiso += cand->pt();
			float iso = phoiso + chiso + nhiso;
				

			PTau *tauon = new PTau();

			tauon->pt = tau.pt();
			tauon->eta = tau.eta();
			tauon->phi = tau.phi();
			tauon->m = tau.mass();
			tauon->q = tau.charge();
			tauon->iso = iso;
			tauon->isoDeltaBetaCorr = isodbcorr;

			tauon->id = 0;
			tauon->id |= (unsigned(tau.tauID("decayModeFindingNewDMs"))
					          *PTau::kBaseline);
			tauon->id |= (unsigned(tau.tauID("decayModeFindingNewDMs"))
					          *PTau::kDecayModeFindingNewDMs);
			tauon->id |= (unsigned(tau.tauID("decayModeFinding"))
					          *PTau::kDecayModeFinding);
			tauon->id |= (unsigned(tau.tauID("byVLooseIsolationMVArun2v1DBnewDMwLT"))
					          *PTau::kVLooseIsolationMVArun2v1DBnewDMwLT);

			data->push_back(tauon);

		}

		std::sort(data->begin(),data->end(),SortPObjects);

		return 0;
}

