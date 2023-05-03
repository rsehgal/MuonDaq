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
void DataTree::push_back(unsigned short sample) { fWaveForm.push_back(sample); }

void DataTree::Reset() {
  fBoardId = 0;
  fBoardName = "";
  currentTStamp = 0;
  fineTStampNear = 0;
  fineTStampFar = 0;
  fCoarseTStampNear = 0;
  fCoarseTStampFar = 0;
  delT = 0;
  longGateA = 0;
  longGateB = 0;
  qMean = 0;
  nearWaveForm.clear();
  farWaveForm.clear();

  // Data member for singles
  fChannelNo = 0;
  fineTStamp = 0;
  longGate = 0;
  fCoarseTStamp = 0;
  fWaveForm.clear();
}

void DataTree::clear() { fWaveForm.clear(); }

DataTree::DataTree(unsigned short boardId, unsigned int runnum) {
  // DataTree::DataTree(unsigned short boardId){
  fboardId = boardId;
  fBoardName = boardVec[boardId];
  currentTStamp = 0;
  localRunnum = runnum;
  // unsigned int runnum = GetRunNumber();
  std::string filename = boardVec[boardId] + "_Run_" + std::to_string(localRunnum) + "_" + GenerateFileName();
  fp = new TFile(filename.c_str(), "RECREATE");

  tree = new TTree("ftree", "ftree");
  tree->Branch("fBoardName", &fBoardName);
  tree->Branch("fBoardId", &fBoardId);

  if (coincidence) {

    tree->Branch("fCoarseTStampNear", &fCoarseTStampNear);
    tree->Branch("fCoarseTStampFar", &fCoarseTStampFar);

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
  } else {
    tree->Branch("fChannelNo", &fChannelNo);
    tree->Branch("fCoarseTStamp", &fCoarseTStamp);
    tree->Branch("fTStamp", &fineTStamp);
    tree->Branch("fQlong", &longGate);
    if (saveWaveForm)
      tree->Branch("fWaveForm", &fWaveForm);
  }
  tree->Branch("fTime", &currentTStamp);
}
// No Run number
DataTree::DataTree(unsigned short boardId) {
  // DataTree::DataTree(unsigned short boardId){
  fboardId = boardId;
  fBoardName = boardVec[boardId];
  currentTStamp = 0;
  // localRunnum = runnum;
  // unsigned int runnum = GetRunNumber();
  // std::string filename=boardVec[boardId]+"_Run_"+std::to_string(localRunnum)+"_"+GenerateFileName();
  std::string filename = boardVec[boardId] + "_" + GenerateFileName();
  fp = new TFile(filename.c_str(), "RECREATE");
  tree = new TTree("ftree", "ftree");
  tree->Branch("fBoardName", &fBoardName);
  tree->Branch("fBoardId", &fBoardId);
  if (coincidence) {
    tree->Branch("fCoarseTStampNear", &fCoarseTStampNear);
    tree->Branch("fCoarseTStampFar", &fCoarseTStampFar);

    tree->Branch("fTNear", &fineTStampNear);
    tree->Branch("fTFar", &fineTStampFar);
    tree->Branch("fDelT", &delT);
    tree->Branch("fQNear", &longGateA);
    tree->Branch("fQFar", &longGateB);
    tree->Branch("fQMean", &qMean);
    if (saveWaveForm) {
      tree->Branch("fNearWaveForm", &nearWaveForm);
      tree->Branch("fFarWaveForm", &farWaveForm);
    }
  } else {
    tree->Branch("fChannelNo", &fChannelNo);
    tree->Branch("fCoarseTStamp", &fCoarseTStamp);
    tree->Branch("fTStamp", &fineTStamp);
    tree->Branch("fQlong", &longGate);
    if (saveWaveForm)
      tree->Branch("fWaveForm", &fWaveForm);
  }
  tree->Branch("fTime", &currentTStamp);
  /*tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  //tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);
  if (saveWaveForm) {
    tree->Branch("fNearWaveForm", &nearWaveForm);
    tree->Branch("fFarWaveForm", &farWaveForm);
  }*/
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
