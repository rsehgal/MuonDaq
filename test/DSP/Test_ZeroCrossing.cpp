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
#include <TH1F.h>
#include "colors.h"
using namespace std;

TGraph* Plot(std::vector<short> *signal,std::string name){  
	
	std::vector<int> xvec;
  std::vector<int> yvec;
 
for (unsigned short j = 0; j < signal->size(); j++) {
      xvec.push_back((int)j);
      yvec.push_back((int)signal->at(j));
      // std::cout << fNearWaveForm->at(j) << std::endl;
    }

TGraph *gr = new TGraph(xvec.size(), &xvec[0], &yvec[0]);
    gr->SetTitle(name.c_str());
    gr->SetMarkerStyle(8);
    gr->SetMarkerSize(0.7);

return gr;
}


int main(int argc, char *argv[]) {

 // DSPAlgos dsp(3);

  TApplication *fApp = new TApplication("TEST", NULL, NULL);
  TCanvas *can = new TCanvas;
  can->Divide(3, 2);
  TFile *fp = new TFile(argv[1], "r");
  // TFile *fp = new TFile("Board5_ED_DIGI_20_04_2023_11hr_15min_17sec.root", "r");
  TTree *ftree = (TTree *)fp->Get("ftree");

  // Declaration of leaves types
  ULong_t fCoarseTStampNear;
  ULong_t fCoarseTStampFar;

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
  ftree->SetBranchAddress("fCoarseTStampNear", &fCoarseTStampNear);
  ftree->SetBranchAddress("fCoarseTStampFar", &fCoarseTStampFar);

  ftree->SetBranchAddress("fTime", &fTime);
  ftree->SetBranchAddress("fTNear", &fTNear);
  ftree->SetBranchAddress("fTFar", &fTFar);
  ftree->SetBranchAddress("fDelT", &fDelT);
  ftree->SetBranchAddress("fQNear", &fQNear);
  ftree->SetBranchAddress("fQFar", &fQFar);
  ftree->SetBranchAddress("fQMean", &fQMean);
  ftree->SetBranchAddress("fNearWaveForm", &fNearWaveForm);
  ftree->SetBranchAddress("fFarWaveForm", &fFarWaveForm);

  DSPAlgos dspNear;//(fCoarseTStampNear,fNearWaveForm);
  DSPAlgos dspFar;
  Long64_t nentries = ftree->GetEntries();
  //nentries = std::atoi(argv[2]);

  Long64_t nbytes = 0;

  unsigned long int fineTStampNear = 0;
  unsigned long int fineTStampFar = 0;

  TH1F *hist = new TH1F("DelT","DelT",1000,-500,500);
  TH1F *histZero = new TH1F("DelTZero","DelTZero",1000,-100,100);
  //TH1F *histZero = new TH1F("DelTZero","DelTZero",1000,100,300);

  //for (Long64_t i = nentries - 1; i < nentries; i++) {
  for (Long64_t i = 1; i < nentries; i++) {

    nbytes += ftree->GetEntry(i);

    //std::cout << fNearWaveForm->size() <<" : " << fFarWaveForm->size() << std::endl;
    //continue;

    dspNear.Set(fCoarseTStampNear,fNearWaveForm);
    
    std::vector<short> smooth_waveform = dspNear.SmoothenSignal();
    std::vector<short> baseline_subtracted_smooth= dspNear.CalculateBaselineSubtractedSignal(&smooth_waveform);
    std::vector<short> cfd_waveform_smooth = dspNear.CalculateCFD(baseline_subtracted_smooth);

    //dsp.CalculateFineTStamp(cfd_waveform_smooth);
    dspNear.CalculateFineTStamp(cfd_waveform_smooth,true);
    fineTStampNear = dspNear.GetFineTStamp();
    //std::cout << "Final FineTStamp in ps : " << dsp.GetFineTStamp() << std::endl;

    double edZeroCrossVal = fCoarseTStampNear-fTNear;
    double myZeroCrossVal = dspNear.CalculateZeroCrossing(cfd_waveform_smooth,true);
    std::cout << RED << "EDZeroCrossVal : " << edZeroCrossVal << " : MYZeroCrossVal : " << myZeroCrossVal << std::endl;
    //histZero->Fill(myZeroCrossVal);
    
    dspFar.Set(fCoarseTStampFar,fFarWaveForm);

    smooth_waveform = dspFar.SmoothenSignal();
    baseline_subtracted_smooth= dspFar.CalculateBaselineSubtractedSignal(&smooth_waveform);
    cfd_waveform_smooth = dspFar.CalculateCFD(baseline_subtracted_smooth);

    //dsp.CalculateFineTStamp(cfd_waveform_smooth);
    dspFar.CalculateFineTStamp(cfd_waveform_smooth,true);
    fineTStampFar = dspFar.GetFineTStamp();

    Long_t diff = fineTStampNear-fineTStampFar;
    //hist->Fill(1.*diff/1000.);
    diff = fCoarseTStampNear-fCoarseTStampFar;
    hist->Fill(diff);
  }

/*  hist->Draw();
  new TCanvas;
  histZero->Draw("hist");
*/
    fApp->Run();
}
