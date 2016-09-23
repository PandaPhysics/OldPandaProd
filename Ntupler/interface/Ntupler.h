#ifndef PANDAPROD_H
#define PANDAPROD_H

#include "Includes.h"
#include "BaseFiller.h"

class Ntupler : public edm::EDAnalyzer {
    public:
        explicit Ntupler(const edm::ParameterSet&);
        ~Ntupler();
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
        virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

        TTree *tree_;

        edm::Service<TFileService> fileService_;

        std::vector<panda::BaseFiller*> obj;
};



#endif
