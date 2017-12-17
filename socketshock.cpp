#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include "Console.h"

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512

void recvmsg(SOCKET &ClientSocket){
  char recvbuf[DEFAULT_BUFLEN];
  int recvbuflen=DEFAULT_BUFLEN;
  int iResult;
  do{
    iResult=recv(ClientSocket,recvbuf,recvbuflen,0);
    if(iResult>0){
      logW("Bytes received","ok",red);
      printf("Bytes received:%d\n",iResult);
      printf(":");
      cout<<blue;
      printf("%s\n",recvbuf);
      cout<<white;
      if(!strcmp(recvbuf,"[quit]"))
        break;
    }
    if(iResult<0){
      printf("recv failed:%d\n",WSAGetLastError());
    }
  }while(1);
}
void sendmsg(SOCKET &ClientSocket){
  do{
    int iSendResult=SOCKET_ERROR;
    char sendbuf[DEFAULT_BUFLEN-1]="hello world";
    printf("input msg\n:");
    gets(sendbuf);
    do{
      if(iSendResult==SOCKET_ERROR){
        iSendResult=send(ClientSocket,sendbuf,(int)(strlen(sendbuf)+1),0);
        if(iSendResult==SOCKET_ERROR){
          printf("send failed:%d\n",WSAGetLastError());
        }else{
          logW("Bytes send","ok",red);
          printf("Bytes sent:%d\n",iSendResult);
          printf(":");
          cout<<red;
          printf("%s\n",sendbuf);
          cout<<white;
          break;
        }
      }
    }while(1);
    if(!strcmp(sendbuf,"[quit]"))
      break;
  }while(1);
}
void asServer(struct addrinfo *result){
  SOCKET ListenSocket=INVALID_SOCKET;
  int iResult;
  do{
    ListenSocket=socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    if(ListenSocket==INVALID_SOCKET){
      printf("Error at socket():%d\n",WSAGetLastError());
      continue;
    }
    logW("socket()","ok",green);
      iResult=bind(ListenSocket,result->ai_addr,(int)result->ai_addrlen);
    if(iResult==SOCKET_ERROR){
      printf("bind failed with error:%d\n",WSAGetLastError());
      continue;
    }
    logW("bind()","ok",green);
    if(listen(ListenSocket,SOMAXCONN)==SOCKET_ERROR){
      printf("Listen failed with error:%ld\n",WSAGetLastError());
      continue;
    }
    logW("listen()","ok",red);
    break;
  }while(1);
  
  SOCKET ClientSocket;
  ClientSocket=INVALID_SOCKET;
  do{
    ClientSocket=accept(ListenSocket,NULL,NULL);
    if(ClientSocket==INVALID_SOCKET){
      printf("accept failed:%d\n",WSAGetLastError());
      continue;
    }
    break;
  }while(1);
  logW("accept a client socket","ok",blue);
  
  thread recvMSG{bind(recvmsg,ClientSocket)};
  thread sendMSG{bind(sendmsg,ClientSocket)};
  
  recvMSG.join();
  sendMSG.join();
  
  iResult=shutdown(ClientSocket,SD_SEND);
  if(iResult==SOCKET_ERROR){
    printf("shutdown failed:%d\n",WSAGetLastError());
    closesocket(ClientSocket);
    return;
  }
  logW("shutdown the connection","ok",green);
}
void asClient(struct addrinfo *result){
  struct addrinfo *ptr=NULL;
  SOCKET ConnectSocket=INVALID_SOCKET;
  int iResult;
  do{
    for(ptr=result;ptr!=NULL;ptr=ptr->ai_next){
      ConnectSocket=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
      
      if(ConnectSocket==INVALID_SOCKET){
        printf("Error at socket():%ld\n",WSAGetLastError());
        break;
      }
      logW("socket()","ok",yellow);
      
      iResult=connect(ConnectSocket,ptr->ai_addr,(int)ptr->ai_addrlen);
      if(iResult==SOCKET_ERROR){
        closesocket(ConnectSocket);
        ConnectSocket=INVALID_SOCKET;
        continue;
      }
      break;
    }
    if(ConnectSocket==INVALID_SOCKET){
      logW("Unable to connect to server!","erro",yellow);
      logW("ReConnecting","waiting",yellow);
      Sleep(500);
      continue;
    }
  }while(ConnectSocket==INVALID_SOCKET||iResult==SOCKET_ERROR);
  logW("connect()","ok",green);
  
  thread recvMSG{bind(recvmsg,ConnectSocket)};
  thread sendMSG{bind(sendmsg,ConnectSocket)};
  
  recvMSG.join();
  sendMSG.join();
  
  iResult=shutdown(ConnectSocket,SD_SEND);
  if(iResult==SOCKET_ERROR){
    printf("shutdown failed:%d\n",WSAGetLastError());
    closesocket(ConnectSocket);
    return;
  }
  logW("shutdown the connection","ok",green);
}
int main(int argc,char *argv[]){
    
  int iResult;
  
  /*Initialize Winsock*/
  
  WSADATA wsaData;
  iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
  if(iResult!=0){
    printf("WSAStartup failed:%d\n",iResult);
    return 1;
  }
  logW("WSAStartup","ok",green);

  struct addrinfo *result=NULL,hints;
  ZeroMemory(&hints,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=IPPROTO_TCP;
  
  int flag=0;
  printf("client(0) or server(1)\n:");
  scanf("%d",&flag);
  
  /*Resolve the server address and port*/
  
  char conAddress[16];
  printf("input address\n:");
  scanf("%s",conAddress);
  iResult=getaddrinfo(conAddress,DEFAULT_PORT,&hints,&result);
  if(iResult!=0){
    printf("getaddrinfo failed:%d\n",iResult);
    WSACleanup();
    return 1;
  }
  logW("getaddrinfo","ok",green);
  
  /*Calling Client or Server*/
  
  if(flag==0)
    asClient(result);
  if(flag==1)
    asServer(result);
  freeaddrinfo(result);
  
  /*Disconnecting the client*/

  WSACleanup();
  logW("closesocket()","ok",green);
  
  return 0;
}
