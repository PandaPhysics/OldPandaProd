#include "../interface/FatJetFiller.h"
#include "../interface/functions/JetIDFunc.h"

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
  delete mDataJetCorrector;
}

void FatJetFiller::init(TTree *t) {
  t->Branch(treename.Data(),&data,99);
  std::string jecDir = "jec/";
 
  std::vector<JetCorrectorParameters> mcParams;
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L1FastJet_AK8PFPuppi.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L2Relative_AK8PFPuppi.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L3Absolute_AK8PFPuppi.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_MC_L2L3Residual_AK8PFPuppi.txt"));
  mMCJetCorrector = new FactorizedJetCorrector(mcParams);
 
  std::vector<JetCorrectorParameters> dataParams;
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L1FastJet_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L2Relative_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L3Absolute_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV6_DATA_L2L3Residual_AK8PFPuppi.txt"));
  mDataJetCorrector = new FactorizedJetCorrector(dataParams);

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
  double maxM13Cut=9999999.;  bool rejectMinR=false;
  htt = new fastjet::HEPTopTaggerV2(optimalR,doHTTQ,
                           minSJPt,minCandPt,
                           sjmass,mucut,
                           filtR,filtN,
                           mode,minCandMass,
                           maxCandMass,massRatioWidth,
                           minM23Cut,minM13Cut,
                           maxM13Cut,rejectMinR);

}

int FatJetFiller::analyze(const edm::Event& iEvent){
    for (auto d : *data)
      delete d;
    data->clear();

    if (skipEvent!=0 && *skipEvent) {
      return 0;
    }

    iEvent.getByToken(jet_token, jet_handle);
    iEvent.getByToken(rho_token,rho_handle);
    iEvent.getByToken(subjets_token,subjets_handle);
    iEvent.getByToken(btags_token,btags_handle);
    iEvent.getByToken(qgl_token,qgl_handle);

    FactorizedJetCorrector *corrector=0;
    corrector = ( iEvent.isRealData() ) ? mDataJetCorrector : mMCJetCorrector;

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
      
      jet->tau1 = j.userFloat(treename+"Njettiness:tau1");
      jet->tau2 = j.userFloat(treename+"Njettiness:tau2");
      jet->tau3 = j.userFloat(treename+"Njettiness:tau3");
      jet->mSD  = j.userFloat(treename+"SDKinematics:Mass");

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

      if (pfcands!=0 || (!minimal && data->size()<2)) {
        // either we want to associate to pf cands OR compute extra info about the first or second jet

        std::vector<edm::Ptr<reco::Candidate>> constituentPtrs = j.getJetConstituents();

        if (pfcands!=0) { // associate to pf cands in tree
          const std::map<const reco::Candidate*,UShort_t> &pfmap = pfcands->get_map();
          jet->constituents = new std::vector<UShort_t>();
          std::vector<UShort_t> *constituents = jet->constituents;

          for (auto ptr : constituentPtrs) {
            //const reco::PFCandidate *constituent = ptr.get();
            const reco::Candidate *constituent = ptr.get();

            auto result_ = pfmap.find(constituent);
            if (result_ == pfmap.end()) {
              PError("PandaProdNtupler::FatJetFiller",TString::Format("could not PF [%s] ...\n",treename.Data()));
            } else {
              constituents->push_back(result_->second);
            }
          }
        } 

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
            std::vector<float> betas = {0.5,1.,2.,4.};
            std::vector<int> Ns = {1,2,3,4};
            std::vector<int> orders = {1,2,3};
            for (unsigned int iB=0; iB!=4; ++iB) {
              calcECFN(betas[iB],sdconstsFiltered,ecfnmanager);
              for (auto N : Ns) {
                for (auto o : orders) {
                  float x = ecfnmanager->ecfns[TString::Format("%i_%i",N,o)];
                  int r = jet->set_ecf(o,N,iB,x);
                  if (r) {
                    PError("PandaProd::Ntupler::FatJetFiller",
                        TString::Format("Could not save o=%i, N=%i, iB=%i",o,N,(int)iB));
                  }
                }
              }
            }

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

        } 
      }

      data->push_back(jet);
  
    }


    return 0;
}

/*
bool FatJetFiller::JetId(const pat::Jet &j, std::string id)
{

  bool jetid = false;

  float NHF    = j.neutralHadronEnergyFraction();
  float NEMF   = j.neutralEmEnergyFraction();
  float CHF    = j.chargedHadronEnergyFraction();
  //float MUF    = j.muonEnergyFraction();                                                                                                                                                                
  float CEMF   = j.chargedEmEnergyFraction();
  int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
  int CHM      = j.chargedMultiplicity();
  int NumNeutralParticle =j.neutralMultiplicity();
  float eta = j.eta();

  if (id=="loose" || id=="monojet" )
    {
      jetid = (NHF<0.99 && NEMF<0.99 && NumConst>1) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) && fabs(eta)<=3.0;
      jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0);
    }

  if (id=="tight")
    {
      jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) && fabs(eta)<=3.0;
      jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0 );
    }

  if (id=="monojet")
    jetid = jetid && (NHF < 0.8 && CHF > 0.1);

  return jetid;
  
}
*/
