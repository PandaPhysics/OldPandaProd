#ifndef FatJet_H
#define FatJet_H

#include "BaseFiller.h"
#include "PFCandFiller.h"
#include "PandaProd/Objects/interface/PFatJet.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "DataFormats/BTauReco/interface/BoostedDoubleSVTagInfo.h"

// fastjet
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/GhostedAreaSpec.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/contrib/NjettinessPlugin.hh"
#include "fastjet/contrib/MeasureDefinition.hh"
#include "fastjet/contrib/EnergyCorrelator.hh"

#include "PandaProd/Utilities/interface/EnergyCorrelations.h"
#include "PandaProd/Utilities/interface/HEPTopTaggerWrapperV2.h"
#include "PandaProd/Utilities/interface/BoostedBtaggingMVACalculator.hh"
#include "PandaUtilities/Common/interface/DataTools.h"

#include <map>
#include <string>

namespace panda {
class FatJetFiller : virtual public BaseFiller
{
		public:
				FatJetFiller(TString n);
				~FatJetFiller();
				static bool JetId(const pat::Jet &, string id);
				int analyze(const edm::Event& iEvent);
				virtual inline string name(){return "FatJetFiller";};
				void init(TTree *t);
				TString get_treename() { return treename; }

				edm::Handle<pat::JetCollection> jet_handle; 
				edm::EDGetTokenT<pat::JetCollection> jet_token;

				edm::Handle<double> rho_handle;
				edm::EDGetTokenT<double> rho_token;

				edm::Handle<reco::PFJetCollection> subjets_handle;
				edm::EDGetTokenT<reco::PFJetCollection> subjets_token;

				edm::Handle<reco::JetTagCollection> btags_handle;
				edm::EDGetTokenT<reco::JetTagCollection> btags_token;

				edm::Handle<reco::BoostedDoubleSVTagInfoCollection> doubleb_handle;
				edm::EDGetTokenT<reco::BoostedDoubleSVTagInfoCollection>  doubleb_token; 

				edm::Handle<edm::ValueMap<float>> qgl_handle;
				edm::EDGetTokenT<edm::ValueMap<float>> qgl_token;

				float minPt=180, maxEta=2.5;
				float jetRadius;

				PFCandFiller *pfcands=0; // pointer to the relevant pf cand filler, used to get a map

				bool minimal = false;
				float radius=1.5;

		private:
				panda::VFatJet *data;
				TString treename;

				FactorizedJetCorrector *mMCJetCorrector;	 
				std::map<TString,FactorizedJetCorrector *> mDataJetCorrectors;	// map from era to corrector

				BoostedBtaggingMVACalculator mJetBoostedBtaggingMVACalc;

				fastjet::AreaDefinition *areaDef;
				fastjet::GhostedAreaSpec *activeArea;
				fastjet::JetDefinition *jetDefCA=0;
				fastjet::contrib::SoftDrop *softdrop=0;
				fastjet::contrib::Njettiness *tau=0;

				ECFNManager *ecfnmanager=0;
				fastjet::HEPTopTaggerV2 *htt=0;

				EraHandler *eras=0;

};

typedef std::vector<fastjet::PseudoJet> VPseudoJet;

}


#endif
