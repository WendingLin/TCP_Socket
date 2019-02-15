#include "check.h"

using namespace std;

vector<Player> players;

int main(int argc, char *argv[]) {
  int status;
  int socket_fd;
  fd_set read_fds;
  int ret;
  int player_num = 2;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port = "23333";

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  checkHostInfo(status, hostname, port);

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  checkCreateServerSocket(socket_fd, hostname, port);

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkIfBind(status, hostname, port);

  status = listen(socket_fd, 100);
  checkIfListen(status, hostname, port);

  for (int i = 0; i < player_num; i++) {
    cout << "Waiting for player connection on port" << port << endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_fd;
    client_fd =
        accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    checkCreateClientSocket(client_fd);

    string client_ip = getClientIP(socket_addr);
    int client_port = getClientPort(socket_addr);

    int player_seq = createPlayer(players, client_fd, client_ip, client_port);
    cout << "Seq: " << player_seq << " FD: " << client_fd
         << " IP: " << client_ip << " Port: " << client_port << endl;
    char buffer[512];
    recv(client_fd, buffer, 512, 0);
    buffer[512] = 0;

    cout << "Server received: " << buffer << endl;
  }

  while (1) {
    int max_fd = 0;
    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);
    int client_fd = players[0].client_fd;
    const char *message = "Start Game!";
    send(client_fd, message, strlen(message), 0);
    sleep(3);
  }

  freeaddrinfo(host_info_list);
  close(socket_fd);
  cout << "Player numbers: " << players.size() << endl;
  return 0;
}
