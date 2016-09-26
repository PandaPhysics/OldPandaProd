#ifndef Tau_H
#define Tau_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PTau.h"

#include <map>
#include <string>

namespace panda {
class TauFiller : virtual public BaseFiller
{
    public:
        TauFiller(TString n);
        ~TauFiller();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "TauFiller";};
        void init(TTree *t);

        edm::EDGetTokenT<pat::TauCollection> tau_token;
        edm::Handle<pat::TauCollection> tau_handle;

        float minPt=10, maxEta=2.5, maxIso=5;

    private:
        // TClonesArray *data;
        panda::VTau *data;
        TString treename;

};
}


#endif
