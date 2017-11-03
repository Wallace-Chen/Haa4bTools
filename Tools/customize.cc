#include "customize.h"

namespace AnaFunctions
{
  bool jetPassCuts(const TLorentzVector& jet, const AnaConsts::AccRec& jetCutsArr)
  {
    const double minAbsEta = jetCutsArr.minAbsEta, maxAbsEta = jetCutsArr.maxAbsEta, minPt = jetCutsArr.minPt, maxPt = jetCutsArr.maxPt;
    double perjetpt = jet.Pt(), perjeteta = jet.Eta();
    return  ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
      && ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
      && (     minPt == -1 || perjetpt >= minPt )
      && (     maxPt == -1 || perjetpt < maxPt );
  }       

  int countJets(const std::vector<TLorentzVector> &inputJets, const AnaConsts::AccRec& jetCutsArr)
  {
    int cntNJets =0;
    for(unsigned int ij=0; ij<inputJets.size(); ij++){
      if(jetPassCuts(inputJets[ij], jetCutsArr)) cntNJets ++;
    }
    return cntNJets;
  }

  int countCSVS(const std::vector<TLorentzVector> &inputJets, const std::vector<double> &inputCSVS, const double cutCSVS, const AnaConsts::AccRec& jetCutsArr)
  {
    const double minAbsEta = jetCutsArr.minAbsEta, maxAbsEta = jetCutsArr.maxAbsEta, minPt = jetCutsArr.minPt, maxPt = jetCutsArr.maxPt;
    int cntNJets =0;
    for(unsigned int ij=0; ij<inputJets.size(); ij++){
      if( !jetPassCuts(inputJets[ij], jetCutsArr) ) continue;
      if( std::isnan(inputCSVS[ij]) ) continue;
      if( inputCSVS[ij] > cutCSVS ) cntNJets ++;
    }
    return cntNJets;
  }

  std::vector<double> calcDPhi(const std::vector<TLorentzVector> &inputJets, const double metphi, const int nDPhi, const AnaConsts::AccRec& jetCutsArr)
  {
    const double minAbsEta = jetCutsArr.minAbsEta, maxAbsEta = jetCutsArr.maxAbsEta, minPt = jetCutsArr.minPt, maxPt = jetCutsArr.maxPt;
    int cntNJets =0;
    std::vector<double> outDPhiVec(nDPhi, 999);
    for(unsigned int ij=0; ij<inputJets.size(); ij++){
      if( !jetPassCuts(inputJets[ij], jetCutsArr) ) continue;
      if( cntNJets < nDPhi ){
        double perDPhi = fabs(TVector2::Phi_mpi_pi( inputJets[ij].Phi() - metphi ));
        outDPhiVec[cntNJets] = perDPhi;
      }
      cntNJets ++;
    }
    return outDPhiVec;
  }

  double calcDeltaT(unsigned int pickedJetIdx, const std::vector<TLorentzVector> &inputJets, const AnaConsts::AccRec& jetCutsArr)
  {
    double deltaT = 0;
    double jres = 0.1;
    double sum = 0.0;
    for(unsigned int ij=0; ij<inputJets.size(); ij++)
    {
      if( ij == pickedJetIdx ) continue;
      if( !jetPassCuts(inputJets.at(ij), jetCutsArr) ) continue;
      sum = sum + ( inputJets.at(pickedJetIdx).Px()*inputJets.at(ij).Py() - inputJets.at(ij).Px()*inputJets.at(pickedJetIdx).Py() ) * ( inputJets.at(pickedJetIdx).Px()*inputJets.at(ij).Py() - inputJets.at(ij).Px()*inputJets.at(pickedJetIdx).Py() );
    }
    deltaT = jres*sqrt(sum)/inputJets.at(pickedJetIdx).Pt();

    return deltaT;
  }

  std::vector<double> calcDPhiN(const std::vector<TLorentzVector> &inputJets, const TLorentzVector &metLVec, const int nDPhi, const AnaConsts::AccRec& jetCutsArr)
  {
    int cntNJets =0;
    std::vector<double> outDPhiVec(nDPhi, 999);
    for(unsigned int ij=0; ij<inputJets.size(); ij++){
      if( !jetPassCuts(inputJets.at(ij), jetCutsArr) ) continue;
      if( cntNJets < nDPhi ){
        double perDphi = std::abs(TVector2::Phi_mpi_pi( inputJets.at(ij).Phi() - metLVec.Phi() ));
        double dT = calcDeltaT(ij, inputJets, jetCutsArr);
        if( dT/metLVec.Pt()>=1.0 ) outDPhiVec[cntNJets] = perDphi/(TMath::Pi()/2.0);
        else outDPhiVec[cntNJets] = perDphi/std::asin(dT/metLVec.Pt());
      }
      cntNJets ++; 
    }
    return outDPhiVec;
  }
  //muon
  bool passMuon(const TLorentzVector& muon, const double& muonIso, const double& muonMtw, int flagID, const AnaConsts::MuIsoAccRec& muonsArr)
  {
    const double minAbsEta = muonsArr.minAbsEta, maxAbsEta = muonsArr.maxAbsEta, minPt = muonsArr.minPt, maxPt = muonsArr.maxPt, maxIso = muonsArr.maxIso, maxMtw = muonsArr.maxMtw; 
    double permuonpt = muon.Pt(), permuoneta = muon.Eta();
    return ( minAbsEta == -1 || fabs(permuoneta) >= minAbsEta )
      && ( maxAbsEta == -1 || fabs(permuoneta) < maxAbsEta )
      && (     minPt == -1 || permuonpt >= minPt )
      && (     maxPt == -1 || permuonpt < maxPt )
      && (    maxIso == -1 || muonIso < maxIso )
      && (    maxMtw == -1 || muonMtw < maxMtw )
      && flagID;
  }

  int countMuons(const std::vector<TLorentzVector> &muonsLVec, const std::vector<double> &muonsRelIso, const std::vector<double> &muonsMtw, const std::vector<int> &muonsFlagID, const AnaConsts::MuIsoAccRec& muonsArr)
  {

    int cntNMuons = 0;
    for(unsigned int im=0; im<muonsLVec.size(); im++){
      if(passMuon(muonsLVec[im], muonsRelIso[im], muonsMtw[im], muonsFlagID[im], muonsArr)) cntNMuons ++;
    }
    return cntNMuons;
  }
  //end muon
  //electron
  bool passElectron(const TLorentzVector& elec, const double electronIso, const double electronMtw, bool isEB, int flagID, const AnaConsts::ElIsoAccRec& elesArr)
  {
    const double minAbsEta = elesArr.minAbsEta, maxAbsEta = elesArr.maxAbsEta, minPt = elesArr.minPt, maxPt = elesArr.maxPt, maxIso = (isEB)?(elesArr.maxIsoEB):(elesArr.maxIsoEE), maxMtw = elesArr.maxMtw;
    double perelectronpt = elec.Pt(), perelectroneta = elec.Eta();
    return ( minAbsEta == -1 || fabs(perelectroneta) >= minAbsEta )
      && ( maxAbsEta == -1 || fabs(perelectroneta) < maxAbsEta )
      && (     minPt == -1 || perelectronpt >= minPt )
      && (     maxPt == -1 || perelectronpt < maxPt ) 
      && (    maxIso == -1 || electronIso < maxIso )
      && (    maxMtw == -1 || electronMtw < maxMtw )
      && flagID;
  }

  int countElectrons(const std::vector<TLorentzVector> &electronsLVec, const std::vector<double> &electronsRelIso, const std::vector<double> &electronsMtw, const std::vector<unsigned int>& isEBVec, const std::vector<int> &electronsFlagID, const AnaConsts::ElIsoAccRec& elesArr)
  {
    int cntNElectrons = 0;
    for(unsigned int ie=0; ie<electronsLVec.size(); ie++)
    {
      if(passElectron(electronsLVec[ie], electronsRelIso[ie], electronsMtw[ie], isEBVec[ie], electronsFlagID[ie], elesArr)) cntNElectrons ++;
    }
    return cntNElectrons;
  }
  //end electron

  // cntNJetsVec stores number of jets counters from tightest requirement to loosest requirement.
  // cutCSVS is the CSV cut value
  void preparecntNJets(const std::vector<TLorentzVector> &inijetsLVec, const std::vector<double> &inirecoJetsBtag, const double cutCSVS, std::vector<int> &cntNJetsVec, std::vector<int> &cntNbJetsVec)
  {
    cntNJetsVec.clear(); cntNbJetsVec.clear();
    int cntNJetsPt50Eta24 = countJets(inijetsLVec, AnaConsts::pt50Eta24Arr);
    int cntNJetsPt30Eta24 = countJets(inijetsLVec, AnaConsts::pt30Eta24Arr);
    int cntNJetsPt30      = countJets(inijetsLVec, AnaConsts::pt30Arr);
    cntNJetsVec.push_back(cntNJetsPt50Eta24);
    cntNJetsVec.push_back(cntNJetsPt30Eta24);
    cntNJetsVec.push_back(cntNJetsPt30);

    int cntCSVS = countCSVS(inijetsLVec, inirecoJetsBtag, cutCSVS, AnaConsts::bTagArr);
    cntNbJetsVec.push_back(cntCSVS);
  }

  void preparecalcDPhi(const std::vector<TLorentzVector> &inijetsLVec, const double metphi, std::vector<double> &outDPhiVec)
  {
    outDPhiVec.clear();
    outDPhiVec = calcDPhi(inijetsLVec, metphi, 3, AnaConsts::dphiArr);
  }

  void prepareForNtupleReader()
  {
    using namespace AnaConsts;
    activatedBranchNames.insert(activatedBranchNames_DataOnly.begin(), activatedBranchNames_DataOnly.end());
  }

  double calcHT(const std::vector<TLorentzVector> &inputJets, const AnaConsts::AccRec& jetCutsArr)
  {
    const double minAbsEta = jetCutsArr.minAbsEta, maxAbsEta = jetCutsArr.maxAbsEta, minPt = jetCutsArr.minPt, maxPt = jetCutsArr.maxPt;

    double ht = 0;
    for(unsigned int ij=0; ij<inputJets.size(); ij++)
    {
      double perjetpt = inputJets[ij].Pt(), perjeteta = inputJets[ij].Eta();
      if(   ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
          && ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
          && (     minPt == -1 || perjetpt >= minPt )
          && (     maxPt == -1 || perjetpt < maxPt ) )
      {
        ht += perjetpt;
      }
    }
    return ht;
  }

  TLorentzVector calcMHT(const std::vector<TLorentzVector> &inputJets, const AnaConsts::AccRec& jetCutsArr)
  {
    const double minAbsEta = jetCutsArr.minAbsEta, maxAbsEta = jetCutsArr.maxAbsEta, minPt = jetCutsArr.minPt, maxPt = jetCutsArr.maxPt;

    TLorentzVector mhtLVec;
    for(unsigned int ij=0; ij<inputJets.size(); ij++)
    {
      double perjetpt = inputJets[ij].Pt(), perjeteta = inputJets[ij].Eta();
      if(   ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
          && ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
          && (     minPt == -1 || perjetpt >= minPt )
          && (     maxPt == -1 || perjetpt < maxPt ) )
      {
        TLorentzVector tmpLVec;
        tmpLVec.SetPtEtaPhiM( inputJets[ij].Pt(), 0, inputJets[ij].Phi(), 0 );
        mhtLVec -= tmpLVec;
      }
    }
    return mhtLVec;
  }

  bool passBaseline()
  {
    return true;
  }

  int jetLepdRMatch(const TLorentzVector& lep, const std::vector<TLorentzVector>& jetsLVec, const double jldRMax)
  {
    double dRmin = 999.0;
    int minJMatch = -1;

    for(int iJet = 0; iJet < jetsLVec.size(); ++iJet)
    {
      double dR = ROOT::Math::VectorUtil::DeltaR(jetsLVec[iJet], lep);
      if(dR < dRmin)
      {
        dRmin = dR;
        minJMatch = iJet;
      }
    }
    if(dRmin < jldRMax) return minJMatch;
    else                return -1;
  }
}
