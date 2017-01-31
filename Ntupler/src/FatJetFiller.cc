#include "../interface/FatJetFiller.h"
#include "../interface/JetIDFunc.h"
#include "DataFormats/BTauReco/interface/TaggingVariable.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/BoostedDoubleSVTagInfo.h"

using namespace panda;

FatJetFiller::FatJetFiller(TString n):
		BaseFiller()
{
	data = new VFatJet();
	treename = n;

	int activeAreaRepeats = 1;
	double ghostArea = 0.01;
	double ghostEtaMax = 7.0;
	activeArea = new fastjet::GhostedAreaSpec(ghostEtaMax,activeAreaRepeats,ghostArea);
	areaDef = new fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts,*activeArea);

	ecfnmanager = new ECFNManager();
}

FatJetFiller::~FatJetFiller(){
	delete data;
	delete activeArea;
	delete areaDef;
	delete jetDefCA;
	delete softdrop;
	delete tau;
	delete ecfnmanager;
	delete htt;
	delete mMCJetCorrector;
	for (auto& iter : mDataJetCorrectors)
		delete iter.second;
	delete eras;
}

void FatJetFiller::init(TTree *t) {
	t->Branch(treename.Data(),&data,99);
	std::string jecDir = "jec/23Sep2016V3/";
 
	std::vector<JetCorrectorParameters> mcParams;
	mcParams.push_back(
			JetCorrectorParameters(
				jecDir + "Summer16_23Sep2016V3_MC/Summer16_23Sep2016V3_MC_L1FastJet_AK8PFPuppi.txt"));
	mcParams.push_back(
			JetCorrectorParameters(
				jecDir + "Summer16_23Sep2016V3_MC/Summer16_23Sep2016V3_MC_L2Relative_AK8PFPuppi.txt"));
	mcParams.push_back(
			JetCorrectorParameters(
				jecDir + "Summer16_23Sep2016V3_MC/Summer16_23Sep2016V3_MC_L3Absolute_AK8PFPuppi.txt"));
	mcParams.push_back(
			JetCorrectorParameters(
				jecDir + "Summer16_23Sep2016V3_MC/Summer16_23Sep2016V3_MC_L2L3Residual_AK8PFPuppi.txt"));
	mMCJetCorrector = new FactorizedJetCorrector(mcParams);
 
	std::vector<std::string> eraGroups = {"BCD","EF","G","H"};
	std::map<std::string,std::vector<JetCorrectorParameters>> dataParams;
	for (auto e : eraGroups) {
		dataParams[e].clear();
		dataParams[e].push_back(
				JetCorrectorParameters(
					jecDir + "Summer16_23Sep2016"+e+"V3_DATA/Summer16_23Sep2016"+e+"V3_DATA_L1FastJet_AK8PFPuppi.txt"));
		dataParams[e].push_back(
				JetCorrectorParameters(
					jecDir + "Summer16_23Sep2016"+e+"V3_DATA/Summer16_23Sep2016"+e+"V3_DATA_L2Relative_AK8PFPuppi.txt"));
		dataParams[e].push_back(
				JetCorrectorParameters(
					jecDir + "Summer16_23Sep2016"+e+"V3_DATA/Summer16_23Sep2016"+e+"V3_DATA_L3Absolute_AK8PFPuppi.txt"));
		dataParams[e].push_back(
				JetCorrectorParameters(
					jecDir + "Summer16_23Sep2016"+e+"V3_DATA/Summer16_23Sep2016"+e+"V3_DATA_L2L3Residual_AK8PFPuppi.txt"));
		mDataJetCorrectors[e.c_str()] = new FactorizedJetCorrector(dataParams[e]);
	}
	eras = new EraHandler(2016);

	jetDefCA = new fastjet::JetDefinition(fastjet::cambridge_algorithm, radius);

	double sdZcut, sdBeta;
	if (radius<1) {
		sdZcut=0.1; sdBeta=0.;
	} else {
		sdZcut=0.15; sdBeta=1.;
	}
	softdrop = new fastjet::contrib::SoftDrop(sdBeta,sdZcut,radius);

	fastjet::contrib::OnePass_KT_Axes onepass;
	tau = new fastjet::contrib::Njettiness(onepass, fastjet::contrib::NormalizedMeasure(1., radius));

	//htt
	bool optimalR=true; bool doHTTQ=false;
	double minSJPt=0.; double minCandPt=0.;
	double sjmass=30.; double mucut=0.8;
	double filtR=0.3; int filtN=5;
	int mode=4; double minCandMass=0.;
	double maxCandMass=9999999.; double massRatioWidth=9999999.;
	double minM23Cut=0.; double minM13Cut=0.;
	double maxM13Cut=9999999.;	bool rejectMinR=false;
	htt = new fastjet::HEPTopTaggerV2(optimalR,doHTTQ,
													 minSJPt,minCandPt,
													 sjmass,mucut,
													 filtR,filtN,
													 mode,minCandMass,
													 maxCandMass,massRatioWidth,
													 minM23Cut,minM13Cut,
													 maxM13Cut,rejectMinR);

	// doubleb
	std::string cmssw_base = getenv("CMSSW_BASE");
	std::string fweight = cmssw_base+"/src/PandaProd/Utilities/data/BoostedSVDoubleCA15_withSubjet_v4.weights.xml";
	mJetBoostedBtaggingMVACalc.initialize("BDT",fweight);

}

int FatJetFiller::analyze(const edm::Event& iEvent){
		for (auto d : *data)
			delete d;
		data->clear();

		if (SkipEvent())
			return 0;

		iEvent.getByToken(jet_token, jet_handle);
		iEvent.getByToken(rho_token,rho_handle);
		iEvent.getByToken(subjets_token,subjets_handle);
		iEvent.getByToken(btags_token,btags_handle);
		iEvent.getByToken(doubleb_token,doubleb_handle);
		iEvent.getByToken(qgl_token,qgl_handle);

		FactorizedJetCorrector *corrector=0;
		if (iEvent.isRealData()) {
			int thisRun = iEvent.id().run();
			TString thisEra = eras->getEra(thisRun);
			TString thisEraGroup;
			for (auto &iter : mDataJetCorrectors) {
				if (iter.first.Contains(thisEra)) {
					thisEraGroup = iter.first;
					corrector = iter.second;
					break;
				}
			}
		} else {
			corrector = mMCJetCorrector;
		}
		if (corrector==0) {
			PError("FatJetFiller::analyze",TString::Format("Could not determine data era for run %i",(int)iEvent.id().run()));
			assert(corrector!=0);
		}

		const reco::PFJetCollection *subjetCol = subjets_handle.product();

		int ijetRef=-1;
		for (const pat::Jet &j : *jet_handle) {

			ijetRef++;

			if (fabs(j.eta())>maxEta) continue;

			double this_pt = j.pt(), this_rawpt=j.pt(), jecFactor=1;
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

			if (this_pt < minPt || this_rawpt < minPt) continue;

			PFatJet *jet = new PFatJet();

			jet->pt = this_pt;
			jet->rawPt = this_rawpt;
			jet->eta = j.eta();
			jet->phi = j.phi();
			jet->m = j.mass();
			jet->area = j.jetArea();
			
			jet->tau1 = j.userFloat(treename+"Njettiness:tau1");
			jet->tau2 = j.userFloat(treename+"Njettiness:tau2");
			jet->tau3 = j.userFloat(treename+"Njettiness:tau3");
			jet->mSD	= j.userFloat(treename+"SDKinematics:Mass");

			jet->id = 0;
			jet->id |= PassJetID(j,PJet::kLoose) * PJet::kLoose;
			jet->id |= PassJetID(j,PJet::kTight) * PJet::kTight;
			jet->id |= PassJetID(j,PJet::kMonojet) * PJet::kMonojet;
			jet->nhf = j.neutralHadronEnergyFraction();
			jet->chf = j.chargedHadronEnergyFraction();

			jet->subjets = new VJet();
			VJet *subjet_data = jet->subjets;

			for (reco::PFJetCollection::const_iterator i = subjetCol->begin(); i!=subjetCol->end(); ++i) {

				if (reco::deltaR(i->eta(),i->phi(),j.eta(),j.phi())>jetRadius) 
					continue;

				PJet *subjet = new PJet();

				subjet->pt = i->pt();
				subjet->eta = i->eta();
				subjet->phi = i->phi();
				subjet->m = i->mass();

				reco::JetBaseRef sjBaseRef(reco::PFJetRef(subjets_handle,i-subjetCol->begin()));
				subjet->csv = (float)(*(btags_handle.product()))[sjBaseRef];
				subjet->qgl = (float)(*(qgl_handle.product()))[sjBaseRef];

				subjet_data->push_back(subjet);
				
			}

			// reset the ECFs
			std::vector<float> betas = {0.5,1.,2.,4.};
			std::vector<int> Ns = {1,2,3,4};
			std::vector<int> orders = {1,2,3};
			for (unsigned int iB=0; iB!=4; ++iB) {
				for (auto N : Ns) {
					for (auto o : orders) {
						jet->set_ecf(o,N,iB,-1);
					}
				}
			}

			if (!ReduceEvent() && (pfcands!=0 || (!minimal && data->size()<2))) {
				// either we want to associate to pf cands OR compute extra info about the first or second jet
				// but do not do any of this if ReduceEvent() is tripped

				std::vector<edm::Ptr<reco::Candidate>> constituentPtrs = j.getJetConstituents();

				if (pfcands!=0) { // associate to pf cands in tree
					const std::map<const reco::Candidate*,UShort_t> &pfmap = pfcands->get_map();
					jet->constituents = new std::vector<UShort_t>();
					std::vector<UShort_t> *constituents = jet->constituents;

					for (auto ptr : constituentPtrs) {
						const reco::Candidate *constituent = ptr.get();

						auto result_ = pfmap.find(constituent); // check if we know about this pf cand
						if (result_ == pfmap.end()) {
							PError("PandaProdNtupler::FatJetFiller",TString::Format("could not PF [%s] ...\n",treename.Data()));
						} else {
							constituents->push_back(result_->second);
						} 
					} // loop through constituents from input
				} // pfcands!=0 

				if (!minimal && data->size()<2) { 
					// calculate ECFs, groomed tauN
					VPseudoJet vjet;
					for (auto ptr : constituentPtrs) { 
						// create vector of PseudoJets
						const reco::Candidate *constituent = ptr.get();
						if (constituent->pt()<0.01) 
							continue;
						vjet.emplace_back(constituent->px(),constituent->py(),constituent->pz(),constituent->energy());
					}
					fastjet::ClusterSequenceArea seq(vjet, *jetDefCA, *areaDef); 
					VPseudoJet alljets = fastjet::sorted_by_pt(seq.inclusive_jets(0.1));
					if (alljets.size()>0){
						fastjet::PseudoJet *leadingJet = &(alljets[0]);
						fastjet::PseudoJet sdJet = (*softdrop)(*leadingJet);

						// get and filter constituents of groomed jet
						VPseudoJet sdconsts = fastjet::sorted_by_pt(sdJet.constituents());
						int nFilter = TMath::Min(100,(int)sdconsts.size());
						VPseudoJet sdconstsFiltered(sdconsts.begin(),sdconsts.begin()+nFilter);

						// calculate ECFs
						for (unsigned int iB=0; iB!=4; ++iB) {
							calcECFN(betas[iB],sdconstsFiltered,ecfnmanager); // calculate for all Ns and os
							for (auto N : Ns) {
								for (auto o : orders) {
									float x = ecfnmanager->ecfns[TString::Format("%i_%i",N,o)];
									int r = jet->set_ecf(o,N,iB,x);
									if (r) {
										PError("PandaProd::Ntupler::FatJetFiller",
												TString::Format("Could not save o=%i, N=%i, iB=%i",o,N,(int)iB));
									}
								} // o loop
							} // N loop
						} // beta loop

						jet->tau3SD = tau->getTau(3,sdconsts);
						jet->tau2SD = tau->getTau(2,sdconsts);
						jet->tau1SD = tau->getTau(1,sdconsts);

						// HTT
						fastjet::PseudoJet httJet = htt->result(*leadingJet);
						if (httJet!=0) {
							fastjet::HEPTopTaggerV2Structure *s = 
								(fastjet::HEPTopTaggerV2Structure*)httJet.structure_non_const_ptr();
							jet->htt_mass = s->top_mass();
							jet->htt_frec = s->fRec();
						}
			
					} else {
						PError("PandaProd::Ntupler::FatJetFiller","Jet could not be clustered");
					}

					// now we do the double-b
					reco::BoostedDoubleSVTagInfoCollection::const_iterator matchTI = doubleb_handle->end();
					for(reco::BoostedDoubleSVTagInfoCollection::const_iterator itTI = doubleb_handle->begin(); 
							itTI != doubleb_handle->end(); ++itTI ) 
					{
						const reco::JetBaseRef jetTI = itTI->jet();
						if( fabs((jetTI->px()-j.px())/j.px()) < 0.01  
								&& fabs((jetTI->pz()-j.pz())/j.pz()) < 0.01 ) 
						{
							matchTI = itTI;
							break;
						}
					}
					
					if( matchTI != doubleb_handle->end() ) {
						float SubJet_csv_ = 999;
						for (auto *sj : *subjet_data) {
							SubJet_csv_ = TMath::Min(SubJet_csv_,sj->csv);
						}
						if ((SubJet_csv_ < -1) || (SubJet_csv_ > 1)) SubJet_csv_ = -1;

						const reco::TaggingVariableList vars = matchTI->taggingVariables();
						float z_ratio_ = vars.get(reco::btau::z_ratio);
						float trackSipdSig_3_ = vars.get(reco::btau::trackSip3dSig_3);
						float trackSipdSig_2_ = vars.get(reco::btau::trackSip3dSig_2);
						float trackSipdSig_1_ = vars.get(reco::btau::trackSip3dSig_1);
						float trackSipdSig_0_ = vars.get(reco::btau::trackSip3dSig_0);
						float trackSipdSig_1_0_ = vars.get(reco::btau::tau2_trackSip3dSig_0);
						float trackSipdSig_0_0_ = vars.get(reco::btau::tau1_trackSip3dSig_0);
						float trackSipdSig_1_1_ = vars.get(reco::btau::tau2_trackSip3dSig_1);
						float trackSipdSig_0_1_ = vars.get(reco::btau::tau1_trackSip3dSig_1);
						float trackSip2dSigAboveCharm_0_ = vars.get(reco::btau::trackSip2dSigAboveCharm);
						float trackSip2dSigAboveBottom_0_ = vars.get(reco::btau::trackSip2dSigAboveBottom_0);
						float trackSip2dSigAboveBottom_1_ = vars.get(reco::btau::trackSip2dSigAboveBottom_1);
						float tau1_trackEtaRel_0_ = vars.get(reco::btau::tau2_trackEtaRel_0);
						float tau1_trackEtaRel_1_ = vars.get(reco::btau::tau2_trackEtaRel_1);
						float tau1_trackEtaRel_2_ = vars.get(reco::btau::tau2_trackEtaRel_2);
						float tau0_trackEtaRel_0_ = vars.get(reco::btau::tau1_trackEtaRel_0);
						float tau0_trackEtaRel_1_ = vars.get(reco::btau::tau1_trackEtaRel_1);
						float tau0_trackEtaRel_2_ = vars.get(reco::btau::tau1_trackEtaRel_2);
						float tau_vertexMass_0_ = vars.get(reco::btau::tau1_vertexMass);
						float tau_vertexEnergyRatio_0_ = vars.get(reco::btau::tau1_vertexEnergyRatio);
						float tau_vertexDeltaR_0_ = vars.get(reco::btau::tau1_vertexDeltaR);
						float tau_flightDistance2dSig_0_ = vars.get(reco::btau::tau1_flightDistance2dSig);
						float tau_vertexMass_1_ = vars.get(reco::btau::tau2_vertexMass);
						float tau_vertexEnergyRatio_1_ = vars.get(reco::btau::tau2_vertexEnergyRatio);
						float tau_flightDistance2dSig_1_ = vars.get(reco::btau::tau2_flightDistance2dSig);
						float jetNTracks_ = vars.get(reco::btau::jetNTracks);
						float nSV_ = vars.get(reco::btau::jetNSecondaryVertices);
						float massPruned_ =jet->m;
						float flavour_ = -1;   //j.partonFlavor();   // they're spectator variables
						float nbHadrons_ = -1; //j.hadronFlavor(); // 
						float ptPruned_ =j.pt();
						float etaPruned_ =j.eta();
	
						jet->double_sub = mJetBoostedBtaggingMVACalc.mvaValue(
							         	massPruned_,
							         	flavour_,
							         	nbHadrons_,
							         	ptPruned_,
							         	etaPruned_,
							         	SubJet_csv_,
							         	z_ratio_,
							         	trackSipdSig_3_,
							         	trackSipdSig_2_,
							         	trackSipdSig_1_,
							         	trackSipdSig_0_,
							         	trackSipdSig_1_0_,
							         	trackSipdSig_0_0_,
							         	trackSipdSig_1_1_,
							         	trackSipdSig_0_1_,
							         	trackSip2dSigAboveCharm_0_,
							         	trackSip2dSigAboveBottom_0_,
							         	trackSip2dSigAboveBottom_1_,
							         	tau0_trackEtaRel_0_,
							         	tau0_trackEtaRel_1_,
							         	tau0_trackEtaRel_2_,
							         	tau1_trackEtaRel_0_,
							         	tau1_trackEtaRel_1_,
							         	tau1_trackEtaRel_2_,
							         	tau_vertexMass_0_,
							         	tau_vertexEnergyRatio_0_,
							         	tau_vertexDeltaR_0_,
							         	tau_flightDistance2dSig_0_,
							         	tau_vertexMass_1_,
							         	tau_vertexEnergyRatio_1_,
							         	tau_flightDistance2dSig_1_,
							         	jetNTracks_,
							         	nSV_,
							         	false
								);
					} // if matchTI is good
					

				} // if not minimal and fewer than 2 
			} // if extras are requested

			data->push_back(jet);
	
		}


		return 0;
}

