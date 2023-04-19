/*
**	Filename : main.cpp
**	2023-03-28
**	username : rsehgal
*/
#include "daq.h"
//#include "common.h"
#include <iostream>
#include <vector>
#include <csignal>
#include <TROOT.h>
int main(int argc, char *argv[]) {
	ROOT::EnableThreadSafety();
  std::vector<std::thread> threadVec;
  runnum = GetRunNumber();
  for (unsigned short i = 0; i < board.size(); i++) {
    if (board[i]) {
      threadVec.push_back(std::thread(thread_func, i, portVec[i]));
    }
  }

  Connect();
  SetParameters();
  SetDAQ();
  //signal(SIGINT, handle_signal); 
  /*while(stop_flag.load()){
  	std::cout << "Stop flag set to : " << stop_flag.load() << std::endl;
	std::cout <<"All the thread should now report back to main program " << std::endl;
  }*/

  //while(true){
  while(cont_mode){
  int a = 1;
  std::cin >> a;
  if(a==0){
  	stop_flag.store(true);
	break;
  }
  }
  //Waiting for threads to finish their work 
  for (unsigned short i = 0; i < threadVec.size(); i++) {
    threadVec[i].join();
  }

  /*
  for (unsigned short i = 0; i < board.size(); i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (board[i]) {
      StopDAQ(ipVec[i].c_str());
    }
  }
  */



  //Giving final stop message
  /*while (true) {
    if (stop_flag) {
      std::cout << "Found Stop flag set ......." << std::endl;
      break;
    }
  }*/

  return 0;
}
