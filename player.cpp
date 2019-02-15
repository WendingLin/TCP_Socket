#include "check.h"

using namespace std;

int main(int argc, char *argv[]) {
  int status;
  int socket_fd;
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

  socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  checkCreateServerSocket(socket_fd, hostname, port);

  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;

  status =
      connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  checkIfConnect(status, hostname, port);

  const char *message = "hi there!";
  send(socket_fd, message, strlen(message), 0);
  while (1) {
    char buffer[512];
    recv(socket_fd, buffer, 512, 0);

    cout << "Received: " << string(buffer) << endl;
  }
  freeaddrinfo(host_info_list);
  close(socket_fd);

  return 0;
}
