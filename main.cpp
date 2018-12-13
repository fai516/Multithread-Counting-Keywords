
#include <iostream>
#include <vector>
#include <chrono>
#include "./Mapper.h"
#include "./Reducer.h"
#include "./File.h"
using namespace std;
string dataPath = "./data/";
bool messageOn = false;


int main(){
  string keyword;
  int N_Mapper=6;
  int N_Reducer=4;
  string dataPath = "./data/";

  ifstream FILE;
  OpenFile(FILE,dataPath,"files.dat",true); //OpenFile
  ReadIntoQueue(FILE,multithread::file_queue);
  FILE.close();

  UserInput(N_Mapper,N_Reducer,keyword,messageOn);
  cout << "Keyword: " << keyword << endl;
  multithread::messageOn = messageOn;
  multithread::currentMapperWorking = N_Mapper;

  chrono::system_clock::time_point start = chrono::system_clock::now();
  vector<multithread::Mapper*> Mappers;
  for(int i=0;i<N_Mapper;i++){
    multithread::Mapper *ptr = new multithread::Mapper(dataPath,keyword,i);
    Mappers.push_back(ptr);
  }
  
  vector<multithread::Reducer*> Reducers;
  for(int i=0;i<N_Reducer;i++){
    multithread::Reducer *ptr = new multithread::Reducer(i);
    Reducers.push_back(ptr);
  }

  for(int i=0;i<N_Mapper;i++){
    Mappers[i]->join();
  }
  for(int i=0;i<N_Reducer;i++){
    Reducers[i]->join();
  }
  chrono::system_clock::time_point end = chrono::system_clock::now();

  cout << "=============" << endl;

  cout << "Total count of \"" << keyword << "\": " << multithread::count_queue.front() <<  endl;
  cout << "Time elapse: " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms" <<endl;
  return 0;
}