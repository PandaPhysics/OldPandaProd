#!/bin/bash

# fixes for MET
git cms-merge-topic -u cms-met:fromCMSSW_8_0_20_postICHEPfilter
git cms-merge-topic ahinzmann:METRecipe_8020_Spring16
#git cms-merge-topic ahinzmann:METRecipe_8020_Moriond17
git cms-merge-topic cms-met:METRecipe_8020

# something ECAL smearing - did we need this?
git cms-merge-topic emanueledimarco:ecal_smear_fix_80X
git clone -b ICHEP2016_v2 https://github.com/ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings

# fixes for el ID - EA and also MVA
git cms-merge-topic ikrav:egm_id_80X_v2
git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-ElectronIdentification.git RecoEgamma/ElectronIdentification/data.new
rsync -avP RecoEgamma/ElectronIdentification/data.new/* RecoEgamma/ElectronIdentification/data/
