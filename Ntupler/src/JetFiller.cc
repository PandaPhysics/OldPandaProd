#include "PandaProd/Ntupler/interface/JetFiller.h"

using namespace panda;

JetFiller::JetFiller(TString n):
    BaseFiller()
{
  // data = new TClonesArray("panda::PJet",100);
  data = new VJet();
  treename = n;
}

JetFiller::~JetFiller(){
  delete data;
}

void JetFiller::init(TTree *t) {
//  PJet::Class()->IgnoreTObjectStreamer();
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

      if (this_pt < minPt || this_rawpt < minPt) continue;

      // const int idx = data->GetEntries();
      // assert(idx<data->GetSize());

      // new((*data)[idx]) PJet();
      // PJet *jet = (PJet*)data->At(idx);

      PJet *jet = new PJet();

      jet->pt = this_pt;
      jet->rawPt = this_rawpt;
      jet->eta = j.eta();
      jet->phi = j.phi();
      jet->m = j.mass();
      jet->csv = j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");

      jet->id |= JetId(j,"loose") * PJet::kLoose;
      jet->id |= JetId(j,"tight") * PJet::kTight;
      jet->id |= JetId(j,"monojet") * PJet::kMonojet;
      
      data->push_back(jet);

    }

    std::sort(data->begin(),data->end(),SortPObjects);

    return 0;
}

bool JetFiller::JetId(const pat::Jet &j, std::string id)
{
  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
  //                              Loose -- Tight Jet ID                                                                                                                                                 
  // --- Number of Constituents   > 1     > 1
  // --- Neutral Hadron Fraction  < 0.99  < 0.90                                                                                                                                                        
  // --- Neutral EM Fraction      < 0.99  < 0.90
  // --- Muon Fraction    < 0.8   < 0.8                                                                                                                                                                 
  // --- And for -2.4 <= eta <= 2.4 in addition apply
  // --- Charged Hadron Fraction  > 0     > 0                                                                                                                                                           
  // --- Charged Multiplicity     > 0     > 0
  // --- Charged EM Fraction      < 0.99  < 0.90                                                                                                                                                        

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
