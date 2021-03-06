#include "check.h"

using namespace std;

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
  checkPlayerArgc(argc, argv);
  int status;
  int ring_fd;
  int neigh_fd = -1;
  int client_fd = -1;
  int server_fd = -1;
  int ret;
  fd_set rfds;
  int player_id;
  int player_num;
  int basic_port = 23334;

  struct timespec time1 = {0, 0};
  clock_gettime(CLOCK_REALTIME, &time1);
  srand(time1.tv_nsec);

  /* Player-Ringmaster as Client */
  struct addrinfo ring_host_info;
  struct addrinfo *ring_host_info_list;
  const char *ring_hostname = argv[1];
  const char *ring_port = argv[2];

  memset(&ring_host_info, 0, sizeof(ring_host_info));
  ring_host_info.ai_family = AF_UNSPEC;
  ring_host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(ring_hostname, ring_port, &ring_host_info,
                       &ring_host_info_list);
  checkHostInfo(status, ring_hostname, ring_port);

  ring_fd =
      socket(ring_host_info_list->ai_family, ring_host_info_list->ai_socktype,
             ring_host_info_list->ai_protocol);
  checkCreateServerSocket(ring_fd, ring_hostname, ring_port);
  // cout << "Connecting to " << ring_hostname << " on port " << ring_port <<
  // "..."
  //<< endl;
  status = connect(ring_fd, ring_host_info_list->ai_addr,
                   ring_host_info_list->ai_addrlen);
  checkIfConnect(status, ring_hostname, ring_port);

  // char buffer[BUF_SIZE];
  // memset(buffer, 0, BUF_SIZE);
  // recv(ring_fd, buffer, 512, 0);
  // const char *message = "hi there!";
  // send(socket_fd, message, strlen(message), 0);

  /* Player-Player as Server */
  struct addrinfo server_host_info;
  struct addrinfo *server_host_info_list;
  const char *server_hostname = NULL;
  int real_port;

  /* Player-Player as Client*/
  struct addrinfo neigh_host_info;
  struct addrinfo *neigh_host_info_list;
  const char *neigh_hostname;
  const char *neigh_port;

  while (1) {
    FD_ZERO(&rfds);
    FD_SET(ring_fd, &rfds);
    if (server_fd > 0) {
      FD_SET(server_fd, &rfds);
    }
    if (neigh_fd > 0) {
      FD_SET(neigh_fd, &rfds);
    }
    if (client_fd > 0) {
      FD_SET(client_fd, &rfds);
    }
    ret = select(512, &rfds, NULL, NULL, NULL);
    // cout << "BEFORE FOR IN ISSET" << endl;
    checkIfSelect(ret);

    if (FD_ISSET(ring_fd, &rfds)) {
      char buf[BUF_SIZE];
      memset(buf, 0, BUF_SIZE);
      int status = recv(ring_fd, buf, BUF_SIZE, 0);
      if (status == 0)
        break;
      checkReceive(status);
      string recvdata = string(buf);
      // cout << "Receive: " << recvdata << endl;
      string header = getHeader(recvdata);
      if (header == "PLAYER") {
        // cout << "-------------GET ID----------------" << endl;

        player_id = atoi(getPlayerID(recvdata).c_str());
        player_num = atoi(getPlayerNum(recvdata).c_str());
        cout << "Connected as player " << player_id << " out of " << player_num
             << " total players" << endl;
        // cout << "-------------OPEN SERVER----------------" << endl;
        /* Player-Player as Server */
        real_port = basic_port + player_id;
        stringstream ss;
        ss << real_port;
        const char *server_port = ss.str().c_str();

        // cout << "Server Port is " << server_port << endl;
        memset(&server_host_info, 0, sizeof(server_host_info));

        server_host_info.ai_family = AF_UNSPEC;
        server_host_info.ai_socktype = SOCK_STREAM;
        server_host_info.ai_flags = AI_PASSIVE;

        status = getaddrinfo(server_hostname, server_port, &server_host_info,
                             &server_host_info_list);
        checkHostInfo(status, server_hostname, server_port);

        server_fd = socket(server_host_info_list->ai_family,
                           server_host_info_list->ai_socktype,
                           server_host_info_list->ai_protocol);
        checkCreateServerSocket(server_fd, server_hostname, server_port);

        int yes = 1;
        status =
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        status = bind(server_fd, server_host_info_list->ai_addr,
                      server_host_info_list->ai_addrlen);
        checkIfBind(status, server_hostname, server_port);

        status = listen(server_fd, 10);
        checkIfListen(status, server_hostname, server_port);
        // cout << "Waiting for connection on port" << server_port << endl;
        // cout << "-------------OPEN SERVER SUCCESS----------------" << endl;
        if (player_id + 1 == player_num) {
          const char *message = "READY_SERVER:";
          send(ring_fd, message, strlen(message), 0);
        }
      } else if (header == "CONNECT") {
        // cout << "-------------CONNECT NEIGHBOUR----------------" << endl;
        /* Player-Player as Client */

        int cal_neigh_port = basic_port + (player_id + 1) % player_num;
        stringstream ss;
        ss << cal_neigh_port;
        string temphost = getConnectIP(recvdata);
        neigh_hostname = temphost.c_str();
        neigh_port = ss.str().c_str();

        memset(&neigh_host_info, 0, sizeof(neigh_host_info));
        neigh_host_info.ai_family = AF_UNSPEC;
        neigh_host_info.ai_socktype = SOCK_STREAM;

        status = getaddrinfo(neigh_hostname, neigh_port, &neigh_host_info,
                             &neigh_host_info_list);
        checkHostInfo(status, neigh_hostname, neigh_port);

        neigh_fd = socket(neigh_host_info_list->ai_family,
                          neigh_host_info_list->ai_socktype,
                          neigh_host_info_list->ai_protocol);
        checkCreateClientSocket(neigh_fd);
        // cout << "Connecting to " << neigh_hostname << " on port " <<
        // neigh_port
        // << "..." << endl;
        status = connect(neigh_fd, neigh_host_info_list->ai_addr,
                         neigh_host_info_list->ai_addrlen);
        checkIfConnect(status, neigh_hostname, neigh_port);
        // cout << "RING FD: " << ring_fd << endl;
        // cout << "SERVER FD: " << server_fd << endl;
        // cout << "NEIGH FD: " << neigh_fd << endl;

        // cout << "LOCAL PORT: " << real_port << endl;
        // cout << "Neighbour PORT: " << neigh_port << endl;

      } else if (header == "POTATO") {
        int hops = atoi(getHops(recvdata).c_str());
        /*if (hops == 0) {
          const char *message = "POTATO:";
          send(ring_fd, message, strlen(message), 0);
          continue;
          }*/
        // cout << "Player <" << player_id << "> get the potato" << endl;
        // cout << "Hops Received: " << hops << endl;
        hops--;
        string order = getOrder(recvdata);

        if (hops == 0) {
          string msgstring = rebuildPotato(order, player_id);
          cout << "I'm it" << endl;
          const char *message = msgstring.c_str();
          send(ring_fd, message, strlen(message), 0);
        } else {
          string msgstring = rebuildPotato(hops, order, player_id);
          const char *message = msgstring.c_str();
          // cout << recvdata << endl;
          // cout << message << endl;

          int random = rand() % 2;
          if (random == 1) {

            cout << "Sending potato to " << ((player_id + 1) % player_num)
                 << endl;
            send(neigh_fd, message, strlen(message), 0);
          } else {

            cout << "Sending potato to "
                 << ((player_id + player_num - 1) % player_num) << endl;
            send(client_fd, message, strlen(message), 0);
          }
        }
        // cout << "Send Success when hops = " << (hops + 1) << endl;
        // cout << endl << endl << endl;
        continue;
      } else if (header == "CLOSE") {
        // cout << "-------------GAME ENDS----------------" << endl;
        // cout << "-------------DISCONNECT----------------" << endl;
        // cout << "-------------CLOSE SERVER----------------" << endl;
        break;
      }
    }

    else if (FD_ISSET(neigh_fd, &rfds)) {
      char buf[BUF_SIZE];
      memset(buf, 0, BUF_SIZE);
      // cout << "Inside NEIGH recv" << endl;
      int status = recv(neigh_fd, buf, BUF_SIZE, 0);
      if (status == 0)
        break;
      checkReceive(status);
      string recvdata = string(buf);
      // cout << "Neigh Receive: " << recvdata << endl;
      string header = getHeader(recvdata);
      if (header == "POTATO") {
        int hops = atoi(getHops(recvdata).c_str());
        // cout << "Player <" << player_id << "> get the potato" << endl;
        // cout << "Hops Received: " << hops << endl;
        hops--;
        string order = getOrder(recvdata);
        const char *message;
        if (hops == 0) {
          string msgstring = rebuildPotato(order, player_id);
          cout << "I'm it" << endl;
          message = msgstring.c_str();
          send(ring_fd, message, strlen(message), 0);
        } else {
          string msgstring = rebuildPotato(hops, order, player_id);
          message = msgstring.c_str();

          int random = rand() % 2;
          if (random == 1) {

            cout << "Sending potato to " << ((player_id + 1) % player_num)
                 << endl;
            send(neigh_fd, message, strlen(message), 0);
          } else {

            cout << "Sending potato to "
                 << ((player_id + player_num - 1) % player_num) << endl;
            send(client_fd, message, strlen(message), 0);
          }
        }
      }
    } else if (FD_ISSET(client_fd, &rfds)) {
      char buf[BUF_SIZE];
      memset(buf, 0, BUF_SIZE);
      int status = recv(client_fd, buf, BUF_SIZE, 0);
      if (status == 0)
        break;
      checkReceive(status);
      string recvdata = string(buf);
      // cout << "Client Receive: " << recvdata << endl;
      string header = getHeader(recvdata);
      if (header == "POTATO") {
        int hops = atoi(getHops(recvdata).c_str());
        // cout << "Player <" << player_id << "> get the potato" << endl;
        // cout << "Hops Received: " << hops << endl;
        hops--;
        string order = getOrder(recvdata);

        if (hops == 0) {
          string msgstring = rebuildPotato(order, player_id).c_str();
          cout << "I'm it" << endl;
          const char *msg = msgstring.c_str();
          send(ring_fd, msg, strlen(msg), 0);
        } else {
          string msgstring = rebuildPotato(hops, order, player_id);
          const char *message = msgstring.c_str();

          int random = rand() % 2;
          if (random == 1) {

            cout << "Sending potato to " << ((player_id + 1) % player_num)
                 << endl;
            send(neigh_fd, message, strlen(message), 0);
          } else {

            cout << "Sending potato to "
                 << ((player_id + player_num - 1) % player_num) << endl;
            send(client_fd, message, strlen(message), 0);
          }
        }
        // cout << "Send Success when hops = " << (hops + 1) << endl;
        // cout << endl << endl << endl;
        continue;
      }
    } else if (FD_ISSET(server_fd, &rfds)) {
      struct sockaddr_storage socket_addr;
      socklen_t socket_addr_len = sizeof(socket_addr);

      client_fd =
          accept(server_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
      checkCreateClientSocket(client_fd);

      string client_ip = getClientIP(socket_addr);
      int client_port = getClientPort(socket_addr);
      // cout << "NEIGH2 FD: " << client_fd << endl;
      // cout << " FD: " << client_fd << " IP: " << client_ip
      // << " Port: " << client_port << endl;

      // const char *msg = "HELLO！！";
      // send(client_fd, msg, strlen(msg), 0);

      const char *message = "READY_NEIGHBOUR:";
      send(ring_fd, message, strlen(message), 0);

      // cout << "-------------CONNECT NEIGHBOUR SUCCESS----------------" <<
      // endl;

      continue;
    } else
      continue;
    // cout << "Try to send msg" << endl;
    // const char *message = "TEST_SELECT";
    // send(ring_fd, message, strlen(message), 0);
    // sleep(7);
    // char buffer[512];
    // recv(socket_fd, buffer, 512, 0);
    // cout << "Received: " << string(buffer) << endl;
  }

  freeaddrinfo(neigh_host_info_list);
  close(neigh_fd);
  freeaddrinfo(server_host_info_list);
  close(server_fd);
  freeaddrinfo(ring_host_info_list);
  close(ring_fd);

  return 0;
}
