#include <iostream>
#include <string>

struct Player {
  int client_fd;
  int port;
  std::string ip;
  int neigh_client_fd;
};
