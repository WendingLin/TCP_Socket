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

bool is_digits(const std::string &str) {
  return str.find_first_not_of("0123456789") == std::string::npos;
}

void checkPort(const string &port) {
  if (is_digits(port) == false) {
    cerr << "Error: Please input correct port number" << endl;
    exit(EXIT_FAILURE);
  }
  const char *port_temp = port.c_str();
  int port_digit = atoi(port_temp);
  if (port_digit < 0 || port_digit > 65535) {
    cerr << "Error: Please input correct port number: 0~65535" << endl;
    exit(EXIT_FAILURE);
  }
}

void checkPlayerNum(const string &player_num_str) {
  if (is_digits(player_num_str) == false) {
    cerr << "Error: Please input correct player number" << endl;
    exit(EXIT_FAILURE);
  }
  const char *player_num_temp = player_num_str.c_str();
  int player_num = atoi(player_num_temp);
  if (player_num < 2 || player_num > 1024) {
    cerr << "Error: Please input correct player number: 0~1024" << endl;
    exit(EXIT_FAILURE);
  }
}

void checkHopsNum(const string &hops_num_str) {
  if (is_digits(hops_num_str) == false) {
    cerr << "Error: Please input correct hops number" << endl;
    exit(EXIT_FAILURE);
  }
  const char *hops_num_temp = hops_num_str.c_str();
  int hops_num = atoi(hops_num_temp);
  if (hops_num < 0 || hops_num > 512) {
    cerr << "Error: Please input correct hops number: 0~512" << endl;
    exit(EXIT_FAILURE);
  }
}

void checkRingArgc(int argc, char *argv[]) {
  if (argc < 4) {
    cout << "Syntax: ringmaster <port_num> <num_player> <num_hops>\n" << endl;
    exit(EXIT_FAILURE);
  }
  /*Check Number*/
  string port_str = string(argv[1]);
  checkPort(port_str);
  string player_num_str = string(argv[2]);
  checkPlayerNum(player_num_str);
  string hops_str = string(argv[3]);
  checkHopsNum(hops_str);
}

void checkPlayerArgc(int argc, char *argv[]) {
  if (argc < 3) {
    cout << "Syntax: player <machine_name> <port_num> \n" << endl;
    exit(EXIT_FAILURE);
  }
  /*Check Number*/
  string port_str = string(argv[2]);
  checkPort(port_str);
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
    printf("Oh dear, something went wrong with  %s\n", strerror(errno));
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
    return;
    // cerr << "Error: Client has been closed" << endl;
    // exit(EXIT_FAILURE);
  }
}

void checkCreateClientSocket(int socket_fd) {
  if (socket_fd == -1) {

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
  char str[INET_ADDRSTRLEN];

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

string getPlayerID(string recvdata) {
  size_t pos_m = recvdata.find_first_of(':');
  size_t pos_c = recvdata.find_first_of(',');
  string id = recvdata.substr(pos_m + 1, pos_c - pos_m - 1);
  return id;
}

string buildConnect(const Player &player) {
  string connect = "CONNECT:";
  connect.append(player.ip);
  return connect;
}

string getConnectIP(string recvdata) {
  size_t pos_m = recvdata.find_first_of(':');
  return recvdata.substr(pos_m + 1);
}

string getPlayerNum(string recvdata) {
  size_t pos_c = recvdata.find_first_of(',');
  string num = recvdata.substr(pos_c + 1);
  return num;
}
string getHops(string recvdata) {
  size_t pos_m = recvdata.find_first_of(':');
  size_t pos_u = recvdata.find_first_of('_');
  string hops = recvdata.substr(pos_m + 1, pos_u - pos_m - 1);
  return hops;
}

string getOrder(string recvdata) {
  size_t pos_u = recvdata.find_first_of('_');
  if (pos_u + 1 == recvdata.size())
    return "";
  else {
    string order = recvdata.substr(pos_u + 1);
    return order;
  }
}

string buildPotato(int hops) {
  stringstream ss;
  ss << "POTATO:";
  ss << hops;
  ss << "_";
  return ss.str();
}

string rebuildPotato(string order, int player_id) {
  stringstream ss;
  ss << "POTATO:";
  ss << order;
  ss << player_id;
  return ss.str();
}

string rebuildPotato(int hops, string order, int player_id) {
  // cout << "Inside REBUILD: hops " << hops << "order " << order << "player_id
  // "
  // << player_id << endl;
  stringstream ss;
  ss << "POTATO:";
  ss << hops;
  ss << "_";
  ss << order;
  ss << player_id;
  ss << ",";
  return ss.str();
}
void splitPotato(string recvdata) {
  size_t pos = recvdata.find_first_of(':');
  string data = recvdata.substr(pos + 1);
  cout << data << endl;
}
