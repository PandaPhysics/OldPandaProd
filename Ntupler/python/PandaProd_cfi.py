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

    # offline skimming
    doJetSkim = cms.bool(False),
    doRecoilFilter = cms.bool(True),

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
    puppiAK4 = cms.InputTag("slimmedJetsPuppi"),
    #puppiAK4 = cms.InputTag("patJetsPFAK4Puppi"),
    chsAK8 = cms.InputTag("packedPatJetsPFchsAK8"),
    puppiAK8 = cms.InputTag("packedPatJetsPFpuppiAK8"),
    chsCA15 = cms.InputTag("packedPatJetsPFchsCA15"),
    puppiCA15 = cms.InputTag("packedPatJetsPFpuppiCA15"),

    pfmet = cms.InputTag("slimmedMETs"),
    puppimet = cms.InputTag("slimmedMETsPuppi"),

    puppiPFCands = cms.InputTag("puppi"),
    chsPFCands = cms.InputTag('packedPFCandidates'),

    # egm stuffs
    eleEA = cms.string("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
    eleVetoIdMap   = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-veto"),
    eleLooseIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-loose"),
    eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-medium"),
    eleTightIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-tight"),
    eleHLTIdMap    = cms.InputTag("egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1"),
    eleMvaMap      = cms.InputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values"),

    phoLooseIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-loose"),
    phoMediumIdMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-medium"),
    phoTightIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-tight"),
    phoChargedIsolation = cms.InputTag("photonIDValueMapProducer:phoChargedIsolation"),
    phoNeutralHadronIsolation = cms.InputTag("photonIDValueMapProducer:phoNeutralHadronIsolation"),
    phoPhotonIsolation = cms.InputTag("photonIDValueMapProducer:phoPhotonIsolation"),
    phoWorstChargedIsolation = cms.InputTag("photonIDValueMapProducer:phoWorstChargedIsolation"),

    # gen
    generator = cms.InputTag("generator"),
    lhe = cms.InputTag("externalLHEProducer"),
    genjets = cms.InputTag("slimmedGenJets"),
    prunedgen = cms.InputTag("prunedGenParticles"),
    packedgen = cms.InputTag("packedGenParticles"),

    #ak4
    minAK4Pt  = cms.double (15.),
    maxAK4Eta = cms.double (4.7),

    #ak8
    minAK8Pt  = cms.double (100.),
    maxAK8Eta = cms.double (2.5),

    #ca15
    minCA15Pt  = cms.double (100.),
    maxCA15Eta = cms.double (2.5),

    #gen
    minGenParticlePt = cms.double(5.),
    minGenJetPt = cms.double(20.),

    # triggers
    trigger = cms.InputTag("TriggerResults","","HLT"),
    triggerPaths = cms.vstring([
                                 'HLT_PFMET170_NoiseCleaned',                   # MET
                                 'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight',
                                 'HLT_PFMETNoMu110_NoiseCleaned_PFMHTNoMu110_IDTight',
                                 'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight',
                                 'HLT_PFMET170_HBHECleaned',                            
                                 'HLT_PFMET170_JetIdCleaned',                           
                                 'HLT_PFMET170_NotCleaned',                             
                                 'HLT_PFMET170_HBHE_BeamHaloCleaned',                   
                                 'HLT_PFMETNoMu90_PFMHTNoMu90_IDTight',
                                 'HLT_PFMETNoMu100_PFMHTNoMu100_IDTight',
                                 'HLT_PFMETNoMu110_PFMHTNoMu110_IDTight',
                                 'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight',

                                 'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140_v', # VBF

                                 'HLT_IsoMu20_v',                               # MUON
                                 'HLT_IsoTkMu20_v',
                                 'HLT_IsoMu22_v',
                                 'HLT_IsoTkMu22_v',
                                 'HLT_IsoMu24',
                                 'HLT_IsoTkMu24_v',
                                 'HLT_IsoMu27_v',
                                 'HLT_IsoTkMu27_v',
                                 'HLT_Mu45_eta2p1_v',                                     
                                 'HLT_Mu50_v',                                            
                                 'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',                    
                                 'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',                  
                                 'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',                 
                                 'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',               

                                 'HLT_Ele25_eta2p1_WPTight_Gsf_v',              # ELECTRON
                                 'HLT_Ele27_eta2p1_WPLoose_Gsf_v',
                                 'HLT_Ele27_WPTight_Gsf_v',
                                 'HLT_Ele30_WPTight_Gsf_v',          
                                 'HLT_Ele32_eta2p1_WPTight_Gsf_v',   
                                 'HLT_Ele35_WPLoose_Gsf_v',
                                 'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',  
                                 'HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v',      
                                 'HLT_Ele105_CaloIdVT_GsfTrkIdT_v', 

                                 'HLT_ECALHT800_v',                             # ELECTRON+PHOTON

                                 'HLT_Photon175_v',                             # PHOTON
                                 'HLT_Photon165_HE10_v',
                                 'HLT_Photon36_R9Id90_HE10_IsoM_v', 
                                 'HLT_Photon50_R9Id90_HE10_IsoM_v', 
                                 'HLT_Photon75_R9Id90_HE10_IsoM_v', 
                                 'HLT_Photon90_R9Id90_HE10_IsoM_v', 
                                 'HLT_Photon120_R9Id90_HE10_IsoM_v',
                                 'HLT_Photon165_R9Id90_HE10_IsoM_v',

                            ]),

    metfilter = cms.InputTag('TriggerResults','','RECO'),
    metfilterPaths = cms.vstring([
                                  'Flag_HBHENoiseFilter', 
                                  'Flag_HBHENoiseIsoFilter', 
                                  'Flag_EcalDeadCellTriggerPrimitiveFilter', 
                                  'Flag_goodVertices', 
                                  'Flag_eeBadScFilter',
                                  'Flag_globalTightHalo2016Filter'
                                ]),
    chcandfilter = cms.InputTag('BadChargedCandidateFilter'),
    pfmuonfilter = cms.InputTag('BadPFMuonFilter'),
                      
)
#------------------------------------------------------


