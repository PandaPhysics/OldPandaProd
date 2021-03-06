#ifndef Jet_H
#define Jet_H

#include "BaseFiller.h"
#include "PandaProd/Objects/interface/PJet.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include <map>
#include <string>

namespace panda {
class JetFiller : virtual public BaseFiller
{
		public:
				JetFiller(TString n);
				~JetFiller();
				static bool JetId(const pat::Jet &, string id);
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "JetFiller";};
				void init(TTree *t);

				edm::Handle<pat::JetCollection> jet_handle; 
				edm::EDGetTokenT<pat::JetCollection> jet_token;

				edm::Handle<double> rho_handle;
				edm::EDGetTokenT<double> rho_token;

				bool applyJEC=false; // should deprecate this soon?

				float minPt=15, maxEta=4.7;

		private:
				// TClonesArray *data;
				panda::VJet *data;
				TString treename;

				FactorizedJetCorrector *mMCJetCorrector;	 // needed for puppi fat jets
				FactorizedJetCorrector *mDataJetCorrector; 

};
}


#endif
