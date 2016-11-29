#include "PandaProd/Ntupler/interface/EventFiller.h"

#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"


using namespace panda;

EventFiller::EventFiller(TString n):
    BaseFiller()
{
  data = new PEvent();
  treename = n;
}

EventFiller::~EventFiller(){
  delete data;
}

void EventFiller::init(TTree *t) {
  t->Branch(treename.Data(),&data,99);
  data->tiggers->resize(trigger_paths.size(),false);
  data->metfilters->resize(metfilter_paths.size()+3,false);
}

int EventFiller::analyze(const edm::Event& iEvent){
    if (skipEvent!=0 && *skipEvent) {
      return 0;
    }

    data->runNumber     = iEvent.id().run();
    data->lumiNumber    = iEvent.luminosityBlock();
    data->eventNumber   = iEvent.id().event();
    data->isData        = iEvent.isRealData();

    if (!(data->isData)) {
      iEvent.getByToken(gen_token,gen_handle); 
      data->mcWeight = gen_handle->weight();
    }

    iEvent.getByToken(vtx_token,vtx_handle);
    data->npv = vtx_handle->size();
    pvtx = &(vtx_handle->front());

    // tiggers and met filters
    if (!minimal) {
      iEvent.getByToken(rho_token,rho_handle);
      rho_ = *rho_handle;

      unsigned int nP = trigger_paths.size();
      if (data->isData) {
        for (unsigned int jP=0; jP!=nP; ++jP) {
          data->tiggers->at(jP) = false;
        }

        iEvent.getByToken(trigger_token,trigger_handle);      
        const edm::TriggerNames &tn = iEvent.triggerNames(*trigger_handle);

        unsigned int nT = tn.size();
        for (unsigned int iT=0; iT!=nT; ++iT) {
          if (!trigger_handle->accept(iT))
            continue;
          string name = tn.triggerName(iT);
          for (unsigned int jP=0; jP!=nP; ++jP) {
            if (name.find(trigger_paths[jP]) != string::npos) {
              data->tiggers->at(jP) = trigger_handle->accept(iT);
            } // if paths match
          } // loop over saved triggers
        } // loop over all triggers
      }
      
      // met filters
      iEvent.getByToken(metfilter_token,metfilter_handle);
      nP = metfilter_paths.size();
      for (unsigned int jP=0; jP!=nP+3; ++jP) {
        // +1 for allrec and +2 for bad ch/pfmu
        data->metfilters->at(jP) = false;
      }
      const edm::TriggerNames &fn = iEvent.triggerNames(*metfilter_handle);
      unsigned int nF = fn.size();

      bool passesAll=true;
      for (unsigned int iF=0; iF!=nF; ++iF) {
        int jP = std::find(metfilter_paths.begin(),
                           metfilter_paths.end(),
                           fn.triggerName(iF)) - metfilter_paths.begin();
        if (jP<(int)nP) {
          // found the met filter
          bool passes = metfilter_handle->accept(iF);
          data->metfilters->at(jP+1) = passes;
          passesAll = passesAll && passes;
        }
      } // loop over stored filters

      // bad ch/pfmu
      iEvent.getByToken(badchcand_token,badchcand_handle);
      iEvent.getByToken(badpfmuon_token,badpfmuon_handle);
      bool passes_badchcand = *badchcand_handle; data->metfilters->at(nP+1) = passes_badchcand;
      bool passes_badpfmuon = *badpfmuon_handle; data->metfilters->at(nP+2) = passes_badpfmuon;
      passesAll = passesAll && (passes_badpfmuon && passes_badchcand);

      data->metfilters->at(0) = passesAll; // AND of all required filters
    } // isData

    return 0;
}

