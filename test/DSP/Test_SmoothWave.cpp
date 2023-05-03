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
  //ftree->SetBranchAddress("fNearWaveForm", &dsp.fWaveForm);

  DSPAlgos dsp;//(fCoarseTStampNear,fNearWaveForm);

  Long64_t nentries = ftree->GetEntries();
  nentries = std::atoi(argv[2]);

  Long64_t nbytes = 0;

  for (Long64_t i = nentries - 1; i < nentries; i++) {

    nbytes += ftree->GetEntry(i);

    dsp.Set(fCoarseTStampNear,fNearWaveForm);
    std::cout << "CoarseTStampNear : " << fCoarseTStampNear <<" : CoarseTStampFar : " << fCoarseTStampFar << std::endl;
    std::cout << fTNear << " : " << fTFar << " : " << fDelT << " : " << fQMean << std::endl;
    std::cout << "Raman ............" << std::endl;
    std::cout << "Size : " << dsp.fWaveForm->size() << std::endl;


    std::vector<short> smooth_waveform = dsp.SmoothenSignal();
    std::vector<short> baseline_subtracted_raw = dsp.CalculateBaselineSubtractedSignal();
    std::vector<short> baseline_subtracted_smooth= dsp.CalculateBaselineSubtractedSignal(&smooth_waveform);

    TGraph *grNear = Plot(dsp.fWaveForm,"Raw WaveForm");
    can->cd(1);
    grNear->Draw("acp");
    TGraph *grSmooth = Plot(&smooth_waveform,"Smooth WaveForm");
    can->cd(2);
    grSmooth->Draw("acp");
    TGraph *grBSRaw = Plot(&baseline_subtracted_raw,"Baseline subtracted Raw WaveForm");
    can->cd(3);
    grBSRaw->Draw("acp");
    TGraph *grBSSmooth = Plot(&baseline_subtracted_smooth,"Baseline subtracted Smooth WaveForm");
    can->cd(4);
    grBSSmooth->Draw("acp");

    std::vector<short> cfd_waveform = dsp.CalculateCFD(baseline_subtracted_raw);
    std::vector<short> cfd_waveform_smooth = dsp.CalculateCFD(baseline_subtracted_smooth);

    TGraph *grCFDRaw = Plot(&cfd_waveform,"CFD Raw WaveForm");
    can->cd(5);
    grCFDRaw->Draw("acp");
    TGraph *grCFDSmooth = Plot(&cfd_waveform_smooth,"CFD Smooth WaveForm");
    can->cd(6);
    grCFDSmooth->Draw("acp");


    //dsp.CalculateFineTStamp(cfd_waveform_smooth);
    dsp.CalculateFineTStamp(cfd_waveform_smooth,true);
    std::cout << "Final FineTStamp in ps : " << dsp.GetFineTStamp() << std::endl;

  }

    fApp->Run();
}
