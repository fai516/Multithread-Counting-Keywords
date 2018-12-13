#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <string>
#include <deque>
#include <iostream>
#include "./multithread.h"
using namespace std;
void OpenFile(ifstream &obj, string path, string filename,bool showMessage);
void ReadIntoQueue(ifstream &obj,deque<string> queue);
void UserInput(int &Mapper_count, int &Reducer_count, string &keyword);


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

void UserInput(int &Mapper_count, int &Reducer_count, string &keyword, bool &messageOn){
  cout << "Message On? ";
  cin >> messageOn;
  cout << "Enter the number of Mapper: ";
  cin >> Mapper_count;
  cout << "Enter the number of Reducer: ";
  cin >> Reducer_count;
  cout << "Enter the keyword: ";
  cin >> keyword;
}

void ReadIntoQueue(ifstream &obj,deque<string> queue){
  string tmp;
  int count=0;
  while(getline(obj, tmp)){
    multithread::file_queue.push_back(tmp); //push file name into the queue
    count++;
  }
  cout << count << " files." << endl;
}
#endif