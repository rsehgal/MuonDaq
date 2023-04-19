//#include "common.h"
#include "DataTree.h"
#include "daq.h"
DataTree::DataTree() {}

DataTree::DataTree(unsigned short boardId, unsigned int runnum) {
  // DataTree::DataTree(unsigned short boardId){
  fboardId = boardId;
  currentTStamp = 0;
  localRunnum = runnum;
  // unsigned int runnum = GetRunNumber();
  std::string filename = boardVec[boardId] + "_Run_" + std::to_string(localRunnum) + "_" + GenerateFileName();
  fp = new TFile(filename.c_str(), "RECREATE");
  tree = new TTree("ftree", "ftree");
  tree->Branch("fCurrentTStamp", &currentTStamp);
  tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  /*tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);*/
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);
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
  tree->Branch("fCurrentTStamp", &currentTStamp);
  tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  /*tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);*/
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);
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
