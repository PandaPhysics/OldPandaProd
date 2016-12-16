#ifndef GenInfo_H
#define GenInfo_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PGenInfo.h"

namespace panda {
class GenInfoFiller : virtual public BaseFiller
{
    public:
        GenInfoFiller(TString n);
        ~GenInfoFiller();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "GenInfoFiller";};
        void init(TTree *t);

	edm::EDGetTokenT<LHEEventProduct> lhe_token;
	edm::Handle<LHEEventProduct> lhe_handle;

    private:
	panda::PGenInfo *data;
        TString treename;
};
}


#endif
