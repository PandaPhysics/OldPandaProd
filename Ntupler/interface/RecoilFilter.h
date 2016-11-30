#ifndef RecoilFilter_H
#define RecoilFilter_H

#include "BaseFiller.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include <map>
#include <string>


namespace panda {
class RecoilFilter : virtual public BaseFiller
{
    public:
        RecoilFilter(TString n);
        ~RecoilFilter();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "RecoilFilter";};
        virtual void endJob();
        void init(TTree *t);

        edm::EDGetTokenT<pat::METCollection> met_token ;    
        edm::Handle<pat::METCollection> met_handle;

        edm::EDGetTokenT<pat::METCollection> puppimet_token;
        edm::Handle<pat::METCollection> puppimet_handle;

        edm::EDGetTokenT<pat::MuonCollection> mu_token;
        edm::Handle<pat::MuonCollection> mu_handle;

        edm::EDGetTokenT<pat::ElectronCollection> el_token;
        edm::Handle<pat::ElectronCollection> el_handle;

        edm::EDGetTokenT<pat::PhotonCollection> ph_token;
        edm::Handle<pat::PhotonCollection> ph_handle;

        float minU=150;

    private:
        bool isGoodMuon(const pat::Muon&);
        bool isGoodElectron(const pat::Electron&);
        bool isGoodPhoton(const pat::Photon&); 

        TString treename;
        float maxRecoil=0;
        int whichRecoil=-1; //!< = 2*RecoilType + METType

        int nTotal=0, nAcc=0;
};
}


#endif
