import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import re
import os

process = cms.Process("PandaNtupler")
cmssw_base = os.environ['CMSSW_BASE']


options = VarParsing.VarParsing ('analysis')
options.register('isData',
				False,
				VarParsing.VarParsing.multiplicity.singleton,
				VarParsing.VarParsing.varType.bool,
				"True if running on Data, False if running on MC")

options.register('isSignal',
				False,
				VarParsing.VarParsing.multiplicity.singleton,
				VarParsing.VarParsing.varType.bool,
				"True if running on MC signal samples")
options.register('infile',
				None,
				VarParsing.VarParsing.multiplicity.singleton,
				VarParsing.VarParsing.varType.string,
				'input file to run on')

options.register('isGrid', False, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set it to true if running on Grid")

options.parseArguments()
isData = options.isData


process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 5

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

if options.infile:
	fileList = ['file:'+options.infile]
	print 'Running on %s'%(options.infile)
elif isData:
	 fileList = [
			 #'file:/data/t3home000/snarayan/test/met_8020.root'
			 'file:/afs/cern.ch/work/s/snarayan/8020_singleel.root'
			 ]
else:
	 fileList = [
			 #'file:/data/t3home000/snarayan/test/tt_8011.root'
			 'file:/afs/cern.ch/work/s/snarayan/8024_tt.root'
			 #'file:/afs/cern.ch/work/s/snarayan/8024_monotop.root'
			 ]
### do not remove the line below!
###FILELIST###

process.source = cms.Source("PoolSource",
					skipEvents = cms.untracked.uint32(0),
					fileNames = cms.untracked.vstring(fileList)
				)

# ---- define the output file -------------------------------------------
process.TFileService = cms.Service("TFileService",
					closeFileFast = cms.untracked.bool(True),
					fileName = cms.string("panda.root"),
				)

##----------------GLOBAL TAG ---------------------------
# used by photon id and jets
process.load("Configuration.Geometry.GeometryIdeal_cff") 
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")

#mc https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions#Global_Tags_for_Run2_MC_Producti
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if (isData):
		# sept reprocessing
		process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v7'
else:
		process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
#if isData and not options.isGrid and False: ## dont load the lumiMaks, will be called by crab
if isData:
		import FWCore.PythonUtilities.LumiList as LumiList
		process.source.lumisToProcess = LumiList.LumiList(filename='goodlumis/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt').getVLuminosityBlockRange()
		print "Using local JSON"

### LOAD CONFIGURATION
process.load('PandaProd.Filter.infoProducerSequence_cff')
process.load('PandaProd.Filter.MonoXFilterSequence_cff')
process.load('PandaProd.Ntupler.PandaProd_cfi')
#process.load('PandaProd.Ntupler.VBF_cfi')

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

process.PandaNtupler.isData = isData
if isData and False:
	process.triggerFilter = cms.EDFilter('TriggerFilter',
																triggerPaths = process.PandaNtupler.triggerPaths,
																trigger = process.PandaNtupler.trigger
															)
	process.triggerFilterSequence = cms.Sequence( process.triggerFilter )
else:
	process.triggerFilterSequence = cms.Sequence()

if options.isSignal:
	process.PandaNtupler.nSystWeight = -1

#-----------------------JES/JER----------------------------------
from CondCore.DBCommon.CondDBSetup_cfi import *
if isData:
	jeclabel = 'Summer16_23Sep2016AllV3_DATA'
else:
	jeclabel = 'Summer16_23Sep2016V3_MC'
process.jec =	cms.ESSource("PoolDBESSource",
#										CondDBSetup,
										DBParameters = cms.PSet(
											messageLevel = cms.untracked.int32(0)
											),
										timetype = cms.string('runnumber'),
										toGet = cms.VPSet(
							cms.PSet(record	= cms.string('JetCorrectionsRecord'),
											 tag		 = cms.string('JetCorrectorParametersCollection_'+jeclabel+'_AK4PFPuppi'),
											 label	 = cms.untracked.string('AK4PFPuppi')
											 ),
							 cms.PSet(record	= cms.string('JetCorrectionsRecord'),
												tag		 = cms.string('JetCorrectorParametersCollection_'+jeclabel+'_AK8PFPuppi'),
												label	 = cms.untracked.string('AK8PFPuppi')
												),
							cms.PSet(record	= cms.string('JetCorrectionsRecord'),
											 tag		 = cms.string('JetCorrectorParametersCollection_'+jeclabel+'_AK4PFchs'),
											 label	 = cms.untracked.string('AK4PFchs')
											 ),
							cms.PSet(record	= cms.string('JetCorrectionsRecord'),
											 tag		 = cms.string('JetCorrectorParametersCollection_'+jeclabel+'_AK8PFchs'),
											 label	 = cms.untracked.string('AK8PFchs')
											 ),
							 ),

				)	
process.jec.connect = cms.string('sqlite:jec/%s.db'%jeclabel)
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource', 'jec')

if isData:
	jerlabel = 'Spring16_25nsV6_DATA'
else:
	jerlabel = 'Spring16_25nsV6_MC'
process.jer = cms.ESSource("PoolDBESSource",
#										CondDBSetup,
										DBParameters = cms.PSet(
											messageLevel = cms.untracked.int32(0)
											),
									toGet = cms.VPSet(
							cms.PSet(record	= cms.string('JetResolutionRcd'),
											 tag		 = cms.string('JR_%s_PtResolution_AK4PFchs'%jerlabel),
											 label	 = cms.untracked.string('AK4PFchs_pt'),
											),
							cms.PSet(record	= cms.string('JetResolutionRcd'),
											 tag		 = cms.string('JR_%s_PhiResolution_AK4PFchs'%jerlabel),
											 label	 = cms.untracked.string('AK4PFchs_phi'),
											),
							cms.PSet(record	= cms.string('JetResolutionScaleFactorRcd'),
											 tag		 = cms.string('JR_%s_SF_AK4PFchs'%jerlabel),
											 label	 = cms.untracked.string('AK4PFchs'),
											),
							cms.PSet(record	= cms.string('JetResolutionRcd'),
											 tag		 = cms.string('JR_%s_PtResolution_AK4PFPuppi'%jerlabel),
											 label	 = cms.untracked.string('AK4PFPuppi_pt'),
											),
							cms.PSet(record	= cms.string('JetResolutionRcd'),
											 tag		 = cms.string('JR_%s_PhiResolution_AK4PFPuppi'%jerlabel),
											 label	 = cms.untracked.string('AK4PFPuppi_phi'),
											),
							cms.PSet(record	= cms.string('JetResolutionScaleFactorRcd'),
											 tag		 = cms.string('JR_%s_SF_AK4PFPuppi'%jerlabel),
											 label	 = cms.untracked.string('AK4PFPuppi'),
											),
						 )
				)
process.jer.connect = cms.string('sqlite:jer/%s.db'%jerlabel)
process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'jer')


#-----------------------ELECTRON ID-------------------------------
from PandaProd.Ntupler.egammavid_cfi import *

initEGammaVID(process,options)

#### RECOMPUTE JEC From GT ###
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
 
jecLevels= ['L1FastJet',	'L2Relative', 'L3Absolute']
if options.isData:
				jecLevels.append('L2L3Residual')
 
updateJetCollection(
		process,
		jetSource = process.PandaNtupler.chsAK4,
		labelName = 'UpdatedJEC',
		jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')	
)

process.PandaNtupler.chsAK4=cms.InputTag('updatedPatJetsUpdatedJEC') # replace CHS with updated JEC-corrected

process.jecSequence = cms.Sequence( process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC)

########### MET Filter ################
process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadPFMuonFilter.taggingMode = cms.bool(True)

process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadChargedCandidateFilter.taggingMode = cms.bool(True)

process.metfilterSequence = cms.Sequence(process.BadPFMuonFilter
																					*process.BadChargedCandidateFilter)

if not options.isData:
	process.PandaNtupler.metfilter = cms.InputTag('TriggerResults','','PAT')

############ RECOMPUTE PUPPI/MET #######################
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(process,				 ## PF MET
														isData=isData)

process.PandaNtupler.pfmet = cms.InputTag('slimmedMETs','','PandaNtupler')
process.MonoXFilter.met = cms.InputTag('slimmedMETs','','PandaNtupler')

from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD

makePuppiesFromMiniAOD(process,True)
process.puppi.useExistingWeights = False # I still don't trust miniaod...
process.puppiNoLep.useExistingWeights = False


runMetCorAndUncFromMiniAOD(process,				 ## Puppi MET
                            isData=options.isData,
                            metType="Puppi",
                            pfCandColl=cms.InputTag("puppiForMET"),
                            recoMetFromPFCs=True,
                            jetFlavor="AK4PFPuppi",
                            postfix="Puppi")
process.puppiForMET.photonId = process.PandaNtupler.phoLooseIdMap
process.PandaNtupler.puppimet = cms.InputTag('slimmedMETsPuppi','','PandaNtupler')
process.MonoXFilter.puppimet = cms.InputTag('slimmedMETsPuppi','','PandaNtupler')

############### REGRESSION EGM #############
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)

########## EGM Smear and Scale ###
process.load('Configuration.StandardSequences.Services_cff')
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
            calibratedPatElectrons  = cms.PSet( initialSeed = cms.untracked.uint32(81),
            engineName = cms.untracked.string('TRandom3'),
            ),
            calibratedPatPhotons  = cms.PSet( initialSeed = cms.untracked.uint32(81),
            engineName = cms.untracked.string('TRandom3'),
            ),
        )
process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi')
process.load('EgammaAnalysis.ElectronTools.calibratedPhotonsRun2_cfi')
process.calibratedPatElectrons.electrons=process.PandaNtupler.electrons
process.calibratedPatPhotons.photons= process.PandaNtupler.photons
process.PandaNtupler.electrons =  cms.InputTag("calibratedPatElectrons")
process.PandaNtupler.photons = cms.InputTag("calibratedPatPhotons")
#######################################

# modify electrons Input Tags
process.egmGsfElectronIDs.physicsObjectSrc = process.PandaNtupler.electrons
process.electronIDValueMapProducer.srcMiniAOD= process.PandaNtupler.electrons
process.electronMVAValueMapProducer.srcMiniAOD= process.PandaNtupler.electrons

# modify photons Input Tags
process.egmPhotonIsolation.srcToIsolate = process.PandaNtupler.photons
process.egmPhotonIDs.physicsObjectSrc = process.PandaNtupler.photons
process.photonIDValueMapProducer.srcMiniAOD= process.PandaNtupler.photons
process.photonMVAValueMapProducer.srcMiniAOD= process.PandaNtupler.photons 
process.puppiForMET.photonName  = process.PandaNtupler.photons
process.puppiPhoton.photonName = process.PandaNtupler.photons 
process.modifiedPhotons.src  = process.PandaNtupler.photons

############ RUN CLUSTERING ##########################
process.jetSequence = cms.Sequence()

# btag and patify puppi AK4 jets
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
##################### FAT JETS #############################

from PandaProd.Ntupler.makeFatJets_cff import initFatJets, makeFatJets
fatjetInitSequence = initFatJets(process,isData)
process.jetSequence += fatjetInitSequence

if process.PandaNtupler.doCHSAK8:
	ak8CHSSequence		= makeFatJets(process,
                                  isData=isData,
                                  pfCandidates='pfCHS',
                                  algoLabel='AK',
                                  jetRadius=0.8)
	process.jetSequence += ak8CHSSequence
if process.PandaNtupler.doPuppiAK8:
	ak8PuppiSequence	= makeFatJets(process,
                                  isData=isData,
                                  pfCandidates='puppi',
                                  algoLabel='AK',
                                  jetRadius=0.8)
	process.jetSequence += ak8PuppiSequence
if process.PandaNtupler.doCHSCA15:
	ca15CHSSequence	 = makeFatJets(process,
                                  isData=isData,
                                  pfCandidates='pfCHS',
                                  algoLabel='CA',
                                  jetRadius=1.5)
	process.jetSequence += ca15CHSSequence
if process.PandaNtupler.doPuppiCA15:
	ca15PuppiSequence = makeFatJets(process,
                                  isData=isData,
                                  pfCandidates='puppi',
                                  algoLabel='CA',
                                  jetRadius=1.5)
	process.jetSequence += ca15PuppiSequence

if not isData:
	process.ak4GenJetsYesNu = ak4GenJets.clone(src = 'packedGenParticles')
	process.jetSequence += process.ak4GenJetsYesNu
'''
'''

###############################

DEBUG=False
if DEBUG:
	pass
#	print "Process=",process, process.__dict__.keys()

process.p = cms.Path(
                        process.infoProducerSequence *
#                        process.triggerFilterSequence *
                        process.regressionApplication *
												process.calibratedPatElectrons * 
												process.calibratedPatPhotons * 
                        process.egmGsfElectronIDSequence *
                        process.egmPhotonIDSequence *
                        process.photonIDValueMapProducer *     # iso map for photons
                        process.electronIDValueMapProducer *   # iso map for photons
                        process.jecSequence *
                        process.fullPatMetSequence *           # pf MET 
                        process.puppiMETSequence *             # builds all the puppi collections
                        process.fullPatMetSequencePuppi *      # puppi MET
#                        process.monoXFilterSequence *          # filter
                        process.jetSequence *                  # patify ak4puppi and do all fatjet stuff
                        process.metfilterSequence *
                        process.PandaNtupler
                    )

if DEBUG:
	process.output = cms.OutputModule("PoolOutputModule",
                                    fileName = cms.untracked.string('pool.root'))
	process.output_step = cms.EndPath(process.output)

	process.schedule = cms.Schedule(process.p,
                                  process.output_step)
