# PandaProd

## Installation instructions

First, set up `CMSSW`:

```bash
cmsrel CMSSW_8_0_24_patch1
cd CMSSW_8_0_24/src
cmsenv
git cms-init
```

Then, we check out all necessary packages:

```bash
git clone https://github.com/PandaPhysics/PandaUtilities
git clone https://github.com/PandaPhysics/PandaProd
sh PandaProd/setuprel.sh           # checks out some CMSSW packages based on POG recommendations
```

Finally, compile:

```bash
scram b -j$(nproc)
```

## Running

The `cmsRun` executables are in `PandaProd/Ntupler/test`. 
You can use `testNtupler.py` to run a few events locally. 
To run on the grid, `runNtupler.py` is the configuration, and `submitCrab.py` is used to submit the jobs.
