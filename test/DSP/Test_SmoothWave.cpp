/*
**	Filename : Test_SmoothWave.cpp
**	2023-05-02
**	username : muon
*/
#include "DSPAlgos.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char *argv[]) {

  DSPAlgos dsp(3);

  TApplication *fApp = new TApplication("TEST", NULL, NULL);
  TCanvas *can = new TCanvas;
  can->Divide(2, 2);
  TFile *fp = new TFile(argv[1], "r");
  // TFile *fp = new TFile("Board5_ED_DIGI_20_04_2023_11hr_15min_17sec.root", "r");
  TTree *ftree = (TTree *)fp->Get("ftree");

  // Declaration of leaves types
  ULong_t fTime;
  ULong_t fTNear;
  ULong_t fTFar;
  Long_t fDelT;
  Int_t fQNear;
  Int_t fQFar;
  Double_t fQMean;
  vector<short int> *fNearWaveForm = nullptr;
  vector<short int> *fFarWaveForm = nullptr;

  // Set branch addresses.
  ftree->SetBranchAddress("fTime", &fTime);
  ftree->SetBranchAddress("fTNear", &fTNear);
  ftree->SetBranchAddress("fTFar", &fTFar);
  ftree->SetBranchAddress("fDelT", &fDelT);
  ftree->SetBranchAddress("fQNear", &fQNear);
  ftree->SetBranchAddress("fQFar", &fQFar);
  ftree->SetBranchAddress("fQMean", &fQMean);
  ftree->SetBranchAddress("fNearWaveForm", &dsp.fWaveForm);

  Long64_t nentries = ftree->GetEntries();
  nentries = std::atoi(argv[2]);

  Long64_t nbytes = 0;
  std::vector<int> xvecNear;
  std::vector<int> yvecNear;
  std::vector<int> xvecFar;
  std::vector<int> yvecFar;
  std::vector<int> xvecCFD;
  std::vector<int> yvecCFD;



  for (Long64_t i = nentries - 1; i < nentries; i++) {

    nbytes += ftree->GetEntry(i);
    std::cout << fTNear << " : " << fTFar << " : " << fDelT << " : " << fQMean << std::endl;
    std::cout << "Raman ............" << std::endl;
    std::cout << "Size : " << dsp.fWaveForm->size() << std::endl;


    std::vector<short> smooth_waveform = dsp.SmoothenSignal();
    //std::vector<short> cfd_waveform = dsp.CalculateCFD(smooth_waveform);
    std::vector<short> cfd_waveform = dsp.CalculateCFD();

    // for (unsigned int j = 0; j < fNearWaveForm->size(); j++) {
    //  std::cout << (*fNearWaveForm)[j] << std::endl;
    //}

    for (unsigned short j = 0; j < dsp.fWaveForm->size(); j++) {
      xvecNear.push_back((int)j);
      yvecNear.push_back((int)dsp.fWaveForm->at(j));
      // std::cout << fNearWaveForm->at(j) << std::endl;
    }
    for (unsigned short j = 0; j < smooth_waveform.size(); j++) {
      xvecFar.push_back((int)j);
      yvecFar.push_back((int)smooth_waveform[j]);
      // std::cout << fNearWaveForm->at(j) << std::endl;
    }
    for (unsigned short j = 0; j < cfd_waveform.size(); j++) {
      xvecCFD.push_back((int)j);
      yvecCFD.push_back((int)cfd_waveform[j]);
      // std::cout << fNearWaveForm->at(j) << std::endl;
    }


    TGraph *grNear = new TGraph(xvecNear.size(), &xvecNear[0], &yvecNear[0]);
    grNear->SetTitle("Near Waveform");
    grNear->SetMarkerStyle(8);
    grNear->SetMarkerSize(0.7);
    // grNear->SetSmooth(true);
    TGraph *grFar = new TGraph(xvecFar.size(), &xvecFar[0], &yvecFar[0]);
    grFar->SetTitle("Smooth Near Waveform");
    grFar->SetMarkerStyle(8);
    grFar->SetMarkerSize(0.7);
    TGraph *grCFD = new TGraph(xvecCFD.size(), &xvecCFD[0], &yvecCFD[0]);
    grCFD->SetTitle("CFD of Smooth Near Waveform");
    grCFD->SetMarkerStyle(8);
    grCFD->SetMarkerSize(0.7);

    // grFar->SetSmooth(true);

    can->cd(1);
    grNear->Draw("acp");
    can->cd(2);
    grFar->Draw("acp");
    can->cd(3);
    grCFD->Draw("acp");

  }

  // ftree->SetBranchAddress("fNearWaveForm", &fNearWaveForm);
  // ftree->SetBranchAddress("fFarWaveForm", &fFarWaveForm);

  /*
    Long64_t nentries = ftree->GetEntries();

    nentries = std::atoi(argv[2]);//10;

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
      //for (unsigned int j = 0; j < fNearWaveForm->size(); j++) {
      //  std::cout << (*fNearWaveForm)[j] << std::endl;
      //}

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

  */
    fApp->Run();
}
