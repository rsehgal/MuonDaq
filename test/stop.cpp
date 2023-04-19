/*
**	Filename : main.cpp
**	2023-03-28
**	username : rsehgal
*/
#include "daq.h"
#include <iostream>
#include <vector>
#include <csignal>
int main(int argc, char *argv[]) {
  ReadConf();
  StopDAQ(argv[1]);
  return 0;
}
