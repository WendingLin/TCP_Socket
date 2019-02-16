#include "potato.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

void checkArgc(int argc, char *argv[]) {
  if (argc < 3) {
    cout << "Syntax: client <hostname>\n" << endl;
    exit(EXIT_FAILURE);
  }
  /*Check Number*/
}

void checkHostInfo(int status, const char *hostname, const char *port) {
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkCreateServerSocket(int socket_fd, const char *hostname,
                             const char *port) {
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkIfBind(int status, const char *hostname, const char *port) {
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkIfConnect(int status, const char *hostname, const char *port) {
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkIfListen(int status, const char *hostname, const char *port) {
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkIfSelect(int ret) {
  if (ret == -1) {
    cerr << "Error: cannot select on socket" << endl;
    exit(EXIT_FAILURE);
  } // if
}

void checkReceive(int status) {
  if (status == -1) {
    cerr << "Error: cannot receive data" << endl;
    exit(EXIT_FAILURE);
  } else if (status == 0) {
    cerr << "Error: Client has been closed" << endl;
    exit(EXIT_FAILURE);
  }
}

void checkCreateClientSocket(int socket_fd) {
  if (socket_fd == -1) {
    printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
    cerr << "Error: cannot accept connection on socket" << endl;
    exit(EXIT_FAILURE);
  } // if
}
void checkCreateSucess(int player_num, size_t players) {
  if ((size_t)player_num != players) {
    cerr << "Error: Not receive every players' information" << endl;
    exit(EXIT_FAILURE);
  } // if
}

string getClientIP(struct sockaddr_storage socket_addr) {
  struct sockaddr_in *temp = (struct sockaddr_in *)&socket_addr;
  struct in_addr in = temp->sin_addr;
  char str[INET_ADDRSTRLEN]; // INET_ADDRSTRLEN这个宏系统默认定义 16
  //成功的话此时IP地址保存在str字符串中。
  inet_ntop(AF_INET, &in, str, sizeof(str));
  return string(str);
}

int getClientPort(struct sockaddr_storage socket_addr) {
  struct sockaddr_in *temp = (struct sockaddr_in *)&socket_addr;
  return ntohs(temp->sin_port);
}

int createPlayer(vector<Player> &players, int client_fd, string ip, int port) {
  players.push_back(Player());
  int seq = players.size() - 1;
  players[seq].port = port;
  players[seq].client_fd = client_fd;
  players[seq].ip = ip;
  return seq;
}

string getHeader(string recvdata) {
  size_t pos = recvdata.find_first_of(':');
  return recvdata.substr(0, pos);
}

void splitPotato(string recvdata) {
  size_t pos = recvdata.find_first_of(':');
  string data = recvdata.substr(pos + 1);
  cout << data << endl;
}
