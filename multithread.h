#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <deque>
#include <string>
#include <mutex>
#include <thread>
using namespace std;

namespace multithread{
  bool messageOn;
  deque<string> file_queue; //contains filename that will be read
  deque<int> count_queue; //contains the occurence of a keyword from files
  int currentMapperWorking=0; //shows how many Mappers are processing right now. This will prevent Reducers from early stop.
  mutex fileQueueMTX;
  mutex countQueueMTX;
  mutex consoleOutMTX;//no interrupt during printing message in terminal
}

#endif