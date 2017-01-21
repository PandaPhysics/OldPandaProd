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
				float pt = gen.pt();
				float eta = gen.eta();

				if (pt<minPt)
					continue;

				if (fabs(eta)>maxEta)
					continue;

				PGenJet *particle = new PGenJet();

				particle->pt = pt;
				particle->eta = eta;
				particle->phi = gen.phi();
				particle->m = gen.mass();
				particle->pdgid = pdg;

				data->push_back(particle);
		}

		return 0;
}

