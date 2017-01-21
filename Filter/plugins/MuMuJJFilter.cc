// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"

#include "PandaUtilities/Common/interface/Common.h"

//
// class declaration
//

class MuMuJJFilter : public edm::EDFilter {
public:
		explicit MuMuJJFilter(const edm::ParameterSet&);
		~MuMuJJFilter();
		
		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
		virtual bool filter(edm::Event&, const edm::EventSetup&) override;
		bool isGoodMuon(const pat::Muon&, const reco::Vertex &vtx);
		
		edm::EDGetTokenT<pat::MuonCollection> mu_token;
		edm::Handle<pat::MuonCollection> mu_handle;

		edm::Handle<pat::JetCollection> jet_handle; 
		edm::EDGetTokenT<pat::JetCollection> jet_token;

		edm::EDGetTokenT<reco::VertexCollection> vtx_token;
		edm::Handle<reco::VertexCollection> vtx_handle;
};

MuMuJJFilter::MuMuJJFilter(const edm::ParameterSet& iConfig):
		mu_token(	consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")) ),
		jet_token(	consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets")) ),
		vtx_token(	consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vtxs")) )
{
}


MuMuJJFilter::~MuMuJJFilter()
{
}

bool MuMuJJFilter::isGoodMuon(const pat::Muon& muon,const reco::Vertex &vtx){
		if (muon.pt()<15) return false;
		if (not muon.isTightMuon(vtx)) return false;
		if (muon.pfIsolationR04().sumChargedHadronPt/muon.pt()>0.12) return false;
		return true;
}


bool
MuMuJJFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
		using namespace edm;
		iEvent.getByToken(jet_token, jet_handle);
		iEvent.getByToken(mu_token, mu_handle);
		iEvent.getByToken(vtx_token, vtx_handle);

		int nFwdJet=0;
		int nCentralJet=0;
		int nBJet=0;
		int mu1Charge=0;
		int mu2Charge=0;
		int nMuons=0;

		for (pat::JetCollection::const_iterator jet = jet_handle->begin(); jet!=jet_handle->end(); jet++){
			if (jet->pt()<30)
				continue;
			if (fabs(jet->eta())<2.4) {
				++nCentralJet;
				if (jet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")>0.8) 
					++nBJet;
			} else if (fabs(jet->eta())>2.4) {
				++nFwdJet;
			}
		}

		if (nCentralJet!=1 || nBJet!=1 || nFwdJet==0)
			return false;

		const reco::Vertex pvtx = vtx_handle->front();

		for (pat::MuonCollection::const_iterator muon = mu_handle->begin(); muon!=mu_handle->end(); muon++){
				if (not isGoodMuon(*muon,pvtx)) 
					continue;
				++nMuons;
				if (mu1Charge==0)
					mu1Charge = muon->charge();
				else if (mu2Charge==0)
					mu2Charge = muon->charge();
		}

		if (nMuons!=2 || mu1Charge+mu2Charge!=0)
			return false;

		printf("accepting event...\n");
		return true;

}

// ------------ method fills 'descriptions' with the allowed parameters for the module	------------
void
MuMuJJFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
		//The following says we do not know what parameters are allowed so do no validation
		// Please change this to state exactly what you do use, even if it is no parameters
		edm::ParameterSetDescription desc;
		desc.setUnknown();
		descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuMuJJFilter);
