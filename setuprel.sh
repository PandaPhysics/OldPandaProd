#!/bin/bash

git cms-merge-topic cms-met:metTool80X
git cms-merge-topic -u cms-met:CMSSW_8_0_X-METFilterUpdate
git cms-merge-topic emanueledimarco:ecal_smear_fix_80X
git clone -b ICHEP2016_v2 https://github.com/ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings
