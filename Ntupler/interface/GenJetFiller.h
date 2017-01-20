#ifndef GenJet_H
#define GenJet_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PGenJet.h"

namespace panda {
class GenJetFiller : virtual public BaseFiller
{
    public:
        GenJetFiller(TString n);
        ~GenJetFiller();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "GenJetFiller";};
        void init(TTree *t);

        edm::Handle<reco::GenJetCollection> genjet_handle;
        edm::EDGetTokenT<reco::GenJetCollection> genjet_token;

        float minPt=15;
				float maxEta=10;

    private:
        panda::VGenJet *data;
        TString treename;
};
}


#endif
