#!/bin/bash

# fixes for MET
git cms-merge-topic zdemirag:conflict_met_resolved

# ecal smearing
git cms-merge-topic emanueledimarco:ecal_smear_fix_80X

# el ID
git cms-merge-topic ikrav:egm_id_80X_v2
git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-ElectronIdentification.git RecoEgamma/ElectronIdentification/data.new
rsync -avP RecoEgamma/ElectronIdentification/data.new/* RecoEgamma/ElectronIdentification/data/
rm -rf  RecoEgamma/ElectronIdentification/data.new/

# pho ID
git cms-merge-topic ikrav:egm_id_80X_v3_photons
git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-PhotonIdentification.git RecoEgamma/PhotonIdentification/data.new
rsync -avP RecoEgamma/PhotonIdentification/data.new/* RecoEgamma/PhotonIdentification/data/
rm -rf RecoEgamma/PhotonIdentification/data.new/

# ECAL smearing 
git cms-add-pkg EgammaAnalysis/ElectronTools
git cms-merge-topic shervin86:Moriond2017_JEC_energyScales
git clone git@github.com:ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings.new
rsync -avP EgammaAnalysis/ElectronTools/data/ScalesSmearings.new/* EgammaAnalysis/ElectronTools/data/ScalesSmearings/

# ECAL regression
git cms-merge-topic rafaellopesdesa:Regression80XEgammaAnalysis_v2
