#include "check.h"

using namespace std;

#define BUF_SIZE 512

int main(int argc, char *argv[]) {
  int status;
  int ring_fd;
  int neigh_fd = -1;
  int server_fd = -1;
  int ret;
  fd_set rfds;
  int player_id;
  int player_num;
  int basic_port = 23334;

  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = argv[1];
  const char *port = "23333";

  if (argc < 2) {
    cout << "Syntax: client <hostname>\n" << endl;
    return 1;
  }

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  checkHostInfo(status, hostname, port);

  ring_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                   host_info_list->ai_protocol);
  checkCreateServerSocket(ring_fd, hostname, port);
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  status =
      connect(ring_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkIfConnect(status, hostname, port);

  char buffer[BUF_SIZE];
  memset(buffer, 0, BUF_SIZE);
  recv(ring_fd, buffer, 512, 0);
  // const char *message = "hi there!";
  // send(socket_fd, message, strlen(message), 0);

  while (1) {
    FD_ZERO(&rfds);
    FD_SET(ring_fd, &rfds);
    if (server_fd > 0) {
      FD_SET(server_fd, &rfds);
    }
    ret = select(player_num + 4, &rfds, NULL, NULL, NULL);
    // cout << "BEFORE FOR IN ISSET" << endl;
    checkIfSelect(ret);

    if (client_fd == -1)
      continue;
    else {
      char buf[BUF_SIZE];
      memset(buf, 0, BUF_SIZE);
      int status = recv(client_fd, buf, BUF_SIZE, 0);
      checkReceive(status);
      string header = getHeader(string(buf));
      if (header == "READY") {
        if (ready == player_num - 1) {
          cout << "-------------GAME START----------------" << endl;
        } else {
          ready++;
          cout << "> PLAYER READY NUM: " << ready << endl;
        }
      } else if (header == "POTATO") {
        cout << "-------------GAME ENDS----------------" << endl;
        cout << "Trace of potato:" << endl;
        splitPotato(string(buf));
        for (int i = 0; i < player_num; i++) {
          const char *message = "CLOSE:";
          send(players[i].client_fd, message, strlen(message), 0);
        }
        break;
      }
      // cout << "Try to send msg" << endl;
      // const char *message = "TEST_SELECT";
      // send(ring_fd, message, strlen(message), 0);
      // sleep(7);
      // char buffer[512];
      // recv(socket_fd, buffer, 512, 0);
      // cout << "Received: " << string(buffer) << endl;
    }
    freeaddrinfo(host_info_list);
    close(ring_fd);

    return 0;
  }
