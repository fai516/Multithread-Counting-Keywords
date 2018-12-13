#ifndef REDUCER_H
#define REDUCER_H

#include <thread>
#include "./multithread.h"
using namespace std;
namespace multithread{
  class Reducer{
    int id;
    thread _th;
    void startSumming();
  public:
    Reducer(int i);
    ~Reducer();
    void join();
  };
}

multithread::Reducer::Reducer(int i){
  id = i;
  _th = thread(&Reducer::startSumming,this);
}
multithread::Reducer::~Reducer(){
  if(_th.joinable()){
    _th.join();
  }
}
void multithread::Reducer::join(){
  _th.join();
}

void multithread::Reducer::startSumming(){
  while(true){
    int sum,a,b;
    multithread::countQueueMTX.lock();
    //cout << "lock after" << endl;
    if(count_queue.size()>1){
      //cout << "size > 1 before" << endl;
        a = count_queue.front();//[a,b,c....]
        count_queue.pop_front();//[b,c....]
        b = count_queue.front();//[b,c....]
        count_queue.pop_front();//[c....]
        //multithread::consoleOutMTX.lock();
        sum=a+b;
        if(messageOn)
        cout << "Reducer #" << id+1 << " pushing " << a << "+" << b << "=" << sum << " into count_queue"<< endl;
        //multithread::consoleOutMTX.unlock();
        count_queue.push_back(sum);//[c....,sum];
        multithread::countQueueMTX.unlock();
    }
    else if(multithread::currentMapperWorking==0){
      multithread::countQueueMTX.unlock();
      return;
    }
    else{
      multithread::countQueueMTX.unlock();
    }
  }
}

#endif