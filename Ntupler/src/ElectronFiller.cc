#include "PandaProd/Ntupler/interface/ElectronFiller.h"
#include "PandaProd/Ntupler/interface/Ntupler.h"

using namespace panda;

ElectronFiller::ElectronFiller(TString n):
    BaseFiller()
{
  // data = new TClonesArray("panda::PElectron",100);
  data = new VElectron();
  treename = n;
}

ElectronFiller::~ElectronFiller(){
  delete data;
}

void ElectronFiller::init(TTree *t) {
//  PElectron::Class()->IgnoreTObjectStreamer();
  t->Branch(treename.Data(),&data);
}

int ElectronFiller::analyze(const edm::Event& iEvent){
    // data->Clear();
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

    unsigned int iE=0;
    for (const pat::Electron& el : *el_handle) {

      if (el.pt()<minPt || fabs(el.eta())>maxEta || !(el.passConversionVeto()) ) {
        // PDebug("PandaProd::Ntupler::ElectronFiller","Rejecting at first step...");
        continue;
      }

      edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iE) ) ;

      bool veto = (*el_vetoid_handle)[ref];
      bool medium = (*el_mediumid_handle)[ref];
      bool loose = (*el_looseid_handle)[ref];
      bool tight = (*el_tightid_handle)[ref];

      if (!veto) {
        // PDebug("PandaProd::Ntupler::ElectronFiller","Rejecting at second step...");
        continue;
      }

      // compute isolation
      float chiso = el.pfIsolationVariables().sumChargedHadronPt;
      float nhiso = el.pfIsolationVariables().sumNeutralHadronEt;
      float phoiso = el.pfIsolationVariables().sumPhotonEt;
      float puiso= el.puChargedHadronIso();

      double ea = 0.; // effective area
      if ( fabs(el.eta() ) < 1.0 ) ea= 0.1752 ; 
      else if (fabs(el.eta() ) < 1.479 ) ea = 0.1862 ;
      else if (fabs(el.eta() ) < 2.0 ) ea = 0.1411 ;
      else if (fabs(el.eta() ) < 2.2 ) ea = 0.1534 ;
      else if (fabs(el.eta() ) < 2.3 ) ea = 0.1903 ;
      else if (fabs(el.eta() ) < 2.4 ) ea = 0.2243 ;
      else if (fabs(el.eta() ) < 2.5 ) ea = 0.2687 ;
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

      data->push_back(electron);

      ++iE;
    }

    std::sort(data->begin(),data->end(),SortPObjects);

    return 0;
}

