//#include "daq.h"
#include <TFile.h>
#include <TTree.h>
class DataTree {
public:
  DataTree();
  DataTree(unsigned short boardId, unsigned int runnum);
  DataTree(unsigned short boardId);
  ~DataTree();
  unsigned long int fineTStampNear = 0;
  unsigned long int fineTStampFar = 0;
  unsigned long int currentTStamp = 0;
  long int delT;
  int longGateA = 0;
  int longGateB = 0;
  double qMean = 0.;

  unsigned int localRunnum = 0;
  unsigned short fboardId = 0;
  TTree *tree;
  TFile *fp;

  void Write();
  void Fill();
};
