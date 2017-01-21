// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "PandaUtilities/Common/interface/Common.h"
#include "PandaUtilities/Common/interface/DataTools.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"

//
// class declaration
//

class TriggerFilter : public edm::EDFilter {
public:
		explicit TriggerFilter(const edm::ParameterSet&);
		~TriggerFilter();
		
		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
		virtual bool filter(edm::Event&, const edm::EventSetup&) override;

		std::vector<std::string> trigger_paths;
		edm::Handle< edm::TriggerResults	> trigger_handle;
		edm::EDGetTokenT< edm::TriggerResults > trigger_token;

};

TriggerFilter::TriggerFilter(const edm::ParameterSet& iConfig):
		trigger_paths( iConfig.getParameter<std::vector<std::string>>("triggerPaths") ),
		trigger_token( consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger")) )
{
}


TriggerFilter::~TriggerFilter()
{
}

bool
TriggerFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
		using namespace edm;

		if (! iEvent.isRealData() )
			return true; // do not filter on MC

		iEvent.getByToken(trigger_token,trigger_handle);
		const edm::TriggerNames &tn = iEvent.triggerNames(*trigger_handle);

		unsigned nP = trigger_paths.size();
		unsigned nT = tn.size();

		bool passesAny=false;
		for (unsigned iT=0; iT!=nT; ++iT) {
			if (passesAny)
				break;
			if (!trigger_handle->accept(iT))
				continue;
			std::string name = tn.triggerName(iT);
			for (unsigned jP=0; jP!=nP; ++jP) {
				if (matchTriggerName(trigger_paths[jP],name)) {
					passesAny=true;
					break;
				} // if paths match
			} // loop over desired paths
		} // loop over all triggers

		return passesAny;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module	------------
void
TriggerFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
		//The following says we do not know what parameters are allowed so do no validation
		// Please change this to state exactly what you do use, even if it is no parameters
		edm::ParameterSetDescription desc;
		desc.setUnknown();
		descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerFilter);
