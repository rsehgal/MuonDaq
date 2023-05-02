/*
**	Filename : DSPAlgos.cpp
**	2023-05-02
**	username : muon
*/
#include "DSPAlgos.h"
#include <iostream>

DSPAlgos::DSPAlgos() {
  fWaveForm = nullptr;
  fWindowSize = 10;
}

DSPAlgos::DSPAlgos(unsigned short windowSize) : fWindowSize(windowSize) { fWaveForm = nullptr; }

DSPAlgos::DSPAlgos(short threshold, unsigned short preTrigger, unsigned short baseline, unsigned short shortGate,
                   unsigned short longGate, unsigned short windowSize)
    : fThreshold(threshold), fPretrigger(preTrigger), fBaseline(baseline), fShortGate(shortGate), fLongGate(longGate),
      fWindowSize(windowSize) {
  fWaveForm = nullptr;
}

// Required Processors
std::vector<short> DSPAlgos::SmoothenSignal() {
  std::vector<short> tWaveForm;
  for (unsigned int i = 0; i < fWaveForm->size(); i++) {
    tWaveForm.push_back(fWaveForm->at(i));
  }
  std::vector<short> smoothed_signal(fWaveForm->size());

  // Loop over each point in the signal
  for (int i = 0; i < tWaveForm.size(); i++) {
    double sum = 0.0;
    int count = 0;

    // Loop over the data points in the moving average window centered on the current point
    for (int j = i - fWindowSize / 2; j <= i + fWindowSize / 2; j++) {
      // Check if the current window position is within the bounds of the signal
      if (j >= 0 && j < tWaveForm.size()) {
        sum += tWaveForm[j];
        count++;
      }
    }
    // Calculate the average of the data points in the window and store in the smoothed signal
    // std::cout << "I : " << i << " : Sum : " << sum << " : Count : " << count << std::endl;
    smoothed_signal[i] = sum / count;
    tWaveForm[i] = smoothed_signal[i];
  }
  return smoothed_signal;
}
void DSPAlgos::CalculateBaseline() {}
void DSPAlgos::CalculateCFD() {}
void DSPAlgos::CalculateIntegratedCharge() {}

// Required Getters
unsigned long int DSPAlgos::GetCoarseTStamp() const { return fCoarseTStamp; }
unsigned long int DSPAlgos::GetFineTStamp() const { return fFineTStamp; }
unsigned short DSPAlgos::GetPreTrigger() const { return fPretrigger; }
unsigned short DSPAlgos::GetBaseline() const { return fBaseline; }
unsigned short DSPAlgos::GetShortGate() const { return fShortGate; }
unsigned short DSPAlgos::GetLongGate() const { return fLongGate; }
short DSPAlgos::GetThreshold() const { return fThreshold; }
unsigned short DSPAlgos::GetWindowSize() const { return fWindowSize; }
