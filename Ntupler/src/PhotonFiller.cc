#include "PandaProd/Ntupler/interface/PhotonFiller.h"

using namespace panda;

PhotonFiller::PhotonFiller(TString n):
		BaseFiller()
{
	// data = new TClonesArray("panda::PPhoton",100);
	data = new VPhoton();
	treename = n;
}

PhotonFiller::~PhotonFiller(){
	delete data;
}

void PhotonFiller::init(TTree *t) {
//	PPhoton::Class()->IgnoreTObjectStreamer();
	t->Branch(treename.Data(),&data);
}

int PhotonFiller::analyze(const edm::Event& iEvent){
		// data->Clear();
		for (auto d : *data)
			delete d;
		data->clear(); 

		if (skipEvent!=0 && *skipEvent) {
			return 0;
		}

		iEvent.getByToken(pho_token, pho_handle);
		iEvent.getByToken(pho_looseid_token,pho_looseid_handle);
		iEvent.getByToken(pho_mediumid_token,pho_mediumid_handle);
		iEvent.getByToken(pho_tightid_token,pho_tightid_handle);
		iEvent.getByToken(iso_ch_token,iso_ch_handle);
		iEvent.getByToken(iso_nh_token,iso_nh_handle);
		iEvent.getByToken(iso_pho_token,iso_pho_handle);

		unsigned int iA=-1;
		for (const pat::Photon& pho : *pho_handle) {

			++iA;

			if (pho.pt()<minPt || fabs(pho.eta())>maxEta 
					|| pho.chargedHadronIso()/pho.pt()>0.3)
				continue;

			edm::RefToBase<pat::Photon> ref ( edm::Ref< pat::PhotonCollection >(pho_handle, iA) ) ;

			float chiso = (*iso_ch_handle)[ref];
			float nhiso = (*iso_nh_handle)[ref];
			float phoiso = (*iso_pho_handle)[ref];
			float iso = chiso + nhiso + phoiso;

			bool medium = (*pho_mediumid_handle)[ref];
			bool loose = (*pho_looseid_handle)[ref];
			bool tight = (*pho_tightid_handle)[ref];

			if (!loose)
				continue;

			// fill
			PPhoton *photon = new PPhoton();

			photon->pt = pho.pt();
			photon->eta = pho.eta();
			photon->phi = pho.phi();
			photon->m = pho.mass();
			photon->iso = iso;

			photon->id = 0;
			photon->id |= (unsigned(loose)*PPhoton::kLoose);
			photon->id |= (unsigned(medium)*PPhoton::kMedium);
			photon->id |= (unsigned(tight)*PPhoton::kTight);
			photon->id |= (unsigned(pho.passElectronVeto())*PPhoton::kEleVeto);


			data->push_back(photon);

		}

		std::sort(data->begin(),data->end(),SortPObjects);

		return 0;
}

