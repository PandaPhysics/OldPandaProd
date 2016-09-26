import FWCore.ParameterSet.Config as cms

from subprocess import check_output
import os


#------------------------------------------------------
PandaNtupler = cms.EDAnalyzer("Ntupler",

    info = cms.string("PandaNtupler"),
    cmssw = cms.string( os.environ['CMSSW_VERSION'] ) , # no need to ship it with the grid option

    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    muons = cms.InputTag("slimmedMuons"),
    electrons = cms.InputTag("slimmedElectrons"),
    taus = cms.InputTag("slimmedTaus"),
    photons = cms.InputTag("slimmedPhotons"),

    # skimming
    doJetSkim = cms.bool(False),

    # jet toggles
    savePuppiCands = cms.bool(False),
    saveCHSCands = cms.bool(False), 
    doCHSAK4 = cms.bool(True),
    doPuppiAK4 = cms.bool(True),
    doPuppiCA15 = cms.bool(True),
    doCHSCA15 = cms.bool(False),
    doPuppiAK8 = cms.bool(False),
    doCHSAK8 = cms.bool(False),

    chsAK4 = cms.InputTag("slimmedJets"),
    puppiAK4 = cms.InputTag("patJetsPFAK4Puppi"),
    chsAK8 = cms.InputTag("packedPatJetsPFchsAK8"),
    puppiAK8 = cms.InputTag("packedPatJetsPFpuppiAK8"),
    chsCA15 = cms.InputTag("packedPatJetsPFchsCA15"),
    puppiCA15 = cms.InputTag("packedPatJetsPFpuppiCA15"),

    mets = cms.InputTag("slimmedMETs"),
    metsPuppi = cms.InputTag("type1PuppiMET"),
    metsPuppiUncorrected = cms.InputTag("pfMETPuppi"),

    puppiPFCands = cms.InputTag("puppi"),
    chsPFCands = cms.InputTag('packedPFCandidates'),
    #chsPFCands = cms.InputTag('pfCHS'),

    # egm IDs
    eleVetoIdMap   = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-%(bx)s-%(vs)s-standalone-veto"),
    eleLooseIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-%(bx)s-%(vs)s-standalone-loose"),
    eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-%(bx)s-%(vs)s-standalone-medium"),
    eleTightIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-%(bx)s-%(vs)s-standalone-tight"),
    phoLooseIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring15-%(bx)s-%(vs)s-standalone-loose"),
    phoMediumIdMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring15-%(bx)s-%(vs)s-standalone-medium"),
    phoTightIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring15-%(bx)s-%(vs)s-standalone-tight"),

    # gen
    generator = cms.InputTag("generator"),
    genjets = cms.InputTag("slimmedGenJets"),
    prunedgen = cms.InputTag("prunedGenParticles"),
    packedgen = cms.InputTag("packedGenParticles"),

    #ak4
    minAK4Pt  = cms.double (15.),
    maxAK4Eta = cms.double (4.7),

    #ak8
    minAK8Pt  = cms.double (180.),
    maxAK8Eta = cms.double (2.5),

    #ca15
    minCA15Pt  = cms.double (180.),
    maxCA15Eta = cms.double (2.5),

    #gen
    minGenParticlePt = cms.double(5.),
    minGenJetPt = cms.double(20.),

    # triggers
    trigger = cms.InputTag("TriggerResults","","HLT"),
    triggerPaths = cms.vstring([
                                 'HLT_PFMET170_NoiseCleaned_v',                 # MET
                                 'HLT_PFMET170_JetIdCleaned_v',
                                 'HLT_PFMET170_HBHECleaned_v',
                                 'HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_v',
                                 'HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_v',
                                 'HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_v',
                                 'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v',
                                 'HLT_IsoMu18_v',                               # MUON
                                 'HLT_IsoMu20_v',
                                 'HLT_IsoMu22_v',
                                 'HLT_IsoMu24',
                                 'HLT_IsoMu27_v',
                                 'HLT_IsoTkMu18_v',
                                 'HLT_IsoTkMu24_v',
                                 'HLT_Ele25_eta2p1_WPTight_Gsf_v',              # ELECTRON
                                 'HLT_Ele27_eta2p1_WPLoose_Gsf_v',
                                 'HLT_Ele27_WPTight_Gsf_v',
                                 'HLT_Ele35_WPLoose_Gsf_v',
                                 'HLT_Ele105_CaloIdVT_GsfTrkIdT_v', 
                                 'HLT_ECALHT800_v',                             # ELECTRON+PHOTON
                                 'HLT_Photon175_v',                             # PHOTON
                                 'HLT_Photon165_HE10_v',
                                 'HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40_v', 
                                 'HLT_Photon135_PFMET100_v',
                                 'HLT_Photon300_NoHE_v',
                            ]),

    metfilter = cms.InputTag('TriggerResults','','RECO'),
    metfilterPaths = cms.vstring([
                                  'Flag_HBHENoiseFilter', 
                                  'Flag_HBHENoiseIsoFilter', 
                                  'Flag_CSCTightHalo2015Filter', 
                                  'Flag_EcalDeadCellTriggerPrimitiveFilter', 
                                  'Flag_goodVertices', 
                                  'Flag_eeBadScFilter',
                                  'Flag_globalTightHalo2016Filter'
                                ]),
    chcandfilter = cms.InputTag('BadChargedCandidateFilter'),
    pfmuonfilter = cms.InputTag('BadPFMuonFilter'),
                      
)
#------------------------------------------------------


