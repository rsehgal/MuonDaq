#include "daq.h"
#include <TROOT.h>
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <ncurses.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
// ROOT related header file
#include "DataTree.h"
#include <TFile.h>
#include <TTree.h>
#include <fstream>
//#include "common.h"

unsigned int runnum = 0;
bool startDAQ = false;

//**************** Going to Fill this block from READCONF ******************
std::string File_prefix;
char Clock_mode;     //= 1; // External clock
char Trig_mode;      //= 1;  // Coincidence
bool coincidence;    //= 1;  // Coincidence
char ClkSrc;         //= 1;
bool cont_mode;      //= true;
bool saveWaveForm;   //= false
bool individualFile; //= false
int numOfEvents;
bool timeNormalization;
std::vector<bool> board;         //= {true,true,true,true};
std::vector<TFile *> fileVec;    //={NULL,NULL,NULL,NULL};
std::vector<DataTree *> treeVec; //={NULL,NULL,NULL,NULL};
// std::vector<bool> board 	;//= {false,false,true,false};
std::vector<std::string> ipVec; //= {"200.100.100.12", "200.100.100.14", "200.100.100.15", "200.100.100.17"};

std::vector<unsigned short> threshold; // = {10, 10, 10, 10, 10, 10, 10, 10};
// std::vector<unsigned short> threshold = {25, 25, 25, 25, 25, 25, 25, 25, 25, 25};
std::vector<unsigned short> portVec; // = {60102, 60104, 60105, 60107};
std::vector<std::string> boardVec;   //={"Board_2","Board_4","Board_5","Board_7"};
unsigned short timeForEachFile;
//****************************************************************************

std::atomic<bool> stop_flag(false);
std::vector<unsigned short> threshold_1 = {10, 10, 10, 10, 10, 10, 10, 10};

// Common settings
unsigned short coincWindow = 24; // ns //iMultiplier //values needs to be checked
unsigned short coincWindow_1 = coincWindow / 4;
unsigned short preTrigger = 180; // ns //iParamM //value needs to be checked
unsigned short preTrigger_1 = preTrigger / 4 + 5;
unsigned short baseline = 120; // baseline //value needs to be checked
unsigned short baseline1 = baseline / 4;
unsigned short shortgate = 160; // shortgate //value needs to be checked
unsigned short shortgate1 = shortgate / 4;
unsigned short longgate = 320; // longgate // value needs to be checked
unsigned short longgate1 = longgate / 4;

unsigned int GetRunNumber() {
  std::ifstream infile("runnum.txt");
  unsigned int runnum = 0;
  infile >> runnum;
  infile.close();

  std::ofstream outfile("runnum.txt");
  outfile << ++runnum;
  outfile.close();

  runnum--;

  return runnum;
}

int BoardStatus(short int *msg) { return (msg[5] & 0x0100); }
int ClockAvailable(short int *msg) { return (msg[5] & 0x0008); }
int StartCounter(short int *msg) { return (msg[5] & 0x0010); }
int ClockOk(short int *msg) { return (msg[5] & 0x0020); }
int TriggerA(short int *msg) {return (msg[5] & 0x0200);}
int TriggerB(short int *msg) {return (msg[5] & 0x0400);}
int CoincidenceMode(short int *msg) {return (msg[5] & 0x0800);}

void handle_signal(int signal) {
  std::cout << "Handlke_Signal called............" << std::endl;
  if (signal == SIGINT) {
    std::cout << "Signal : " << signal << std::endl;
    stop_flag.store(true);
    return;
    // return 0 ;
  }
  // return 1;
}

// void OpenFile(unsigned short boardId,unsigned int thLocalRunnum){
void OpenFile(unsigned short boardId) {
  // unsigned int runnum = GetRunNumber();
  // std::string filename=boardVec[boardId]+"_Run_"+std::to_string(runnum)+"_"+GenerateFileName();
  // fileVec[boardId] = new TFile(filename.c_str(),"RECREATE");
  // treeVec[boardId] = new DataTree(boardId,thLocalRunnum);
  treeVec[boardId] = new DataTree(boardId);
}

void CloseFile(unsigned short boardId) {
  // fileVec[boardId]->Close();
  treeVec[boardId]->Write();
  delete treeVec[boardId];
  runnum++;
  // delete fileVec[boardId];
}

int startUPDServer(int thread_num, int portnum) {
  unsigned int threadLocalRunnum = runnum;
  std::cout << "Entered UDPServer........" << std::endl;
  // stop_flag = false;
  // signal(SIGINT, handle_signal);
  // Create socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int reuseadd = 1;
  // setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuseadd,sizeof(int));
  // setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&reuseadd,sizeof(int));
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket.\n";
    return -1;
  }
  std::cout << "Reachecd checkpoint 1 ........ " << std::endl;
  struct sockaddr_in myaddr;
  memset(&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(portnum);
  // myaddr.sin_port = htons(12345);
  myaddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {

    if (errno == EADDRINUSE) {
      std::cerr << "ERROR INSIDE IF : " << errno << " : EADDRINUSE : " << EADDRINUSE << std::endl;
      close(sockfd);
      return -1;
    } else {
      std::cerr << "ERROR : " << errno << std::endl;
    }
    // while (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
    std::cerr << "Failed to bind socket.\n";
    // return -1;
  }

  std::cout << "Reachecd checkpoint 2 ........ " << std::endl;
  short int msg[740];
  // short int *msg;
  struct sockaddr_in srcaddr;
  socklen_t addrlen = sizeof(srcaddr);

  ssize_t numbytes = 0;
  unsigned short boardId = DetectBoardIdFromPort(portnum);
  std::string filename = boardVec[boardId] + "_" + GenerateFileName();
  std::string ipAdd = ipVec[boardId];

  std::cout << "Reachecd checkpoint 3 ........ " << std::endl;
  std::cout << "Going to create a new file : " << filename << std::endl;
  // TFile *fp = new TFile(filename.c_str(),"RECREATE");
  // OpenFile(boardId,threadLocalRunnum);
  OpenFile(boardId);

  unsigned long int prevTStamp = 0;
  unsigned long int prevEventCounter = 0;

  std::cout << "Reachecd checkpoint 4 ........ now entering loop " << std::endl;
  // Receive packets
  // while (true) {
  unsigned long int eventCounter = 0;
  bool fileClosingCond = false;

  while (!stop_flag.load()) {
    if (eventCounter > 1) {
      // if ((treeVec[boardId]->currentTStamp - prevTStamp) > 60) {
      if (timeNormalization) {
        fileClosingCond = (treeVec[boardId]->currentTStamp - prevTStamp) > (timeForEachFile * 60);
      } else {
        fileClosingCond = ((eventCounter - prevEventCounter) > numOfEvents);
      }
      if (fileClosingCond) {
        std::cout << "Eventcounter : " << eventCounter << " : CurrentTStamp : " << treeVec[boardId]->currentTStamp
                  << " : prevTStamp : " << prevTStamp << std::endl;
        prevTStamp = treeVec[boardId]->currentTStamp;
        prevEventCounter = eventCounter;
        if (!cont_mode) {
          stop_flag.store(true);
          continue;
        }

        CloseFile(boardId);
        threadLocalRunnum++;
        // OpenFile(boardId,threadLocalRunnum);
        OpenFile(boardId);
      }
    }
    // signal(SIGINT, handle_signal);
    char buf[1500];
    short int msg[740];
    numbytes = recvfrom(sockfd, msg, 1480, 0, (struct sockaddr *)&srcaddr, &addrlen);

      if (numbytes > 1) {
      treeVec[boardId]->Reset();
      eventCounter++;
      unsigned long int Coarse_time_stamp_Near_1 = ((msg[18] << 16) & 0xffff0000) + (msg[17] & 0xffff);
      Coarse_time_stamp_Near_1 = Coarse_time_stamp_Near_1 & 0x00000000ffffffff;

      unsigned long int Coarse_time_stamp_Near = ((msg[19] & 0xffff));
      Coarse_time_stamp_Near = Coarse_time_stamp_Near & 0x0000ffffffff;
      Coarse_time_stamp_Near = (Coarse_time_stamp_Near << 32) + Coarse_time_stamp_Near_1;

      unsigned long int Coarse_time_stamp_Far_1 = ((msg[22] << 16) & 0xffff0000) + (msg[21] & 0xffff);
      Coarse_time_stamp_Far_1 = Coarse_time_stamp_Far_1 & 0x00000000ffffffff;

      unsigned long int Coarse_time_stamp_Far = ((msg[23] & 0xffff));
      Coarse_time_stamp_Far = Coarse_time_stamp_Far & 0x0000ffffffff;
      Coarse_time_stamp_Far = (Coarse_time_stamp_Far << 32) + Coarse_time_stamp_Far_1;

      // unsigned long int fineTStampNear = 0;
      // unsigned long int fineTStampFar = 0;
      treeVec[boardId]->fBoardId = boardId;
      treeVec[boardId]->fBoardName = boardVec[boardId];
      treeVec[boardId]->fineTStampNear = (msg[20] & 0xffff);
      treeVec[boardId]->fineTStampFar = (msg[24] & 0xffff);
      treeVec[boardId]->fineTStampNear =
          (Coarse_time_stamp_Near - (1. * treeVec[boardId]->fineTStampNear / 65536.)) * 4;
      treeVec[boardId]->fineTStampFar = (Coarse_time_stamp_Far - (1. * treeVec[boardId]->fineTStampFar / 65536.)) * 4;
      treeVec[boardId]->delT = treeVec[boardId]->fineTStampFar - treeVec[boardId]->fineTStampNear;
      treeVec[boardId]->longGateA = (((msg[431] << 16) & 0xffff0000) + (msg[430] & 0xffff));
      treeVec[boardId]->longGateB = (((msg[441] << 16) & 0xffff0000) + (msg[440] & 0xffff));
      treeVec[boardId]->longGateA = std::abs(treeVec[boardId]->longGateA);
      treeVec[boardId]->longGateB = std::abs(treeVec[boardId]->longGateB);
      treeVec[boardId]->qMean = std::sqrt(treeVec[boardId]->longGateA * treeVec[boardId]->longGateB);
      treeVec[boardId]->currentTStamp = GetCurrentTimeStamp();
      // treeVec[boardId]->tree->Fill();

      if (coincidence) {
        if (saveWaveForm) {
          for (unsigned short w = 25; w < 226; w++) {
            treeVec[boardId]->push_back(msg[w], true);
          }
          for (unsigned short w = 227; w < 428; w++) {
            treeVec[boardId]->push_back(msg[w], false);
          }
        }
        treeVec[boardId]->fCoarseTStampNear = Coarse_time_stamp_Near * 4;
        treeVec[boardId]->fCoarseTStampFar = Coarse_time_stamp_Far * 4;

        treeVec[boardId]->Fill();
      } else {

        if(TriggerA(msg)){
        treeVec[boardId]->fChannelNo = 2 * boardId;
        treeVec[boardId]->fineTStamp = treeVec[boardId]->fineTStampNear;
        treeVec[boardId]->longGate = treeVec[boardId]->longGateA;
        for (unsigned short w = 25; w < 226; w++) {
          treeVec[boardId]->push_back(msg[w]);
        }
        treeVec[boardId]->fCoarseTStamp = Coarse_time_stamp_Near * 4;
        treeVec[boardId]->Fill();
        }

        //treeVec[boardId]->clear();
      if(TriggerB(msg)){
        treeVec[boardId]->fChannelNo = 2 * boardId + 1;
        treeVec[boardId]->fineTStamp = treeVec[boardId]->fineTStampFar;
        treeVec[boardId]->longGate = treeVec[boardId]->longGateB;
        for (unsigned short w = 227; w < 428; w++) {
          treeVec[boardId]->push_back(msg[w]);
        }
        treeVec[boardId]->fCoarseTStamp = Coarse_time_stamp_Far * 4;
        treeVec[boardId]->Fill();
      }
      }

      // std::cout << "IP : " << ipAdd << " :: fineTStampNear : " << fineTStampNear <<" : fineTStampFar : " <<
      // fineTStampFar << std::endl;
      buf[numbytes] = '\0';

      if (eventCounter == 1) {
        prevTStamp = treeVec[boardId]->currentTStamp;
      }
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  StopDAQ(ipAdd.c_str());
  // fp->cd();
  // tree->SetDirectory(fp);

  /*tree->SetDirectory(fileVec[boardId]);
  tree->Write();
  */

  // fp->Close();
  CloseFile(boardId);
  close(sockfd);
  std::cout << "Stop_Flag : " << stop_flag.load() << " : Returning from thread : " << ipVec[boardId] << std::endl;
  return 0;
}

void thread_func(int thread_num, int portnum) {
  std::cout << "Thread " << thread_num << " started.\n";
  startUPDServer(thread_num, portnum);
  std::cout << "Thread " << thread_num << " finished.\n";
}

// int main(int argc, char *argv[]) {
int ConnectBoard(const char *ip) {
  // Create socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  // fcntl(sockfd,F_SETFL,O_NONBLOCK);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket.\n";
    return -1;
  }

  // Set destination address
  struct sockaddr_in destaddr;
  memset(&destaddr, 0, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  destaddr.sin_port = htons(60105);
  inet_pton(AF_INET, ip, &destaddr.sin_addr);

  char data1[500];
  data1[0] = 0xfe;
  data1[1] = 0xfe;
  data1[2] = (0x10 | (ClkSrc << 1));
  std::cout << "ClkSrc : " << (int)ClkSrc << std::endl;
  std::cout << "ClkSrc << 1 : " << (ClkSrc << 1) << std::endl;

  std::cout << "Checking clock : " << (int)data1[2] << std::endl;
  data1[3] = 0x00;
  for (unsigned short i = 4; i < 500; i++) {
    data1[i] = 0x00;
  }
  size_t datalen = std::strlen(data1);
  std::cout << "Data len : " << datalen << std::endl;

  // if (sendto(sockfd, data1, datalen, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
  if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
    std::cerr << "Failed to send packet.\n";
    return -1;
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  data1[2] = 0x00 | (ClkSrc << 1);
  if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
    std::cerr << "Failed to send packet.\n";
    return -1;
  }

  // Close socket
  close(sockfd);

  return 0;
}
/*
 * This function will connect all the
 * enabled boards
 */
void Connect() {
  for (unsigned short i = 0; i < board.size(); i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (board[i]) {
      std::cout << "Sending connection packet to : " << ipVec[i] << std::endl;
      ConnectBoard(ipVec[i].c_str());
    }
  }
}
/*
 * This function will set all the flags required to
 * tell the computer that data acquistion is started.
 * This includes : Opening file for writing,
 *                 Generating required file names etc.
 *
 * In addition to this it will also send a packet to
 * to all the boards, tell them to start sending data
 * packets.
 */

int SetDAQ() {
  std::cout << "------------ SET DAQ ------------------" << std::endl;
  //------------------------------------------------

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  // fcntl(sockfd,F_SETFL,O_NONBLOCK);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket.\n";
    return -1;
  }

  // Set destination address
  struct sockaddr_in destaddr;
  memset(&destaddr, 0, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  destaddr.sin_port = htons(60105);
  // destaddr.sin_port = htons(12345);

  char data1[500];
  data1[0] = 0xfe;
  data1[1] = 0xfe;
  data1[2] = 0x01 | (Trig_mode << 2) | (Clock_mode << 1);
  data1[3] = 0x01;
  data1[4] = 0xab;
  data1[5] = 0xab;
  data1[6] = (preTrigger_1 & 0xFF);
  data1[7] = ((preTrigger_1 >> 8) & 0xFF);
  data1[10] = (coincWindow_1 & 0xFF);
  data1[11] = ((coincWindow_1 >> 8) & 0xFF);
  data1[12] = 0x14;

  for (unsigned short i = 13; i < 24; i++) {
    data1[i] = 0x00; // i;
  }
  for (unsigned short i = 24; i < 500; i++) {
    data1[i] = 0x00;
  }

  std::string ip;
  for (unsigned short i = 0; i < board.size(); i++) {
    if (board[i]) {

      ip = ipVec[i];
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      threshold_1[2 * i] = threshold[2 * i] * 32;
      threshold_1[2 * i + 1] = threshold[2 * i + 1] * 32;
      data1[8] = (threshold_1[2 * i] & 0xFF);
      data1[9] = ((threshold_1[2 * i] >> 8) & 0xFF);
      data1[24] = (threshold_1[2 * i + 1] & 0xFF);
      data1[25] = ((threshold_1[2 * i + 1] >> 8) & 0xFF);

      inet_pton(AF_INET, ipVec[i].c_str(), &destaddr.sin_addr);
      if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        std::cerr << "Failed to send packet.\n";
        return -1;
      }
      std::cout << "Starting DAQ from board with IP : " << ip << std::endl;
    }
  }
  close(sockfd);
  // std::cout <<"REturning from SetDAQ....."<< std::endl;
}
/*
 * This function will send a packet with DAQ parameters
 */
int SetParameters() {
  std::cout << "------------ SETTING PARAMETER ------------------" << std::endl;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  // fcntl(sockfd,F_SETFL,O_NONBLOCK);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket.\n";
    return -1;
  }

  // Set destination address
  struct sockaddr_in destaddr;
  memset(&destaddr, 0, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  destaddr.sin_port = htons(60105);

  char data1[500];
  data1[0] = 0xfe;
  data1[1] = 0xfe;
  data1[2] = 0x08 | (Trig_mode << 2) | (Clock_mode << 1);
  data1[3] = 0x01;
  data1[4] = 0xab;
  data1[5] = 0xab;
  data1[6] = (preTrigger_1 & 0xFF);
  data1[7] = ((preTrigger_1 >> 8) & 0xFF);
  data1[10] = (coincWindow_1 & 0xFF);
  data1[11] = ((coincWindow_1 >> 8) & 0xFF);
  data1[12] = (coincWindow_1 & 0xFF);
  data1[13] = ((coincWindow_1 >> 8) & 0xFF);
  data1[14] = (coincWindow_1 & 0xFF);
  data1[15] = ((coincWindow_1 >> 8) & 0xFF);
  data1[16] = (baseline1 & 0xFF);
  data1[17] = ((baseline1 >> 8) & 0xFF);
  data1[18] = (shortgate1 & 0xFF);
  data1[19] = ((shortgate1 >> 8) & 0xFF);
  data1[20] = (longgate1 & 0xFF);
  data1[21] = ((longgate1 >> 8) & 0xFF);
  data1[22] = (longgate1 & 0xFF);
  data1[23] = ((longgate1 >> 8) & 0xFF);

  int T1, T2, T3, T4;
  T1 = preTrigger_1;
  T2 = baseline1;
  T3 = shortgate1;
  T4 = longgate1;

  for (unsigned int i = 26; i < 500; i++) {
    data1[i] = i;
  }

  if (((T1 - T2) < 15) && (T1 > 0)) {
    std::cout << "range exceed errror : Baseline related issue" << std::endl;
    return -1;
    // break;
  }

  if ((T3 >= T4)) // || (T3==T4))
  {
    std::cout << "Range exceed error : Long gate related issue" << std::endl;
    return -1;
    // break;
  }

  if ((T2 + T4) > 200) {
    std::cout << "Range exceed error : baseline + longgate related issue " << std::endl;
    return -1;
    // break;
  }

  float fgain = 0.75;
  for (unsigned short i = 0; i < board.size(); i++) {
    if (board[i]) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      threshold_1[2 * i] = threshold[2 * i] * 32 * fgain;
      threshold_1[2 * i + 1] = threshold[2 * i + 1] * 32 * fgain;
      data1[8] = (threshold_1[2 * i] & 0xFF);
      data1[9] = ((threshold_1[2 * i] >> 8) & 0xFF);
      data1[24] = (threshold_1[2 * i + 1] & 0xFF);
      data1[25] = ((threshold_1[2 * i + 1] >> 8) & 0xFF);

      inet_pton(AF_INET, ipVec[i].c_str(), &destaddr.sin_addr);
      if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        std::cerr << "Failed to send packet.\n";
        return -1;
      }
    }
  }

  close(sockfd);
}

unsigned short DetectBoardId(const char *ip) {
  auto it = std::find(ipVec.begin(), ipVec.end(), std::string(ip));
  if (it != ipVec.end()) {
    return std::distance(ipVec.begin(), it);
  }
}
unsigned short DetectBoardIdFromPort(unsigned short portNum) {
  auto it = std::find(portVec.begin(), portVec.end(), portNum);
  if (it != portVec.end()) {
    return std::distance(portVec.begin(), it);
  }
}

int StopDAQ(const char *ip) {
  std::cout << "------------ STOP DAQ ------------------" << std::endl;
  //------------------------------------------------
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  // fcntl(sockfd,F_SETFL,O_NONBLOCK);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket.\n";
    return -1;
  }

  // Set destination address
  struct sockaddr_in destaddr;
  memset(&destaddr, 0, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  destaddr.sin_port = htons(60105);
  // destaddr.sin_port = htons(12345);

  char data1[500];
  data1[0] = 0xfe;
  data1[1] = 0xfe;
  data1[2] = 0x00 | (Clock_mode << 1);
  data1[3] = 0x01;
  data1[4] = 0xab;
  data1[5] = 0xab;
  data1[6] = (preTrigger_1 & 0xFF);
  data1[7] = ((preTrigger_1 >> 8) & 0xFF);
  data1[10] = (coincWindow_1 & 0xFF);
  data1[11] = ((coincWindow_1 >> 8) & 0xFF);

  for (unsigned short i = 12; i < 500; i++) {
    data1[i] = 0x00; // i
  }

  // for(unsigned short i = 0 ; i < board.size() ; i++)
  unsigned short boardId = DetectBoardId(ip);
  std::cout << "StopDAQ called for IP : " << ip << " : BoardID : " << boardId << std::endl;
  {
    if (board[boardId]) {
      threshold_1[2 * boardId] = threshold[2 * boardId] * 32;
      threshold_1[2 * boardId + 1] = threshold[2 * boardId + 1] * 32;
      data1[8] = (threshold_1[2 * boardId] & 0xFF);
      data1[9] = ((threshold_1[2 * boardId] >> 8) & 0xFF);

      inet_pton(AF_INET, ip, &destaddr.sin_addr);
      if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        std::cerr << "Failed to send packet.\n";
        return -1;
      }
    }
  }
  close(sockfd);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

unsigned long int GetCurrentTimeStamp() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(now);
}

std::string GenerateFileName() {
  auto now = std::chrono::system_clock::now();

  // Convert time to local time
  std::time_t current_time = std::chrono::system_clock::to_time_t(now);
  std::tm *local_time = std::localtime(&current_time);

  // Format filename string
  std::ostringstream filename;
  filename << std::put_time(local_time, "ED_DIGI_%d_%m_%Y_%Hhr_%Mmin_%Ssec") << ".root";
  std::string finalFileName = File_prefix + filename.str();
  return finalFileName;
  // return filename.str();
}
/*int main() {
  Connect();
  SetParameters();
  SetDAQ();
  return 0;
}
*/
