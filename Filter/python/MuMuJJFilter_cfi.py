import FWCore.ParameterSet.Config as cms

MuMuJJFilter = cms.EDFilter(
                             "MuMuJJFilter",
                             jets = cms.InputTag("slimmedJets"),
                             muons = cms.InputTag("slimmedMuons"),
                             vtxs = cms.InputTag("offlineSlimmedPrimaryVertices"),
                           )
