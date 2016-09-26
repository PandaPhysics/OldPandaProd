#include "PandaProd/Ntupler/interface/MuonFiller.h"
#include "PandaProd/Ntupler/interface/Ntupler.h"

using namespace panda;

MuonFiller::MuonFiller(TString n):
    BaseFiller()
{
  // data = new TClonesArray("panda::PMuon",100);
  data = new VMuon();
  treename = n;
}

MuonFiller::~MuonFiller(){
  delete data;
}

void MuonFiller::init(TTree *t) {
//  PMuon::Class()->IgnoreTObjectStreamer();
  t->Branch(treename.Data(),&data);
}

int MuonFiller::analyze(const edm::Event& iEvent){
    // data->Clear();
    for (auto d : *data)
      delete d;
    data->clear(); 

    if (skipEvent!=0 && *skipEvent) {
      return 0;
    }

    iEvent.getByToken(mu_token, mu_handle);

    for (const pat::Muon& mu : *mu_handle) {
      if (mu.pt()<minPt || fabs(mu.eta())>maxEta || !(mu.isLooseMuon()) ) 
        continue;

      float chiso  = mu.pfIsolationR04().sumChargedHadronPt;
      float nhiso   = mu.pfIsolationR04().sumNeutralHadronEt;
      float phoiso = mu.pfIsolationR04().sumPhotonEt;
      float puiso = mu.pfIsolationR04().sumPUPt;
      float iso = chiso + TMath::Max( nhiso + phoiso - .5*puiso, 0. ) ;

      PMuon *muon = new PMuon();

      muon->pt = mu.pt();
      muon->eta = mu.eta();
      muon->phi = mu.phi();
      muon->m = mu.mass();
      muon->q = mu.charge();
      muon->iso = iso;
      muon->chiso = chiso;
      muon->nhiso = nhiso;
      muon->phoiso = phoiso;
      muon->puiso = puiso;

      muon->id = 0;
      muon->id |= (unsigned(mu.isLooseMuon())*PMuon::kLoose);
      muon->id |= (unsigned(mu.isMediumMuon())*PMuon::kMedium);
      muon->id |= (unsigned(mu.isTightMuon(* evt->pv()))*PMuon::kTight);

      data->push_back(muon);

    }

    std::sort(data->begin(),data->end(),SortPObjects);

    return 0;
}

