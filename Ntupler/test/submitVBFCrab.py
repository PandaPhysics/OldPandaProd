from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call, check_output

import sys, os
from re import findall

### CHECK THAT CMS env and it is correct
pwd = os.environ['PWD']
if 'CMSSW_VERSION' not in os.environ:
    print "Do cmsenv!"
    exit(0)
version = os.environ['CMSSW_VERSION']
ok = False
for dir in reversed(pwd.split('/')):
    if version == dir : 
        ok = True
        break
if not ok:
    print "Do (redo) cmsenv (2) !"
    exit(0)


config = config()

config.General.requestName = 'PandaProd_request_XXX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

## JobType
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runVBFNtupler.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False']

config.JobType.inputFiles=['jec','jer']

### DATA configuration
config.Data.inputDataset = '/HplusToTauNu-M500/amarini-amarini_PrivateMC_HPlusToTauNu_June2015-16aa19d591b8b49c55c4508e7a7c9233/USER'
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = True

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 10
config.Data.totalUnits = -1

config.Data.outLFNDirBase = '/store/group/phys_exotica/monotop/pandaprod/v_8022_1_sync/' 
#config.Data.outLFNDirBase = '/store/user/%s/scramjet/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag ='PandA'

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']
config.Site.blacklist = ['T3_US_UCR','T3_TW_NTU_HEP']


if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
    config.General.workArea = 'Submission'

    def submit(config):
        ### for some reason only the first dataset is submitted correctly, work around
        if len(sys.argv) ==1:
            ## book the command and run python
            cmd = "python " + sys.argv[0] + " '" + config.General.requestName + "'"
            print "calling: "+cmd
            call(cmd,shell=True)
            return
        if len(sys.argv) > 1:
            ## if it is not in the request try the next
            if sys.argv[1] !=  config.General.requestName: return
            ###
            print "--- Submitting " + "\033[01;32m" + config.Data.inputDataset.split('/')[1] + "\033[00m"  + " ---"
            config.Data.outputDatasetTag = config.General.requestName
            try:
                crabCommand('submit', config = config)
            except HTTPException as hte:
                print "Failed submitting task: %s" % (hte.headers)
            except ClientException as cle:
                print "Failed submitting task: %s" % (cle)

    def setdata(value="True"):
        if value=='True':
            config.Data.splitting = 'LumiBased'
            #config.Data.lumiMask=None
            url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/"
            config.Data.lumiMask = url + "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
        else:
            config.Data.lumiMask = None
            config.Data.splitting = 'FileBased'

        for idx,par in enumerate(config.JobType.pyCfgParams):
            if "isData" in par:
                config.JobType.pyCfgParams[idx] = "isData=" + value
        return 
            
    def submitList(l):
        for ll in l:
            split = ll.split('/')
            config.Data.inputDataset = ll
            if split[-1]=='MINIAOD':
                config.General.requestName = split[1]+'_'+split[2]
            elif 'ext' in split[-2]:
                ext = findall('ext[0-9]+',split[-2])
                if len(ext)>0:
                    config.General.requestName = split[1] + '_' + ext[0]
                else:
                    config.General.requestName = split[1]
            else:
                config.General.requestName = split[1]
            submit(config)

    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################
    
    ###################################################
    setdata("True")
    ###################################################
    config.Data.unitsPerJob = 70

    submitList([
        ])
    '''
    '''
    
    config.Data.unitsPerJob = 30

    submitList([
        ])

    config.Data.unitsPerJob = 60

    submitList([
      ])
    '''
    '''
    ###################################################
    setdata("False")
    ###################################################

    config.Data.splitting = 'EventAwareLumiBased'
    config.Data.unitsPerJob = 20000
    submitList([
      '/EWKWPlus2Jets_WToLNu_M-50_13TeV-madgraph-pythia8/RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM',
      '/EWKZ2Jets_ZToLL_M-50_13TeV-madgraph-pythia8/RunIISpring16MiniAODv1-PUSpring16_80X_mcRun2_asymptotic_2016_v3-v2/MINIAODSIM',
      '/EWKZ2Jets_ZToNuNu_13TeV-madgraph-pythia8/RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM',
    ])


    ###################################################
    setdata("False")
    ###################################################

    config.Data.splitting = 'FileBased'
    config.Data.unitsPerJob = 1

    config.Data.unitsPerJob = 2
