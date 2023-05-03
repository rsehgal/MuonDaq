/*
**	Filename : DSPAlgos.cpp
**	2023-05-02
**	username : muon
*/
#include "DSPAlgos.h"
#include <iostream>

DSPAlgos::DSPAlgos() {
  fWaveForm = nullptr;
  fWindowSize = 3;
  fBaseline = 30; // (120ns / 4ns)
  fThreshold = 10; //mV
  fCoarseTStamp = 0 ;

}

DSPAlgos::DSPAlgos(unsigned long int coarseTStamp,std::vector<short> *waveForm) {
  fWaveForm = nullptr;
  fWindowSize = 3;
  fBaseline = 30; // (120ns / 4ns)
  fThreshold = 10; //mV
  fCoarseTStamp = coarseTStamp;
  fWaveForm = waveForm;

}
void DSPAlgos::Set(unsigned long int coarseTStamp,std::vector<short> *waveForm){
  fCoarseTStamp = coarseTStamp;
  fWaveForm = waveForm;

}
DSPAlgos::DSPAlgos(unsigned short windowSize) : fWindowSize(windowSize) {
	fThreshold = 10.; 
	fBaseline = 30; 
	fWaveForm = nullptr; 
}

DSPAlgos::DSPAlgos(short threshold, unsigned short preTrigger, unsigned short baseline, unsigned short shortGate,
                   unsigned short longGate, unsigned short windowSize, double fraction, short delay)
    : fThreshold(threshold), fPretrigger(preTrigger), fBaseline(baseline), fShortGate(shortGate), fLongGate(longGate),
      fWindowSize(windowSize), fFraction(fraction), fDelay(delay) {
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

void DSPAlgos::CalculateFineTStamp(std::vector<short> signal,bool ns){

int trigger_start = 0;
while (trigger_start < signal.size() && signal[trigger_start] < fThreshold) {
    trigger_start++;
}

// Find the indices of the samples surrounding the threshold crossing
int idx1 = trigger_start - 1;
int idx2 = trigger_start;
while (signal[idx1] > fThreshold && idx1 >= 0) {
    idx1--;
}
while (signal[idx2] < fThreshold && idx2 < signal.size()) {
    idx2++;
}

// Perform linear interpolation to estimate the zero-crossing point
//if(idx1 == idx2)
//	idx2 = idx1+1;
double zero_crossing = 0;
if(signal[idx1]==signal[idx2]){
zero_crossing = 0;
return;
}
if(!ns){
double t1 = idx1 + (fThreshold - signal[idx1]) / (signal[idx2] - signal[idx1]);
double t2 = idx2 - (signal[idx2] - fThreshold) / (signal[idx2] - signal[idx1]);
zero_crossing = ((t1 + t2) / 2.0)*4.;
}else{
double t1 = idx1*4. + (fThreshold - signal[idx1]) / (signal[idx2] - signal[idx1]);
double t2 = idx2*4. - (signal[idx2] - fThreshold) / (signal[idx2] - signal[idx1]);
zero_crossing = (t1 + t2) / 2.0;

}

// Compute the fine timestamp by adding the zero-crossing time to the trigger time
double fine_timestamp = zero_crossing;// + trigger_start;
//std::cout << "TriggerStart : " << trigger_start << " : ZeroCrossing : " << zero_crossing << " : FineTStamp : " << fine_timestamp << std::endl;
std::cout << "FCoarseTStamp : " << fCoarseTStamp <<" : ZeroCross : " << fine_timestamp << std::endl;

fFineTStamp = fCoarseTStamp*1000 + fine_timestamp*1000;
}

short DSPAlgos::CalculateBaseline(std::vector<short> *signal) {
int sum=0;
for(unsigned int i = 0 ; i < fBaseline ; i++){
  sum += signal->at(i);
}
return sum/fBaseline; 
}

std::vector<short> DSPAlgos::CalculateBaselineSubtractedSignal(std::vector<short> *signal){
std::vector<short> baselineSubtractedSignal;
short baselineValue = CalculateBaseline(signal);
for(unsigned int i = 0 ; i < signal->size() ; i++){
baselineSubtractedSignal.push_back(signal->at(i)-baselineValue);
}
return baselineSubtractedSignal;
}

std::vector<short> DSPAlgos::CalculateBaselineSubtractedSignal(){
return CalculateBaselineSubtractedSignal(fWaveForm);
}
std::vector<short> DSPAlgos::CalculateCFD() {
  fFraction = 0.5;
  fDelay = 4;

  std::vector<short> cfd_signal(fWaveForm->size());

  // Calculate the CFD signal as the difference between the original signal and a fraction of the delayed signal
  for (int i = fDelay; i < fWaveForm->size(); i++) {
    short delayed_signal = fWaveForm->at(i - fDelay) * fFraction;
    cfd_signal[i] = fWaveForm->at(i) - delayed_signal;
  }

  return cfd_signal;
}

std::vector<short> DSPAlgos::CalculateCFD(std::vector<short> signal) {

  fFraction = 0.5;
  fDelay = 4;
  fWaveForm->clear();
  fWaveForm->resize(signal.size());
  for (unsigned int i = 0; i < fWaveForm->size(); i++) {
    fWaveForm->at(i) = signal[i];
  }

  std::vector<short> cfd_signal(fWaveForm->size());

  // Calculate the CFD signal as the difference between the original signal and a fraction of the delayed signal
  for (int i = fDelay; i < fWaveForm->size(); i++) {
    short delayed_signal = fWaveForm->at(i - fDelay) * fFraction;
    cfd_signal[i] = fWaveForm->at(i) - delayed_signal;
  }

  return cfd_signal;
}

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
double DSPAlgos::GetCFDFraction() const { return fFraction; }
short DSPAlgos::GetCFDDelay() const { return fDelay; }
