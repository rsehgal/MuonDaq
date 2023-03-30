/*
**	Filename : daq.h
**	2023-03-28
**	username : rsehgal
*/
#ifndef daq_h
#define daq_h

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <atomic>
extern bool startDAQ;   //= false;
extern char Clock_mode; //= 0; // External clock
extern char Trig_mode;  //= 0;  // Coincidence
extern char ClkSrc;     //= 0;
extern std::atomic<bool> stop_flag;
//extern bool stop_flag;
extern std::vector<bool> board;// = {false, false, true, true, false};
extern std::vector<std::string> ipVec;// = {"200.100.100.12", "200.100.100.14", "200.100.100.15", "200.100.100.17", "200.100.100.7"}

extern std::vector<unsigned short> threshold;                           //{10,10,10,10,10,20,30,40,10,10};
extern std::vector<unsigned short> threshold_1; //{10,10,10,10,10,10,10,10,10,10};
extern std::vector<unsigned short> portVec;        //{60102,60104,60105,60107,60109};

// Common settings
extern unsigned short coincWindow;   // 24 ; //ns //iMultiplier //values needs to be checked
extern unsigned short coincWindow_1; // coincWindow/4;
extern unsigned short preTrigger;    // 180 ; //ns //iParamM //value needs to be checked
extern unsigned short preTrigger_1;  // preTrigger/4 + 5;
extern unsigned short baseline;      // 120;// baseline //value needs to be checked
extern unsigned short baseline1;     // baseline/4;
extern unsigned short shortgate;     // 160;// shortgate //value needs to be checked
extern unsigned short shortgate1;    // shortgate/4;
extern unsigned short longgate;      // 320;// longgate // value needs to be checked
extern unsigned short longgate1;     // longgate/4;

extern int ConnectBoard(const char *ip);
extern void Connect();
extern int SetDAQ();
extern int SetParameters();
extern int BoardStatus(short int *msg);
extern int ClockAvailable(short int *msg);
extern int StartCounter(short int *msg);
extern int ClockOk(short int *msg);

extern void thread_func(int thread_num, int portnum);
extern int startUPDServer(int thread_num,int portnum);
extern void handle_signal(int signal);
extern int BoardStatus( short int *msg);
extern int ClockAvailable( short int *msg);
extern int StartCounter( short int *msg);
extern int ClockOk( short int *msg);
extern unsigned short DetectBoardId(const char *ip);
extern int StopDAQ(const char *ip);



#endif
