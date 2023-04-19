#include "daq.h"
#include <iostream>
int main() {
  ReadConf();
  ConnectBoard("200.100.100.7");
  return 0;
}
