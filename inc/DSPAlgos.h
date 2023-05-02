/*
**	Filename : DSPAlgos.h
**	2023-05-02
**	username : muon
*/
#ifndef DSPAlgos_h
#define DSPAlgos_h

#include <vector>

class DSPAlgos {
public:
  std::vector<short> *fWaveForm;
  unsigned long int fCoarseTStamp;
  unsigned long int fFineTStamp;

private:
  // To be provided from constructor
  short fThreshold;
  unsigned short fPretrigger;
  unsigned short fBaseline;
  unsigned short fShortGate;
  unsigned short fLongGate;
  unsigned short fWindowSize;

public:
  DSPAlgos();
  DSPAlgos(unsigned short windowSize);
  DSPAlgos(short threshold, unsigned short preTrigger, unsigned short baseline, unsigned short shortGate,
           unsigned short longGate,unsigned short windowSize);
  // Required Processors
  std::vector<short> SmoothenSignal();
  void CalculateBaseline();
  void CalculateCFD();
  void CalculateIntegratedCharge();

  // Requird Getters
  unsigned long int GetCoarseTStamp() const;
  unsigned long int GetFineTStamp() const;
  unsigned short GetPreTrigger()const;
  unsigned short GetBaseline()const;
  unsigned short GetShortGate()const;
  unsigned short GetLongGate()const;
  short GetThreshold()const;
  unsigned short GetWindowSize()const;
};

#endif
