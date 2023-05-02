/*
**	Filename : DSPAlgos.cpp
**	2023-05-02
**	username : muon
*/
#include "DSPAlgos.h"
#include <iostream>

DSPAlgos::DSPAlgos() {}

DSPAlgos::DSPAlgos(short threshold, unsigned short preTrigger, unsigned short baseline, unsigned short shortGate,
                   unsigned short longGate)
    : fThreshold(threshold), fPretrigger(preTrigger), fBaseline(baseline), fShortGate(shortGate), fLongGate(longGate) {}

unsigned long int DSPAlgos::GetCoarseTStamp() const { return fCoarseTStamp; }
unsigned long int DSPAlgos::GetFineTStamp() const { return fFineTStamp; }
unsigned short DSPAlgos::GetPreTrigger() const { return fPretrigger; }
unsigned short DSPAlgos::GetBaseline() const { return fBaseline; }
unsigned short DSPAlgos::GetShortGate() const { return fShortGate; }
unsigned short DSPAlgos::GetLongGate() const { return fLongGate; }
short DSPAlgos::GetThreshold() const { return fThreshold; }
