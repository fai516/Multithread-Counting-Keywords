#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

string dataPath = "./data/";

void OpenFile(ifstream &obj, string path, string filename,bool showMessage);
void ReadIntoQueue(ifstream &obj,deque<string> queue);
void UserInput(int &mapper_count, int &reducer_count, string &keyword);

namespace multithread{
  deque<string> file_queue; //contains filename that will be read
  deque<int> count_queue; //contains the occurence of a keyword from files
  int currentMapperWorking=0; //shows how many mappers are processing right now. This will prevent reducers from early stop.
  mutex fileQueueMTX;
  mutex countQueueMTX;
  mutex consoleOutMTX;//no interrupt during printing message in terminal
  class mapper{
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
    mapper(string d, string k,int i){
      id = i;
      dataPath = d;
      keyword = k;
      _th = thread(&mapper::mapper::start,this);
    }
    ~mapper(){
      if(_th.joinable()){
        _th.join();
      }
    }
    void join(){
      _th.join();
    }
  };
  class reducer{
    int id;
    thread _th;
    void startSumming();
  public:
    reducer(int i){
      id = i;
      _th = thread(&reducer::startSumming,this);
    }
    ~reducer(){
      if(_th.joinable()){
        _th.join();
      }
    }
    void join(){
      _th.join();
    }
  };

}



int main(){
  string keyword;
  int N_mapper=6;
  int N_reducer=4;
  string dataPath = "./data/";

  ifstream FILE;
  OpenFile(FILE,dataPath,"files.dat",true); //OpenFile
  ReadIntoQueue(FILE,multithread::file_queue);
  FILE.close();

  UserInput(N_mapper,N_reducer,keyword);
  cout << "Keyword: " << keyword << endl;

  chrono::system_clock::time_point start = chrono::system_clock::now();
  vector<multithread::mapper*> mappers;
  for(int i=0;i<N_mapper;i++){
    multithread::mapper *ptr = new multithread::mapper(dataPath,keyword,i);
    mappers.push_back(ptr);
  }
  
  vector<multithread::reducer*> reducers;
  for(int i=0;i<N_reducer;i++){
    multithread::reducer *ptr = new multithread::reducer(i);
    reducers.push_back(ptr);
  }

  for(int i=0;i<N_mapper;i++){
    mappers[i]->join();
  }
  for(int i=0;i<N_reducer;i++){
    reducers[i]->join();
  }
  chrono::system_clock::time_point end = chrono::system_clock::now();

  cout << "=============" << endl;

  cout << "Total count of \"" << keyword << "\": " << multithread::count_queue.front() <<  endl;
  cout << "Time elapse: " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms" <<endl;
  return 0;
}
void ReadIntoQueue(ifstream &obj,deque<string> queue){
  string tmp;
  while(getline(obj, tmp)){
    multithread::file_queue.push_back(tmp); //push file name into the queue
  }
}

void OpenFile(ifstream &obj, string path, string filename, bool showMessage){
  obj.open(path+filename);
  if(showMessage){
    cout << "Opening " << filename << "...";
    if(obj.is_open()){
      cout << "[Sucessful]" << endl;
    }
    else{
      cout << "[Fail]" << endl;
    }
  }
}

void UserInput(int &mapper_count, int &reducer_count, string &keyword){
  cout << "Enter the number of Mapper: ";
  cin >> mapper_count;
  cout << "Enter the number of Reducer: ";
  cin >> reducer_count;
  cout << "Enter the keyword: ";
  cin >> keyword;
}


void multithread::reducer::startSumming(){
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
        multithread::consoleOutMTX.lock();
        sum=a+b;
        cout << "Reducer #" << id+1 << " pushing " << a << "+" << b << "=" << sum << " into count_queue"<< endl;
        multithread::consoleOutMTX.unlock();
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

/** thread safe function **/
/** All variabes are local variable ***/
/** no longer use **/
/*
int multithread::mapper::countSubstring(string &line){
  int sum=0;
  int currentIndex = 0;
  bool isContinuous = true;
  map<char,bool> mapping;
  for(int i=0;i<line.size();i++){
    if(line[i]==keyword[0]){
      int match=1;
      for(int j=1;j<keyword.size() && (i+j)<line.size();j++){
        if(keyword[j]==line[i+j]){
          match++;
        }
        else{
          break;
        }
      }
      if(match == keyword.size()){
        sum++;
        i+=keyword.size()-1;
      }
    }
  }
  return sum;
}
*/

int multithread::mapper::countLineByLine(){
  int count=0;
  string word;
  /* old version
  while(getline(file,line)){
    count += countSubstring(line);
  }
  */
  while(file >> word){
    if(keyword==word)
      count++;
  }
  return count;
}
void multithread::mapper::start(){
  multithread::currentMapperWorking++;
  while(true){
    multithread::fileQueueMTX.lock();
    if(multithread::file_queue.size() != 0){ //check if the queue have filename remaining
      filename = multithread::file_queue.front();
      multithread::consoleOutMTX.lock();
      cout << "Mapper #" << id+1 << " reading " << filename << endl;
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
    cout << "Mapper #" << id+1 << " pushing " << count << " into count_queue" << endl;
    multithread::consoleOutMTX.unlock();
    multithread::count_queue.push_back(count);
    multithread::countQueueMTX.unlock();
  }
}