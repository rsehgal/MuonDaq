#include <TApplication.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <iostream>
#include <vector>
#include <TCanvas.h>
using namespace std;
int main(int argc, char *argv[]) {
  TApplication *fApp = new TApplication("TEST", NULL, NULL);
  TCanvas *can = new TCanvas;
  can->Divide(1,2);
  TFile *fp = new TFile(argv[1], "r");
  // TFile *fp = new TFile("Board5_ED_DIGI_20_04_2023_11hr_15min_17sec.root", "r");
  TTree *ftree = (TTree *)fp->Get("ftree");

  // Declaration of leaves types
  ULong_t fCurrentTStamp;
  ULong_t fTNear;
  ULong_t fTFar;
  Long_t fDelT;
  Int_t fQNear;
  Int_t fQFar;
  Double_t fQMean;
  vector<short int> *fNearWaveForm = nullptr;
  vector<short int> *fFarWaveForm = nullptr;

  // Set branch addresses.
  ftree->SetBranchAddress("fCurrentTStamp", &fCurrentTStamp);
  ftree->SetBranchAddress("fTNear", &fTNear);
  ftree->SetBranchAddress("fTFar", &fTFar);
  ftree->SetBranchAddress("fDelT", &fDelT);
  ftree->SetBranchAddress("fQNear", &fQNear);
  ftree->SetBranchAddress("fQFar", &fQFar);
  ftree->SetBranchAddress("fQMean", &fQMean);
  ftree->SetBranchAddress("fNearWaveForm", &fNearWaveForm);
  ftree->SetBranchAddress("fFarWaveForm", &fFarWaveForm);

  Long64_t nentries = ftree->GetEntries();

  nentries = 10;

  Long64_t nbytes = 0;
  std::vector<int> xvecNear;
  std::vector<int> yvecNear;
  std::vector<int> xvecFar;
  std::vector<int> yvecFar;

  for (Long64_t i = nentries - 1; i < nentries; i++) {

    nbytes += ftree->GetEntry(i);
    std::cout << fTNear << " : " << fTFar << " : " << fDelT << " : " << fQMean << std::endl;
    std::cout << "Raman ............" << std::endl;
    std::cout << "Size : " << fNearWaveForm->size() << std::endl;
    /*for (unsigned int j = 0; j < fNearWaveForm->size(); j++) {
      std::cout << (*fNearWaveForm)[j] << std::endl;
    }*/

    for (unsigned short j = 0; j < fNearWaveForm->size(); j++) {
      xvecNear.push_back((int)j);
      yvecNear.push_back((int)fNearWaveForm->at(j));
      //std::cout << fNearWaveForm->at(j) << std::endl;
    }
    for (unsigned short j = 0; j < fFarWaveForm->size(); j++) {
      xvecFar.push_back((int)j);
      yvecFar.push_back((int)fFarWaveForm->at(j));
      //std::cout << fNearWaveForm->at(j) << std::endl;
    }

     TGraph *grNear = new TGraph(xvecNear.size(), &xvecNear[0], &yvecNear[0]);
     grNear->SetTitle("Near Waveform");
     grNear->SetMarkerStyle(8);
     grNear->SetMarkerSize(0.7);
     //grNear->SetSmooth(true);
     TGraph *grFar =  new TGraph(xvecFar.size(), &xvecFar[0], &yvecFar[0]);
     grFar->SetTitle("Far Waveform");
     grFar->SetMarkerStyle(8);
     grFar->SetMarkerSize(0.7);
     //grFar->SetSmooth(true);


     can->cd(1);
     grNear->Draw("acp");
     can->cd(2);
     grFar->Draw("acp");

  }

  fApp->Run();
}
