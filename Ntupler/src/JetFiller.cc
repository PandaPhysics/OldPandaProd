#include "..//interface/JetFiller.h"
#include "../interface/JetIDFunc.h"


using namespace panda;

JetFiller::JetFiller(TString n):
		BaseFiller()
{
	data = new VJet();
	treename = n;
}

JetFiller::~JetFiller(){
	delete data;
}

void JetFiller::init(TTree *t) {
	t->Branch(treename.Data(),&data);
	
	if (applyJEC) {
	 std::string jecDir = "jec/";

	 std::vector<JetCorrectorParameters> mcParams;
	 mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L1FastJet_AK4PFPuppi.txt"));
	 mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L2Relative_AK4PFPuppi.txt"));
	 mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L3Absolute_AK4PFPuppi.txt"));
	 mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L2L3Residual_AK4PFPuppi.txt"));
	 mMCJetCorrector = new FactorizedJetCorrector(mcParams);
 
	 std::vector<JetCorrectorParameters> dataParams;
	 dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L1FastJet_AK4PFPuppi.txt"));
	 dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L2Relative_AK4PFPuppi.txt"));
	 dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L3Absolute_AK4PFPuppi.txt"));
	 dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L2L3Residual_AK4PFPuppi.txt"));
	 mDataJetCorrector = new FactorizedJetCorrector(dataParams);

	}
}

int JetFiller::analyze(const edm::Event& iEvent){
		// data->Clear();
		for (auto d : *data)
			delete d;
		data->clear(); 


		if (skipEvent!=0 && *skipEvent) {
			return 0;
		}

		iEvent.getByToken(jet_token, jet_handle);
		if (applyJEC) 
			iEvent.getByToken(rho_token,rho_handle);

		FactorizedJetCorrector *corrector=0;
		if (applyJEC) 
			corrector = ( iEvent.isRealData() ) ? mDataJetCorrector : mMCJetCorrector;

		for (const pat::Jet& j : *jet_handle) {
			if (fabs(j.eta())>maxEta) continue;

			double this_pt = j.pt(), this_rawpt=0, jecFactor=1;

			// PDebug(TString::Format("JetFiller::%s",treename.Data()),
			//				TString::Format("corr pT=%.3f, uncorr pT=%.3f",
			//												this_pt,
			//												this_pt*j.jecFactor("Uncorrected")));

			if (applyJEC) {
				this_rawpt = this_pt;
				if (fabs(j.eta())<5.191) {
					corrector->setJetPt(j.pt());
					corrector->setJetEta(j.eta());
					corrector->setJetPhi(j.phi());
					corrector->setJetE(j.energy());
					corrector->setRho(*rho_handle);
					corrector->setJetA(j.jetArea());
					corrector->setJetEMF(-99.0);
					jecFactor = corrector->getCorrection();
					this_pt *= jecFactor;
				}
			} else {
					this_rawpt = j.pt()*j.jecFactor("Uncorrected");
			}

			if (this_pt < minPt && this_rawpt < minPt) continue;

			PJet *jet = new PJet();

			jet->pt = this_pt;
			jet->rawPt = this_rawpt;
			jet->eta = j.eta();
			jet->phi = j.phi();
			jet->m = j.mass();
			jet->area = j.jetArea();
			jet->csv = j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");

			// PDebug(TString::Format("JetFiller::%s",treename.Data()),
			//				TString::Format("csv=%.3f",jet->csv));

			jet->id = 0;
			jet->id |= PassJetID(j,PJet::kLoose) * PJet::kLoose;
			jet->id |= PassJetID(j,PJet::kTight) * PJet::kTight;
			jet->id |= PassJetID(j,PJet::kMonojet) * PJet::kMonojet;
			jet->nhf = j.neutralHadronEnergyFraction();
			jet->chf = j.chargedHadronEnergyFraction();
			
			data->push_back(jet);

		}

		std::sort(data->begin(),data->end(),SortPObjects);

		return 0;
}
