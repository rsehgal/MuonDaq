#include "daq.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <ncurses.h>
#include <csignal>
#include <algorithm>
#include <atomic>
#include <fcntl.h>

//ROOT related header file
#include <TFile.h>
#include <TTree.h>

bool startDAQ = false;
char Clock_mode = 0; // External clock
char Trig_mode = 1;  // Coincidence
char ClkSrc = 0;
std::atomic<bool> stop_flag(false);

//std::atomic<bool>
//bool stop_flag = false;
// Enabled boards
std::vector<bool> board = {false,false,true,true, false};
std::vector<std::string> ipVec = {"200.100.100.12", "200.100.100.14", "200.100.100.15", "200.100.100.17",
                                  "200.100.100.7"};

std::vector<unsigned short> threshold = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
std::vector<unsigned short> threshold_1 = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
std::vector<unsigned short> portVec = {60102, 60104, 60105, 60107, 60109};
std::vector<std::string> boardVec={"Board_2","Board_4","Board_5","Board_7","Board_9"};
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

int BoardStatus(short int *msg) { return (msg[5] & 0x0100); }
int ClockAvailable(short int *msg) { return (msg[5] & 0x0008); }
int StartCounter(short int *msg) { return (msg[5] & 0x0010); }
int ClockOk(short int *msg) { return (msg[5] & 0x0020); }
void handle_signal(int signal) {
  if (signal == SIGINT) {
    stop_flag.store(true);
  }
}

int startUPDServer(int thread_num, int portnum) {
  std::cout << "Entered UDPServer........"<<std::endl;
  stop_flag = false;
  //signal(SIGINT, handle_signal);
  // Create socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int reuseadd = 1;
  //setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuseadd,sizeof(int));
  //setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&reuseadd,sizeof(int));
  fcntl(sockfd,F_SETFL,O_NONBLOCK);
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

	  if(errno == EADDRINUSE){
		  std::cerr << "ERROR INSIDE IF : " << errno << " : EADDRINUSE : " << EADDRINUSE<< std::endl;
	  close(sockfd);
	  return -1;
	  }else{
		  std::cerr << "ERROR : " << errno << std::endl;
	  }
  //while (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
    std::cerr << "Failed to bind socket.\n";
    //return -1;
  }

  std::cout << "Reachecd checkpoint 2 ........ " << std::endl;
  short int msg[150];
  //short int *msg;
  struct sockaddr_in srcaddr;
  socklen_t addrlen = sizeof(srcaddr);

  ssize_t numbytes = 0;
  while(numbytes <=0 )
  {
  numbytes = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&srcaddr, &addrlen);
  std::cout << "Number of bytes received : " << numbytes << std::endl;
  }
  //numbytes = recvfrom(sockfd, NULL, 0, 0, (struct sockaddr *)&srcaddr, &addrlen);
  std::cout <<"Size of msg : " << numbytes << std::endl;

  std::string ipAdd = std::string(inet_ntoa(srcaddr.sin_addr));
  std::cout <<"Fetched IP addresss : " << ipAdd << std::endl;
  unsigned short boardId = DetectBoardId(ipAdd.c_str());
  std::string filename=boardVec[boardId]+".root";

  std::cout << "Reachecd checkpoint 3 ........ " << std::endl;
  std::cout << "Going to create a new file : "<< filename << std::endl;
  TFile *fp = new TFile(filename.c_str(),"RECREATE");

  unsigned long int fineTStampNear = 0;
  unsigned long int fineTStampFar = 0;
  long int delT;

  TTree *tree = new TTree("ftree", "ftree");
  tree->Branch("fTNear", &fineTStampNear);
  tree->Branch("fTFar", &fineTStampFar);
  tree->Branch("fDelT", &delT);
  /*tree->Branch("fDelT_WithoutConversion", &delT_WithoutConversion);
  tree->Branch("fQNear", &longGateA);
  tree->Branch("fQFar", &longGateB);
  tree->Branch("fQMean", &qMean);*/

  std::cout << "Reachecd checkpoint 4 ........ now entering loop " << std::endl;
  // Receive packets
  //while (true) {
  while (!stop_flag.load()) {
    //signal(SIGINT, handle_signal);
    char buf[1500];
     //short int *msg=NULL;
    short int msg[740];
    //struct sockaddr_in srcaddr;
    //socklen_t addrlen = sizeof(srcaddr);

    //ssize_t numbytes = 0;
    numbytes = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&srcaddr, &addrlen);

    if (numbytes == -1) {
	continue;
    }
    /*
    else{
	 //   msg = new short int[numbytes];
    }
    */
    /*if (numbytes == -1) {
      std::cerr << "Failed to receive packet.\n";
      return -1;
    }*/


    unsigned long int Coarse_time_stamp_Near_1=  ((msg[18]<< 16)& 0xffff0000)+ (msg[17]& 0xffff)  ;
    Coarse_time_stamp_Near_1 =Coarse_time_stamp_Near_1 & 0x00000000ffffffff ;

    unsigned long int Coarse_time_stamp_Near=  ((msg[19]& 0xffff));
    Coarse_time_stamp_Near =Coarse_time_stamp_Near & 0x0000ffffffff ;
    Coarse_time_stamp_Near= ( Coarse_time_stamp_Near<< 32)+Coarse_time_stamp_Near_1 ;

    unsigned long int Coarse_time_stamp_Far_1=  ((msg[22]<< 16)& 0xffff0000)+ (msg[21]& 0xffff)  ;
    Coarse_time_stamp_Far_1 =Coarse_time_stamp_Far_1 & 0x00000000ffffffff ;

    unsigned long int Coarse_time_stamp_Far=  ((msg[23]& 0xffff));
    Coarse_time_stamp_Far =Coarse_time_stamp_Far & 0x0000ffffffff ;
    Coarse_time_stamp_Far= ( Coarse_time_stamp_Far<< 32)+Coarse_time_stamp_Far_1 ;


    //unsigned long int fineTStampNear = 0;
    //unsigned long int fineTStampFar = 0;
    fineTStampNear = (msg[20] & 0xffff);
    fineTStampFar = (msg[24] & 0xffff);

    fineTStampNear = (Coarse_time_stamp_Near-(1.*fineTStampNear/65536.))*4;
    fineTStampFar = (Coarse_time_stamp_Far-(1.*fineTStampFar/65536.))*4;
    delT = fineTStampFar - fineTStampNear;
    tree->Fill();

    //std::cout << "IP : " << ipAdd << " :: fineTStampNear : " << fineTStampNear <<" : fineTStampFar : " << fineTStampFar << std::endl;
    buf[numbytes] = '\0';
    //std::string command = "./stopDaq " + std::string(inet_ntoa(srcaddr.sin_addr));
    /*if (stop_flag) {
      //system(command.c_str());
      StopDAQ(ipAdd.c_str());
      break;
    }*/
  }

  
  StopDAQ(ipAdd.c_str());
  fp->cd();
  //tree->Write();
  fp->Close();
  close(sockfd);
  std::cout <<"Returning from thread : " << ipAdd << std::endl;
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
  //fcntl(sockfd,F_SETFL,O_NONBLOCK);
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
    data1[i] = 0;
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
    if (board[i]) {
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
  //fcntl(sockfd,F_SETFL,O_NONBLOCK);
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
    data1[i] = i;
  }
  for (unsigned short i = 24; i < 500; i++) {
    data1[i] = i; // 0x00;
  }

  for (unsigned short i = 0; i < board.size(); i++) {
    if (board[i]) {
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
    }
  }
  std::cout <<"REturning from SetDAQ....."<< std::endl;
}
/*
 * This function will send a packet with DAQ parameters
 */
int SetParameters() {
  std::cout << "------------ SETTING PARAMETER ------------------" << std::endl;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  //fcntl(sockfd,F_SETFL,O_NONBLOCK);
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

unsigned short DetectBoardId(const char *ip){
        auto it = std::find(ipVec.begin(),ipVec.end(),std::string(ip));
          if(it != ipVec.end()){
            return std::distance(ipVec.begin(),it);
          }
}

int StopDAQ(const char *ip){
        std::cout <<"------------ STOP DAQ ------------------"<< std::endl;
//------------------------------------------------
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  //fcntl(sockfd,F_SETFL,O_NONBLOCK);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    // Set destination address
    struct sockaddr_in destaddr;
    memset(&destaddr, 0, sizeof(destaddr));
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(60105);
    //destaddr.sin_port = htons(12345);


char data1[500];
data1[0]=0xfe;
data1[1]=0xfe;
data1[2]=0x00 | (Clock_mode << 1);
data1[3]=0x01;
data1[4]=0xab;
data1[5]=0xab;
data1[6]=(preTrigger_1 & 0xFF);
data1[7]=((preTrigger_1 >> 8) & 0xFF);
data1[10]=(coincWindow_1 & 0xFF);
data1[11]=((coincWindow_1 >> 8) & 0xFF);

for(unsigned short i = 12 ; i < 500 ; i++){
data1[i]=i;//0x00;
}

//for(unsigned short i = 0 ; i < board.size() ; i++)
unsigned short boardId = DetectBoardId(ip);
{
        if(board[boardId]){
                threshold_1[2*boardId]=threshold[2*boardId]*32;
                threshold_1[2*boardId+1]=threshold[2*boardId+1]*32;
                data1[8]=(threshold_1[2*boardId] & 0xFF);
                data1[9]=((threshold_1[2*boardId] >> 8) & 0xFF);

                inet_pton(AF_INET, ip , &destaddr.sin_addr);
                if (sendto(sockfd, data1, 500, 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        std::cerr << "Failed to send packet.\n";
        return -1;
    }


        }
}
}

/*int main() {
  Connect();
  SetParameters();
  SetDAQ();
  return 0;
}
*/
