//#include "common.h"
#include "DataTree.h"
#include "daq.h"
DataTree::DataTree() {}
void DataTree::push_back(unsigned short sample, bool near) {
  if (near) {
    nearWaveForm.push_back(sample);
  } else {
    farWaveForm.push_back(sample);
  }
}

void DataTree::Reset() {
  fBoardId = 0;
  currentTStamp = 0;
  fineTStampNear = 0;
  fineTStampFar = 0;
  delT = 0;
  longGateA = 0;
  longGateB = 0;
  qMean = 0;
  nearWaveForm.clear();
  farWaveForm.clear();
}

DataTree::DataTree(unsigned short boardId, unsigned int runnum) {
  // DataTree::DataTree(unsigned short boardId){
  fboardId = boardId;
  currentTStamp = 0;
  localRunnum = runnum;
  // unsigned int runnum = GetRunNumber();
  std::string filename = boardVec[boardId] + "_Run_" + std::to_string(localRunnum) + "_" + GenerateFileName();
  fp = new TFile(filename.c_str(), "RECREATE");
  
  tree = new TTree("ftree", "ftree");
  tree->Branch("fBoardId", &fBoardId);
  tree->Branch("fCurrentTStamp", &currentTStamp);
  tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  /*tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);*/
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);
  if (saveWaveForm) {
    tree->Branch("fNearWaveForm", &nearWaveForm);
    tree->Branch("fFarWaveForm", &farWaveForm);
  }
}
// No Run number
DataTree::DataTree(unsigned short boardId) {
  // DataTree::DataTree(unsigned short boardId){
  fboardId = boardId;
  currentTStamp = 0;
  // localRunnum = runnum;
  // unsigned int runnum = GetRunNumber();
  // std::string filename=boardVec[boardId]+"_Run_"+std::to_string(localRunnum)+"_"+GenerateFileName();
  std::string filename = boardVec[boardId] + "_" + GenerateFileName();
  fp = new TFile(filename.c_str(), "RECREATE");
  tree = new TTree("ftree", "ftree");
  tree->Branch("fBoardId", &fBoardId);
  tree->Branch("fCurrentTStamp", &currentTStamp);
  tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  /*tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);*/
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);
  if (saveWaveForm) {
    tree->Branch("fNearWaveForm", &nearWaveForm);
    tree->Branch("fFarWaveForm", &farWaveForm);
  }
}

void DataTree::Fill() { tree->Fill(); }

DataTree::~DataTree() {
  // delete tree;
  // delete fp;
}

void DataTree::Write() {
  tree->Write();
  fp->Close();
  // std::cout << "localRunnum : " << localRunnum
  /*std::this_thread::sleep_for(std::chrono::milliseconds(100*fboardId));
  if(localRunnum==runnum){
          runnum++;
  }*/
}
