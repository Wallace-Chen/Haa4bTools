# Haa4bTools

This tool is based on stop analysis team alpha, where i am working on for 3 years, with great colleagues. <br />

## Set up BDT training framework
```bash
cmsrel CMSSW_9_2_14_patch1
cd CMSSW_9_2_14_patch1/src
cmsenv
mkdir UserCode
cd $CMSSW_BASE/src/UserCode
git clone git@github.com:weihuacern/Haa4bTools.git
```

## Compile Code
Note, make sure make clean before commit. <br />
```bash
cd $CMSSW_BASE/src/UserCode/Haa4bTools/Tools
make
cd $CMSSW_BASE/src/UserCode/Haa4bTools/AnaModules
make
```

## W/b-jets tagger with Machine learning algorithm

### Input: root files and weights
```bash
ls $CMSSW_BASE/src/UserCode/Haa4bTools/AnaModules/FileList
```
In MVAAna.cc: <br />
```cpp
QCDSampleWeight mySGSampleWeight;
mySGSampleWeight.QCDSampleInfo_push_back( "_Wh_amass12"  , 1.37, 397600 - 0, LumiScale, 1, "FileList/MVASGMCFileList.txt", TrainMode );
//...
QCDSampleWeight myBGSampleWeight;
float TTbar_SingleLept_BR = 0.43930872; // 2*W_Lept_BR*(1-W_Lept_BR)
float TTbar_DiLept_BR = 0.10614564; // W_Lept_BR^2
myBGSampleWeight.QCDSampleInfo_push_back( "_SingleT_s_" , 3.362, 811495 - 188505, LumiScale, 1, "FileList/MVABGMCFileList.txt", TrainMode );
//...
QCDSampleWeight myDataSampleWeight;
myDataSampleWeight.QCDSampleInfo_push_back( "_SingleElectron2016B_ver2" , 1 , 1, 1, 1, "FileList/MVADataFileList.txt", TrainMode );
//...
```
Make sure to use correct xSec and weight, in TMVATrainer.h: <br />
```cpp
std::string dir = "root://eosuser.cern.ch//eos/user/h/hua/Haa4b/MVARes/";
std::make_pair( std::string("mva_MC13TeV_SingleT_s_2016.root" ), 0.193558)
std::make_pair( std::string("mva_MC13TeV_SingleT_atW_2016.root" ), 0.184169)
//...
```
To calculate xSec weight, use MVAAna.cc: <br />

### Train: boosted decision tree
```bash
./MVAAna Train TribMVA
./MVAAna Train QuabMVA
```
Model .xml and .C files are generated: <br />
```bash
ls $CMSSW_BASE/src/UserCode/Haa4bTools/AnaModules/dataset/weights
Haa4bSBClassificationQuabMVA_BDT.class.C  Haa4bSBClassificationQuabMVA_BDT.weights.xml  Haa4bSBClassificationTribMVA_BDT.class.C  Haa4bSBClassificationTribMVA_BDT.weights.xml
```
Note, training BDT will also generate two big root files... Please make sure not include them when commit. <br />

### Application: use trained model for background suppression
Run analysis code to generate root for plot: <br />
```bash
./MVAAna Application TribMVA
./MVAAna Application QuabMVA
```
Location of the root file for plot: <br />
```bash
ls $CMSSW_BASE/src/UserCode/Haa4bTools/AnaModules/OutDir
MVACutFlowQuabMVA.root  MVACutFlowTribMVA.root
```

### Plotting
Plots for BDT training cross section, train vs test: <br />
```bash
./MVAOutPlots TribMVA Test
./MVAOutPlots QuabMVA Test
```
Inputs are from Train BDT step (big root file). Path can be customized in MVAOutPlots.h: <br />
```cpp
std::string din = "/afs/cern.ch/user/h/hua/workarea/Haa4b/TrainRes/20171130/";
```
Outputs will be in the Test directory. <br />

Plots for distribution checks, and sensitivity study: <br />
```bash
./MVACutFlowPlots TribMVA Test
./MVACutFlowPlots QuabMVA Test
```
Inputs are from Application step. <br />

Outputs will be in the Test directory. <br />
Make sure delete Test directory before commit. <br />

### Others

Generate Ntuple file list: <br />
```bash
cd $CMSSW_BASE/src/UserCode/Haa4bTools/EOSTools
python genList.py -d /eos/cms/store/user/georgia/results_2018_03_03 -l
python genList.py -d /eos/cms/store/user/georgia/results_2018_03_03/MinBias -l
```

Special treat for WXJets and DYXJets when train model: <br />
```bash
cd $CMSSW_BASE/src/UserCode/Haa4bTools/AnaModules
./LHESplit LHENJets.root 0
./LHESplit LHENJets.root 1
./LHESplit LHENJets.root 2
./LHESplit LHENJets.root 3
./LHESplit LHENJets.root 4
```
, where the LHENJets.root can be WJets or DY_10to50, DY_50toInf, hadd all root files from same physics process including XJets files. <br />
