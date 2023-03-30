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
  std::vector<std::thread> threadVec;
  for (unsigned short i = 0; i < board.size(); i++) {
    if (board[i]) {
      threadVec.push_back(std::thread(thread_func, i, portVec[i]));
    }
  }

  Connect();
  SetParameters();
  SetDAQ();
  signal(SIGINT, handle_signal); 
  //Waiting for threads to finish their work 
  for (unsigned short i = 0; i < threadVec.size(); i++) {
    threadVec[i].join();
  }
  //Giving final stop message
  while (true) {
    if (stop_flag) {
      std::cout << "Found Stop flag set ......." << std::endl;
      break;
    }
  }

  return 0;
}
