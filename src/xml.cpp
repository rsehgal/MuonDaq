#include "daq.h"
#include "tinyxml2.h"
#include <iostream>
using namespace tinyxml2;

void ReadConf() {
  XMLDocument doc;
  doc.LoadFile("daq.xml");
  // doc.LoadFile("example.xml");
  if (doc.Error()) {
    std::cerr << "Error loading XML file" << std::endl;
    return;
  }
  // std::cout <<"Reached checkpoint 1 .... " << std::endl;
  // XMLNode* root = doc.FirstChild();
  XMLElement *root = doc.FirstChildElement("root");
  if (root == nullptr) {
    std::cerr << "Error: empty document" << std::endl;
    return;
  }

  // std::cout <<"Reached checkpoint 2 .... " << std::endl;

  if (root->NoChildren()) {
    std::cerr << "Error : root element has no childeren..." << std::endl;
  }

  XMLNode *testNode = root->FirstChild();
  for (XMLNode *node = root->FirstChild(); node != nullptr; node = node->NextSibling()) {
    if (node->ToComment() != nullptr) {
      continue;
    }
    int th1 = 0;
    int th2 = 0;
    int enable = 0;
    int coinc = 0;
    int extclock = 0;
    int Cont_Mode = 0;
    int saveWave = 0;
    std::string prefix="";
    unsigned short timeForFile = 0;
    std::cout << "Node name: " << node->Value() << std::endl;
    if (std::string(node->Value()) == "common") {
      const XMLAttribute *attr = node->ToElement()->FirstAttribute();
      if (attr != nullptr) {
        while (attr != nullptr) {
          if (std::string(attr->Name()) == "coinc") {
            coinc = std::stoi(attr->Value());
          }
          if (std::string(attr->Name()) == "extclock") {
            extclock = std::stoi(attr->Value());
          }
          if (std::string(attr->Name()) == "cont_mode") {
            Cont_Mode = std::stoi(attr->Value());
          }
          if (std::string(attr->Name()) == "time") {
            timeForFile = std::stoi(attr->Value());
          }
          if (std::string(attr->Name()) == "saveWaveForm") {
            saveWave = std::stoi(attr->Value());
          }
	  if (std::string(attr->Name()) == "prefix") {
            prefix = std::string(attr->Value());
          }


          attr = attr->Next();
        }
      }
      std::cout << "==================== Common Setting =========================" << std::endl;
      std::cout << "Coincidence Mode : " << coinc << " : ExtClock : " << extclock << " : Cont_Mode : " << Cont_Mode
                << std::endl;
      std::cout << "=============================================================" << std::endl;

      Clock_mode = extclock;
      ClkSrc = extclock;
      Trig_mode = coinc;
      File_prefix = prefix;
      if (Cont_Mode == 1) {
        std::cout << "CONT MODE SET TO TRUE...." << std::endl;
        cont_mode = true;
      } else {
        std::cout << "CONT MODE SET TO FALSE...." << std::endl;
        cont_mode = false;
      }
      timeForEachFile = timeForFile;
      if (saveWave == 1) {
        saveWaveForm = true;
      } else {
        saveWaveForm = false;
      }
    }

    if (std::string(node->Value()) == "board") {
      std::cout << "=============================================================" << std::endl;
      treeVec.push_back(NULL);
      const XMLAttribute *attr = node->ToElement()->FirstAttribute();
      if (attr != nullptr) {
        while (attr != nullptr) {

          if (std::string(attr->Name()) == "th1") {
            th1 = std::stoi(attr->Value());
            threshold.push_back(th1);
          }
          if (std::string(attr->Name()) == "th2") {
            th2 = std::stoi(attr->Value());
            threshold.push_back(th2);
          }
          if (std::string(attr->Name()) == "enable") {
            enable = std::stoi(attr->Value());
            if (enable == 1) {
              board.push_back(true);
            } else {
              board.push_back(false);
            }
          }
          if (std::string(attr->Name()) == "ip") {
            ipVec.push_back(std::string(attr->Value()));
          }
          if (std::string(attr->Name()) == "name") {
            boardVec.push_back(std::string(attr->Value()));
          }
          if (std::string(attr->Name()) == "port") {
            portVec.push_back(std::stoi(attr->Value()));
          }

          /*
          if(std::string(attr->Name())=="th1" || std::string(attr->Name())=="th2" )
          std::cout << "\t" << attr->Name() << " : " << std::stoi(attr->Value()) << std::endl;
          else
          std::cout << "\t" << attr->Name() << " : " << attr->Value() << std::endl;
          */
          std::cout << "\t" << attr->Name() << " : " << attr->Value() << std::endl;
          attr = attr->Next();
        }

        std::cout << "\t"
                  << "TH1 : " << th1 << " : TH2 : " << th2 << " : Enable : " << enable << std::endl;
      }
    }
    // std::cout << "Node type: " << node->Type() << std::endl;
  }

  std::cout << "=============================================================" << std::endl;

  return;
}
