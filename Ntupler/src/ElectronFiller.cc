#include "PandaProd/Ntupler/interface/ElectronFiller.h"

using namespace panda;

ElectronFiller::ElectronFiller(TString n):
		BaseFiller()
{
	data = new VElectron();
	treename = n;
}

ElectronFiller::~ElectronFiller(){
	delete data;
}

void ElectronFiller::init(TTree *t) {
	t->Branch(treename.Data(),&data);
}

int ElectronFiller::analyze(const edm::Event& iEvent){
		for (auto d : *data)
			delete d;
		data->clear(); 

		if (skipEvent!=0 && *skipEvent) {
			return 0;
		}

		iEvent.getByToken(el_token, el_handle);
		iEvent.getByToken(el_vetoid_token,el_vetoid_handle);
		iEvent.getByToken(el_looseid_token,el_looseid_handle);
		iEvent.getByToken(el_mediumid_token,el_mediumid_handle);
		iEvent.getByToken(el_tightid_token,el_tightid_handle);
		iEvent.getByToken(el_hltid_token,el_hltid_handle);
		iEvent.getByToken(el_unsmeared_token, el_unsmeared_handle);

		unsigned int iE=-1;
		for (const pat::Electron& el : *el_handle) {

			++iE;

			// PDebug("ElectronFiller",TString::Format("Considering electron with pT=%.3f, eta=%.3f",el.pt(),el.eta()));

			if (el.pt()<minPt || fabs(el.eta())>maxEta || !(el.passConversionVeto()) ) {
				continue;
			}

			// PDebug("ElectronFiller",TString::Format("Passed kinematics and conv veto"));

			edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iE) ) ;

			bool veto = (*el_vetoid_handle)[ref];
			bool medium = (*el_mediumid_handle)[ref];
			bool loose = (*el_looseid_handle)[ref];
			bool tight = (*el_tightid_handle)[ref];
			bool hltsafe = (*el_hltid_handle)[ref];

			if (!veto) {
				continue;
			}

			// PDebug("ElectronFiller",TString::Format("Passed veto"));


			// compute isolation
			float chiso = el.pfIsolationVariables().sumChargedHadronPt;
			float nhiso = el.pfIsolationVariables().sumNeutralHadronEt;
			float phoiso = el.pfIsolationVariables().sumPhotonEt;
			float puiso= el.puChargedHadronIso();

			double ea = 0.; // effective area
			ea = effArea->getEffectiveArea(el.eta());

			float iso = chiso + TMath::Max(nhiso+phoiso-(evt->rho()*ea),(double)0);

			// fill
			PElectron *electron = new PElectron();

			electron->pt = el.pt();
			electron->eta = el.eta();
			electron->phi = el.phi();
			electron->m = el.mass();
			electron->q = el.charge();
			electron->iso = iso;
			electron->chiso = chiso;
			electron->nhiso = nhiso;
			electron->phoiso = phoiso;
			electron->puiso = puiso;

			electron->id = 0;
			electron->id |= (unsigned(veto)*PElectron::kVeto);
			electron->id |= (unsigned(loose)*PElectron::kLoose);
			electron->id |= (unsigned(medium)*PElectron::kMedium);
			electron->id |= (unsigned(tight)*PElectron::kTight);
			electron->id |= (unsigned(hltsafe)*PElectron::kHLTPresel);

			// get the unsmeared momentum
			electron->pt_unsmeared = (*el_unsmeared_handle)[iE].pt();

			data->push_back(electron);
		}

		std::sort(data->begin(),data->end(),SortPObjects);

		return 0;
}

