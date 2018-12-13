#ifndef MAPPER_H
#define MAPPER_H

#include <deque>
#include <fstream>
#include <string>
#include <thread>
#include "./multithread.h"
#include "File.h"
using namespace std;

namespace multithread{
  class Mapper{
    int id;
    thread _th;
    ifstream file;
    string dataPath;
    string filename;
    string keyword;
    int countLineByLine();
    void start();
    int countSubstring(string &line);    
  public:
    Mapper(string d, string k,int i);
    ~Mapper();
    void join();
  };
}

int multithread::Mapper::countLineByLine(){
  int count=0;
  string word;

  while(file >> word){
    if(keyword==word)
      count++;
  }
  return count;
}
void multithread::Mapper::start(){
  //multithread::currentMapperWorking++;
  while(true){
    multithread::fileQueueMTX.lock();
    if(multithread::file_queue.size() != 0){ //check if the queue have filename remaining
      filename = multithread::file_queue.front();
      multithread::consoleOutMTX.lock();
      if(messageOn)
      cout << "Mapper #" << id+1 << " reading " << filename <<  "-----" << messageOn << endl;
      multithread::consoleOutMTX.unlock();
      multithread::file_queue.pop_front();
    }
    else{//if not, unlock the mutex and return the function
      fileQueueMTX.unlock();
      multithread::currentMapperWorking--;
      return;
    }
    multithread::fileQueueMTX.unlock();
    /* process counting occurence of keyword */
    OpenFile(file,dataPath,filename,false);
    int count=countLineByLine();
    file.close();
    multithread::countQueueMTX.lock();
    multithread::consoleOutMTX.lock();
    if(messageOn)
    cout << "Mapper #" << id+1 << " pushing " << count << " into count_queue" << endl;
    multithread::consoleOutMTX.unlock();
    multithread::count_queue.push_back(count);
    multithread::countQueueMTX.unlock();
  }
}

multithread::Mapper::Mapper(string d, string k,int i){
  id = i;
  dataPath = d;
  keyword = k;
  _th = thread(&Mapper::Mapper::start,this);
}

multithread::Mapper::~Mapper(){
  if(_th.joinable()){
    _th.join();
  }
}
void multithread::Mapper::join(){
  _th.join();
}

#endif