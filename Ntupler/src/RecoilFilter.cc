#include "PandaProd/Ntupler/interface/RecoilFilter.h"

using namespace panda;

RecoilFilter::RecoilFilter(TString n):
		BaseFiller()
{
	treename = n;
}

RecoilFilter::~RecoilFilter(){
}

void RecoilFilter::init(TTree *t) {
	t->Branch("filter_maxRecoil",&maxRecoil,"filter_maxRecoil/F");
	t->Branch("filter_whichRecoil",&whichRecoil,"filter_whichRecoil/I");
}

bool RecoilFilter::isGoodMuon(const pat::Muon& muon){
		if (muon.pt()<15) return false;
		if (not muon.isLooseMuon()) return false;
		if (muon.pfIsolationR04().sumChargedHadronPt/muon.pt()>0.12) return false;
		return true;
}

bool RecoilFilter::isGoodElectron(const pat::Electron& ele){
		if (ele.pt()<15) return false;
		if (ele.chargedHadronIso()/ele.pt()>0.1) return false;
		bool isEB = ele.isEB();
		bool isEE = ele.isEE();
		if (isEB and ele.full5x5_sigmaIetaIeta()>0.011) return false;
		if (isEE and ele.full5x5_sigmaIetaIeta()>0.030) return false;
		return true;
}

bool RecoilFilter::isGoodPhoton(const pat::Photon& photon){
		if (photon.chargedHadronIso()>10) return false;
		if (not photon.isEB()) return false;
		if (photon.full5x5_sigmaIetaIeta()>0.020) return false;
		return true;
}

void RecoilFilter::endJob() {
	PInfo("PandaProd::RecoilFilter",TString::Format("Accepted %i/%i events",nAcc,nTotal));
}

int RecoilFilter::analyze(const edm::Event& iEvent){
		bool keep=false;
		maxRecoil = -1;
		whichRecoil = -1;
		++nTotal;

		using namespace edm;
		iEvent.getByToken(met_token, met_handle);
		iEvent.getByToken(puppimet_token, puppimet_handle);
		iEvent.getByToken(mu_token, mu_handle);
		iEvent.getByToken(el_token, el_handle);
		iEvent.getByToken(ph_token, ph_handle);

		std::vector<reco::Candidate::LorentzVector> mets;
		for (auto met = met_handle->begin(); met!=met_handle->end(); met++){
				mets.push_back(met->p4());
		}

		for (auto met = puppimet_handle->begin(); met!=puppimet_handle->end(); met++) {
				mets.push_back(met->p4());
		}
		
		float U=0;
		for (unsigned iM=0; iM!=2; ++iM) {
				reco::Candidate::LorentzVector met = mets.at(iM);
				U = met.pt();
				if (U>maxRecoil) {
					maxRecoil = U;
					whichRecoil = 2*0+iM;
				}
				if (U>minU) {
					keep = true;
				}
				
				// loop over leptons to get W+jets events
				for (pat::MuonCollection::const_iterator muon = mu_handle->begin(); muon!=mu_handle->end(); muon++){
						if (not isGoodMuon(*muon)) continue;
						U = (met+muon->p4()).pt();
						if (U>maxRecoil) {
							maxRecoil = U;
							whichRecoil = 2*1+iM;
						}
						if (U>minU) {
							keep = true;
						}
				}
				for (pat::ElectronCollection::const_iterator ele = el_handle->begin(); ele!=el_handle->end(); ele++){
						if (not isGoodElectron(*ele)) continue;
						U = (met+ele->p4()).pt();
						if (U>maxRecoil) {
							maxRecoil = U;
							whichRecoil = 2*2+iM;
						}
						if (U>minU) {
							keep = true;
						}
				}

				// loop over dilepton pairs
				for (unsigned int imuon=0; imuon+1<mu_handle->size(); imuon++){
						if (not isGoodMuon(mu_handle->at(imuon))) continue;
						for (unsigned int jmuon=imuon+1; jmuon<mu_handle->size(); jmuon++){
								if (not isGoodMuon(mu_handle->at(jmuon))) continue;
								U = (met+mu_handle->at(imuon).p4()+mu_handle->at(jmuon).p4()).pt();
								if (U>maxRecoil) {
									maxRecoil = U;
									whichRecoil = 2*3+iM;
								}
								if (U>minU) {
									keep = true;
								}
						}					 
				}
				for (unsigned int iele=0; iele+1<el_handle->size(); iele++){
						if (not isGoodElectron(el_handle->at(iele))) continue;
						for (unsigned int jele=iele+1; jele<el_handle->size(); jele++){
								if (not isGoodElectron(el_handle->at(jele))) continue;
								U = (met+el_handle->at(iele).p4()+el_handle->at(jele).p4()).pt();
								if (U>maxRecoil) {
									maxRecoil = U;
									whichRecoil = 2*4+iM;
								}
								if (U>minU) {
									keep = true;
								}
						}					 
				}
						
				for (unsigned int ipho=0; ipho<ph_handle->size(); ipho++){
						if (not isGoodPhoton(ph_handle->at(ipho))) continue;
						U = (met+ph_handle->at(ipho).p4()).pt();
						if (U>maxRecoil) {
							maxRecoil = U;
							whichRecoil = 2*5+iM;
						}
						if (U>minU) {
							keep = true;
						}
				}
		}

		*reduceEvent = !keep;

		if (keep)
			++nAcc;
	
		return 0;
}

