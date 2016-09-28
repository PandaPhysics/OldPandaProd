import FWCore.ParameterSet.Config as cms

MonoXFilter = cms.EDFilter("MonoXFilter",
                             met = cms.InputTag("slimmedMETs"),
                             puppimet = cms.InputTag("type1PuppiMET"),
                             muons = cms.InputTag("slimmedMuons"),
                             electrons = cms.InputTag("slimmedElectrons"),
                             photons = cms.InputTag("slimmedPhotons"),
                             saveWlv = cms.bool(True),
                             saveZll = cms.bool(True),
                             savePho = cms.bool(True),
                             minU = cms.double(175)
                           )
