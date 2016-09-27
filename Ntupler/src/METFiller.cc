#include "PandaProd/Ntupler/interface/METFiller.h"
#include "PandaProd/Ntupler/interface/Ntupler.h"

using namespace panda;

METFiller::METFiller(TString n):
    BaseFiller()
{
  data = new PMET();
  treename = n;
}

METFiller::~METFiller(){
  delete data;
}

void METFiller::init(TTree *t) {
  t->Branch(treename.Data(),&data);
}

void METFiller::fillMETs(std::vector<const reco::Candidate*> pfs) {
}


int METFiller::analyze(const edm::Event& iEvent){
    if (skipEvent!=0 && *skipEvent) {
      return 0;
    }

    if (!rerun) {
      iEvent.getByToken(met_token,met_handle);
      const pat::MET &met = met_handle->front();
      data->pt = met.pt();
      data->phi = met.phi();
      data->sumETRaw = met.uncorSumEt();
      data->raw_pt = met.uncorPt();
      data->raw_phi = met.uncorPhi(); 
    } else {
      iEvent.getByToken(remet_token,remet_handle);
      iEvent.getByToken(remetuncorr_token,remetuncorr_handle);
      auto &met = remet_handle->front();
      auto &metuncorr = remetuncorr_handle->front();
      data->pt = met.pt();
      data->phi = met.phi();
      data->sumETRaw = metuncorr.sumEt();
      data->raw_pt = metuncorr.pt();
      data->raw_phi = metuncorr.phi();
    }

    if (minimal)
      return 0;

    // TODO: write function to produce different METs
    // if (which_cand==kPat) {
    //   iEvent.getByToken(pat_token,pat_handle);
    //   assert(pat_handle.isValid());

    //   const pat::PackedCandidateCollection *pfCol = pat_handle.product();

    //   for(pat::PackedCandidateCollection::const_iterator iPF = pfCol->begin(); 
    //         iPF!=pfCol->end(); ++iPF) {
    //     fillCand((const reco::Candidate*)&(*iPF));      
    //   }
    // } else if (which_cand==kRecoPF) {
    //   iEvent.getByToken(recopf_token,recopf_handle);
    //   assert(recopf_handle.isValid());

    //   const reco::METidateCollection *pfCol = recopf_handle.product();

    //   for (reco::METidateCollection::const_iterator iPF=pfCol->begin();
    //         iPF!=pfCol->end(); ++iPF) {
    //     fillCand((const reco::Candidate*)&(*iPF));      
    //   }
    // } else if (which_cand==kReco) {
    //   return 0;
    //   iEvent.getByToken(reco_token,reco_handle);
    //   assert(reco_handle.isValid());

    //   const reco::CandidateCollection *pfCol = reco_handle.product();

    //   for (reco::CandidateCollection::const_iterator iPF=pfCol->begin();
    //         iPF!=pfCol->end(); ++iPF) {
    //     fillCand((const reco::Candidate*)&(*iPF));      
    //   }
    // }


    return 0;
}

