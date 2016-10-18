#include "PandaProd/Ntupler/interface/GenJetFiller.h"

using namespace panda;

GenJetFiller::GenJetFiller(TString n):
    BaseFiller()
{
  data = new VGenJet();
  treename = n;
}

GenJetFiller::~GenJetFiller(){
  delete data;
}

void GenJetFiller::init(TTree *t) {
  t->Branch(treename.Data(),&data,99);
}

int GenJetFiller::analyze(const edm::Event& iEvent){
    for (auto d : *data)
      delete d;
    data->clear(); 

    if (skipEvent!=0 && *skipEvent) {
      return 0;
    }

    if (iEvent.isRealData()) return 0;

    iEvent.getByToken(genjet_token, genjet_handle);

    for (auto &gen : *genjet_handle) {
        int pdg = gen.pdgId();

        PGenJet *particle = new PGenJet();

        particle->pt = gen.pt();
        particle->eta = gen.eta();
        particle->phi = gen.phi();
        particle->m = gen.mass();
        particle->pdgid = pdg;

        data->push_back(particle);
    }

    return 0;
}

