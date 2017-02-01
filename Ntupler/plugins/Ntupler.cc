#include "PandaProd/Ntupler/interface/Ntupler.h"
#include "PandaProd/Ntupler/interface/BaseFiller.h"
#include "PandaProd/Ntupler/interface/JetSkimmer.h"
#include "PandaProd/Ntupler/interface/RecoilFilter.h"
#include "PandaProd/Ntupler/interface/EventFiller.h"
#include "PandaProd/Ntupler/interface/GenInfoFiller.h"
#include "PandaProd/Ntupler/interface/METFiller.h"
#include "PandaProd/Ntupler/interface/PFCandFiller.h"
#include "PandaProd/Ntupler/interface/MuonFiller.h"
#include "PandaProd/Ntupler/interface/ElectronFiller.h"
#include "PandaProd/Ntupler/interface/TauFiller.h"
#include "PandaProd/Ntupler/interface/PhotonFiller.h"
#include "PandaProd/Ntupler/interface/JetFiller.h"
#include "PandaProd/Ntupler/interface/FatJetFiller.h"
#include "PandaProd/Ntupler/interface/GenParticleFiller.h"
#include "PandaProd/Ntupler/interface/GenJetFiller.h"

using namespace panda;

Ntupler::Ntupler(const edm::ParameterSet& iConfig) 

{

		// INFO FILLER --------------------------------------------
		info = new InfoFiller("info");
		info->events_token	 = consumes<std::vector<long>,edm::InLumi>( edm::InputTag("InfoProducer","vecEvents") ) ;
		info->weights_token	= consumes<std::vector<float>,edm::InLumi>( edm::InputTag("InfoProducer","vecMcWeights") ) ;
		obj.push_back(info);

		skipEvent = new bool(false);
		reduceEvent = new bool(false);

		// SKIMS --------------------------------------------
		if (iConfig.getParameter<bool>("doJetSkim")) {
			JetSkimmer *skim				 = new JetSkimmer("skimmer");
			if (iConfig.getParameter<bool>("doCHSAK8"))
				skim->chsAK8_token		= consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsAK8"));
			if (iConfig.getParameter<bool>("doPuppiAK8"))
				skim->puppiAK8_token	= consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiAK8"));
			if (iConfig.getParameter<bool>("doCHSCA15"))
				skim->chsCA15_token	 = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsCA15"));
			if (iConfig.getParameter<bool>("doPuppiCA15"))
				skim->puppiCA15_token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiCA15"));
			skim->skipEvent			 = skipEvent;
			skim->minPt					 = 200;
			skim->minMass				 = 0;
			skim->maxEta					= 2.5;
			obj.push_back(skim);
		}


		// EVENT REDUCERS-------------------------------------------
		if (iConfig.getParameter<bool>("doRecoilFilter")) {
			RecoilFilter *filter			= new RecoilFilter("filter");
			filter->met_token				 = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("pfmet"));
			filter->puppimet_token		= consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("puppimet"));
			filter->mu_token					= consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
			filter->el_token					= consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
			filter->ph_token					= consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
			filter->minU							= 175;
			filter->reduceEvent			 = reduceEvent;
			obj.push_back(filter);
		}

		// EVENT FILLER --------------------------------------------
		event									 = new EventFiller("event");
		event->gen_token				= consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
		event->vtx_token				= mayConsume<reco::VertexCollection>(edm::InputTag("offlineSlimmedPrimaryVertices"));
		event->rho_token				= consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
		event->skipEvent				= skipEvent;
		event->trigger_paths		= iConfig.getParameter<std::vector<std::string>>("triggerPaths");
		event->trigger_token		= consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger"));
		event->metfilter_paths	= iConfig.getParameter<std::vector<std::string>>("metfilterPaths");
		event->metfilter_token	= consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("metfilter"));
		event->badchcand_token	= consumes<bool>(iConfig.getParameter<edm::InputTag>("chcandfilter"));
		event->badpfmuon_token	= consumes<bool>(iConfig.getParameter<edm::InputTag>("pfmuonfilter"));
		obj.push_back(event);


		// MET FILLERS -----------------------------------------------
		METFiller *pfmet					 = new METFiller("pfmet");
		pfmet->skipEvent					 = skipEvent;
		pfmet->rerun							 = false;
		pfmet->met_token					 = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("pfmet"));
		pfmet->pat_token					 = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("chsPFCands")); // these are not actually CHS, so don't worry
		pfmet->which_cand					= METFiller::kPat;
		pfmet->minimal						 = false;
		obj.push_back(pfmet);

		METFiller *puppimet				 = new METFiller("puppimet");
		puppimet->skipEvent				 = skipEvent;
		puppimet->rerun						 = false;
		puppimet->met_token				 = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("puppimet"));
		puppimet->minimal					 = true;
		obj.push_back(puppimet);


		// LEPTON FILLERS --------------------------------------------
		MuonFiller *muon					 = new MuonFiller("muon");
		muon->skipEvent						= skipEvent;
		muon->evt									= event;
		muon->mu_token						 = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
		obj.push_back(muon);

		ElectronFiller *electron		= new ElectronFiller("electron");
		electron->evt							 = event;
		electron->skipEvent				 = skipEvent;
		electron->el_token					= consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
		electron->el_vetoid_token	 = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleVetoIdMap"));
		electron->el_looseid_token	= consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleLooseIdMap"));
		electron->el_mediumid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleMediumIdMap"));
		electron->el_tightid_token	= consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleTightIdMap"));
		electron->el_hltid_token		= consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleHLTIdMap"));
		electron->effArea.reset(
			new EffectiveAreas(
				edm::FileInPath(iConfig.getParameter<std::string>("eleEA")).fullPath()
				)
			);
		obj.push_back(electron);

		TauFiller *tau						= new TauFiller("tau");
		tau->skipEvent						= skipEvent;
		tau->tau_token						= consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"));
		obj.push_back(tau);


		// PHOTON FILLER --------------------------------------------
		PhotonFiller *photon				= new PhotonFiller("photon");
		photon->skipEvent					 = skipEvent;
		photon->pho_token					 = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
		photon->pho_looseid_token	 = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoLooseIdMap"));
		photon->pho_mediumid_token	= consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoMediumIdMap"));
		photon->pho_tightid_token	 = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoTightIdMap"));
		photon->iso_ch_token				= consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoChargedIsolation"));
		photon->iso_nh_token				= consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation"));
		photon->iso_pho_token			 = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoPhotonIsolation"));
		photon->iso_wch_token			 = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoWorstChargedIsolation"));
		obj.push_back(photon);


		// PFCAND FILLERS --------------------------------------------
		PFCandFiller *puppicands=0, *pfcands=0;

		if (iConfig.getParameter<bool>("savePuppiCands")) {
			puppicands = new PFCandFiller("puppicands");
			puppicands->which_cand	 = PFCandFiller::kRecoPF;
			puppicands->recopf_token = consumes<reco::PFCandidateCollection>(iConfig.getParameter<edm::InputTag>("puppiPFCands"));
			puppicands->skipEvent		= skipEvent;
			obj.push_back(puppicands);
		}
	
		if (iConfig.getParameter<bool>("saveCHSCands")) {
			pfcands = new PFCandFiller("pfcands");
			pfcands->pat_token		= consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("chsPFCands"));
			pfcands->which_cand	 = PFCandFiller::kPat;
			pfcands->skipEvent		= skipEvent;
			obj.push_back(pfcands);
		}


		// JET FILLERS --------------------------------------------
		if (iConfig.getParameter<bool>("doCHSAK4")) {
			JetFiller *chsAK4		 = new JetFiller("chsAK4");
			chsAK4->rho_token		 = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			chsAK4->jet_token		 = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsAK4"));
			chsAK4->applyJEC			= false;
			chsAK4->minPt				 = 15;
			chsAK4->skipEvent		 = skipEvent;
			obj.push_back(chsAK4);
		}

		if (iConfig.getParameter<bool>("doPuppiAK4")) {
			JetFiller *puppiAK4		 = new JetFiller("puppiAK4");
			puppiAK4->rho_token		 = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			puppiAK4->jet_token		 = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiAK4"));
			puppiAK4->applyJEC			= false;
			puppiAK4->minPt				 = 15;
			puppiAK4->skipEvent		 = skipEvent;
			obj.push_back(puppiAK4);
		}

		if (iConfig.getParameter<bool>("doCHSAK8")) {
			FatJetFiller *chsAK8	= new FatJetFiller("chsAK8");
			chsAK8->rho_token     = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			chsAK8->jet_token     = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsAK8"));
			chsAK8->subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDropchsAK8","SubJets"));
			chsAK8->btags_token	  = mayConsume<reco::JetTagCollection>(edm::InputTag("chsAK8PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
			chsAK8->doubleb_token = consumes<reco::BoostedDoubleSVTagInfoCollection> (iConfig.getUntrackedParameter<std::string>("chsAK8BoostedDoubleSVTagInfos","chsAK8PFBoostedDoubleSVTagInfos"));
			chsAK8->qgl_token		  = mayConsume<edm::ValueMap<float>>(edm::InputTag("chsAK8SubQGTag","qgLikelihood") ) ;
			chsAK8->jetRadius		  = 0.8;
			chsAK8->skipEvent		  = skipEvent;
			chsAK8->reduceEvent	  = reduceEvent;
			chsAK8->pfcands			  = pfcands;
			chsAK8->minimal			  = true;
			obj.push_back(chsAK8);
		}

		if (iConfig.getParameter<bool>("doPuppiAK8")) {
			FatJetFiller *puppiAK8  = new FatJetFiller("puppiAK8");
			puppiAK8->rho_token     = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			puppiAK8->jet_token     = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiAK8"));
			puppiAK8->subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDroppuppiAK8","SubJets"));
			puppiAK8->btags_token   = mayConsume<reco::JetTagCollection>(edm::InputTag("puppiAK8PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
      puppiAK8->doubleb_token = consumes<reco::BoostedDoubleSVTagInfoCollection> (iConfig.getUntrackedParameter<std::string>("puppiAK8BoostedDoubleSVTagInfos","puppiAK8PFBoostedDoubleSVTagInfos"));
			puppiAK8->qgl_token    = mayConsume<edm::ValueMap<float>>(edm::InputTag("puppiAK8SubQGTag","qgLikelihood") ) ;
			puppiAK8->jetRadius    = 0.8;
			puppiAK8->skipEvent    = skipEvent;
			puppiAK8->reduceEvent  = reduceEvent;
			puppiAK8->pfcands      = puppicands;
			puppiAK8->minimal      = true;
			obj.push_back(puppiAK8);
		}

		if (iConfig.getParameter<bool>("doCHSCA15")) {
			FatJetFiller *chsCA15	 = new FatJetFiller("chsCA15");
			chsCA15->rho_token     = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			chsCA15->jet_token     = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsCA15"));
			chsCA15->subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDropchsCA15","SubJets"));
			chsCA15->btags_token   = mayConsume<reco::JetTagCollection>(edm::InputTag("chsCA15PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
      chsCA15->doubleb_token = consumes<reco::BoostedDoubleSVTagInfoCollection> (iConfig.getUntrackedParameter<std::string>("chsCA15BoostedDoubleSVTagInfos","chsCA15PFBoostedDoubleSVTagInfos"));
			chsCA15->qgl_token     = mayConsume<edm::ValueMap<float>>(edm::InputTag("chsCA15SubQGTag","qgLikelihood") ) ;
			chsCA15->jetRadius     = 1.5;
			chsCA15->skipEvent     = skipEvent;
			chsCA15->reduceEvent   = reduceEvent;
			chsCA15->pfcands       = pfcands;
			chsCA15->minimal       = false;
			obj.push_back(chsCA15);
		}

		if (iConfig.getParameter<bool>("doPuppiCA15")) {
			FatJetFiller *puppiCA15	 = new FatJetFiller("puppiCA15");
			puppiCA15->rho_token     = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
			puppiCA15->jet_token     = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiCA15"));
			puppiCA15->subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDroppuppiCA15","SubJets"));
			puppiCA15->btags_token   = mayConsume<reco::JetTagCollection>(edm::InputTag("puppiCA15PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
      puppiCA15->doubleb_token = consumes<reco::BoostedDoubleSVTagInfoCollection> (iConfig.getUntrackedParameter<std::string>("puppiCA15BoostedDoubleSVTagInfos","puppiCA15PFBoostedDoubleSVTagInfos"));
			puppiCA15->qgl_token     = mayConsume<edm::ValueMap<float>>(edm::InputTag("puppiCA15SubQGTag","qgLikelihood") ) ;
			puppiCA15->jetRadius     = 1.5;
			puppiCA15->minPt         = 100;
			puppiCA15->skipEvent     = skipEvent;
			puppiCA15->reduceEvent   = reduceEvent;
			puppiCA15->pfcands       = puppicands;
			puppiCA15->minimal       = false;
			obj.push_back(puppiCA15);
		}


		// GEN FILLER -------------------------------------------
		if (! iConfig.getParameter<bool>("isData") ) {
			GenParticleFiller *gen	 = new GenParticleFiller("gen");
			gen->packed_token				= consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
			gen->pruned_token				= consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
			gen->skipEvent					 = skipEvent;
			obj.push_back(gen);

			GenJetFiller *genjet				= new GenJetFiller("genjet");
			genjet->genjet_token				= mayConsume<reco::GenJetCollection>(edm::InputTag("ak4GenJetsYesNu"));
			genjet->skipEvent					 = skipEvent;
			obj.push_back(genjet);
			
			if (iConfig.getParameter<bool>("doPuppiCA15")) {
				GenJetFiller *genCA15			 = new GenJetFiller("genCA15");
				genCA15->genjet_token			 = mayConsume<reco::GenJetCollection>(edm::InputTag("genJetsNoNuCA15"));
				genCA15->skipEvent					= skipEvent;
				genCA15->minPt							= 150;
				genCA15->maxEta						 = 3;
				obj.push_back(genCA15);
			}
			
			GenInfoFiller *geninfo	= new GenInfoFiller("geninfo");
			geninfo->lhe_token			= mayConsume<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhe"));
			geninfo->skipEvent			= skipEvent;
			geninfo->nsyst					= iConfig.getParameter<int>("nSystWeight");
			obj.push_back(geninfo);
		}
}


Ntupler::~Ntupler()
{
	delete skipEvent;
	delete reduceEvent;
}


void Ntupler::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
		using namespace edm;

		(*skipEvent) = false; // reset for every event

		for(auto o : obj) {
				if (o->analyze(iEvent, iSetup) ) return; 
		}


		if (!(*skipEvent))
			tree_->Fill();

}


void Ntupler::beginJob()
{
		tree_ = fileService_->make<TTree>("events", "events");
		for (auto o : obj) {
				o->init(tree_);
		}

		all_ = fileService_->make<TTree>("all","all");
		info->init(all_);
		info->hDTotalMCWeight = fileService_->make<TH1D>("hDTotalMCWeight","hDTotalMCWeight",1,0,1); 
		info->hDTotalEvents = fileService_->make<TH1D>("hDTotalEvents","hDTotalEvents",1,0,1); 

		TString triggerTable("");
		std::vector<std::string> trigger_paths = event->trigger_paths;
		for (unsigned int iT=0; iT!=trigger_paths.size(); ++iT) {
			triggerTable += TString::Format("\n%i:%s",int(iT),trigger_paths.at(iT).c_str());
		}
		fileService_->make<TNamed>("triggerTable",triggerTable.Data());

		TString metFilterTable("\n0:AllFilters");
		std::vector<std::string> metfilter_paths = event->metfilter_paths;
		for (unsigned int iF=0; iF!=metfilter_paths.size(); ++iF) {
			metFilterTable += TString::Format("\n%i:%s",int(iF+1),metfilter_paths.at(iF).c_str());
		}
		metFilterTable += TString::Format("\n%i:BadChargedCandidateFilter",int(metfilter_paths.size()+1));
		metFilterTable += TString::Format("\n%i:BadPFMuonFilter",int(metfilter_paths.size()+2));
		fileService_->make<TNamed>("metFilterTable",metFilterTable.Data());
} 


void Ntupler::endJob() 
{
	for (auto o : obj) {
		string desc = o->description();
		if (desc.size()>0) {
			string title = o->name() + "Description"; // TODO - need a more descriptive name
			fileService_->make<TNamed>(title.c_str(),desc.c_str());
		}
		o->endJob();
	}
}


void Ntupler::beginRun(edm::Run const&iRun, edm::EventSetup const&)
{
}



void Ntupler::endRun(edm::Run const&iRun, edm::EventSetup const&iSetup)
{ 
}



void Ntupler::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}


void Ntupler::endLuminosityBlock(edm::LuminosityBlock const&iLumi, edm::EventSetup const&)
{
	info->analyzeLumi(iLumi,all_);
}


void Ntupler::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
		//The following says we do not know what parameters are allowed so do no validation
		// Please change this to state exactly what you do use, even if it is no parameters
		edm::ParameterSetDescription desc;
		desc.setUnknown();
		descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Ntupler);
