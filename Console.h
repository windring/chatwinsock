#include <iostream>
#include <windows.h>
using namespace std;

enum color{
  red=FOREGROUND_RED,
  green=FOREGROUND_GREEN,
  blue=FOREGROUND_BLUE,
  yellow=FOREGROUND_GREEN|FOREGROUND_RED,
  white=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
};

inline ostream& operator<< (ostream& s,const color& c){
  HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hStdout,c);
  return s;
}

inline int logW(const char *contest,const char *state,color col){
  static int conW=0;
  if(!conW){
    int iResult;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(!(iResult=GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi))){
      cout<<"GetConsoleScreenBufferInfo failed:"<<red<<iResult<<white<<endl;
      return 1;
    }
    conW=csbi.dwSize.X;
  }
  cout<<setw(conW/2)<<left<<white<<contest
      <<setw(conW/2-2-strlen(state))<<right<<white<<"["<<col<<state<<white<<"]"
      <<endl;
  return 0;
}
