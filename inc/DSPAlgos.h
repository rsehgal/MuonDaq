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
  double fFraction;
  short fDelay;

public:
  DSPAlgos();
  DSPAlgos(unsigned short windowSize);
  DSPAlgos(unsigned long int coarseTStamp, std::vector<short> *waveForm);
  void Set(unsigned long int coarseTStamp, std::vector<short> *waveForm);
  DSPAlgos(short threshold, unsigned short preTrigger, unsigned short baseline, unsigned short shortGate,
           unsigned short longGate, unsigned short windowSize, double fraction, short delay);
  // Required Processors
  std::vector<short> SmoothenSignal();
  short CalculateBaseline(std::vector<short> *signal);
  std::vector<short> CalculateBaselineSubtractedSignal();
  std::vector<short> CalculateBaselineSubtractedSignal(std::vector<short> *signal);
  std::vector<short> CalculateCFD();
  std::vector<short> CalculateCFD(std::vector<short> signal);
  void CalculateIntegratedCharge();
  void CalculateFineTStamp(std::vector<short> signal, bool ns = true);
  double CalculateInterpolatedValue(double x1, double y1, double x2, double y2);
  double CalculateZeroCrossing(std::vector<short> signal, bool ns);

  // Requird Getters
  unsigned long int GetCoarseTStamp() const;
  unsigned long int GetFineTStamp() const;
  unsigned short GetPreTrigger() const;
  unsigned short GetBaseline() const;
  unsigned short GetShortGate() const;
  unsigned short GetLongGate() const;
  short GetThreshold() const;
  unsigned short GetWindowSize() const;
  double GetCFDFraction() const;
  short GetCFDDelay() const;
};

#endif
