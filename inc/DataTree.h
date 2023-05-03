//#include "daq.h"
#include <TFile.h>
#include <TTree.h>
class DataTree {
public:
  DataTree();
  DataTree(unsigned short boardId, unsigned int runnum);
  DataTree(unsigned short boardId);
  ~DataTree();
  unsigned short fBoardId=0;
  std::string fBoardName;
  unsigned long int fineTStampNear = 0;
  unsigned long int fineTStampFar = 0;
  unsigned long int currentTStamp = 0;
  unsigned long int fCoarseTStampNear = 0;
  unsigned long int fCoarseTStampFar = 0;
  long int delT;
  int longGateA = 0;
  int longGateB = 0;
  double qMean = 0.;

  //single
  unsigned short fChannelNo = 0;
  unsigned long int fineTStamp = 0;
  int longGate = 0;
  std::vector<short int> fWaveForm;
  unsigned long int fCoarseTStamp=0;

  std::vector<short int> nearWaveForm;
  std::vector<short int> farWaveForm;

  unsigned int localRunnum = 0;
  unsigned short fboardId = 0;
  TTree *tree;
  TFile *fp;

  void Write();
  void Fill();
  void push_back(unsigned short sample,bool near);
  void push_back(unsigned short sample);
  void clear();
  void Reset();
};
