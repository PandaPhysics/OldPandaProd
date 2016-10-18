#ifndef Photon_H
#define Photon_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PPhoton.h"

#include <map>
#include <string>

namespace panda {
class PhotonFiller : virtual public BaseFiller
{
    public:
        PhotonFiller(TString n);
        ~PhotonFiller();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "PhotonFiller";};
        void init(TTree *t);

        edm::EDGetTokenT<pat::PhotonCollection> pho_token;
        edm::Handle<pat::PhotonCollection> pho_handle;

        edm::EDGetTokenT<edm::ValueMap<bool> >  pho_looseid_token;
        edm::Handle<edm::ValueMap<bool> > pho_looseid_handle;
        edm::EDGetTokenT<edm::ValueMap<bool> >  pho_mediumid_token;
        edm::Handle<edm::ValueMap<bool> > pho_mediumid_handle;
        edm::EDGetTokenT<edm::ValueMap<bool> >  pho_tightid_token;
        edm::Handle<edm::ValueMap<bool> > pho_tightid_handle;

        float minPt=15, maxEta=2.5;

    private:
        // TClonesArray *data;
        panda::VPhoton *data;
        TString treename;

};
}


#endif
