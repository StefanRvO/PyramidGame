#include <cstdio>
#include <sys/socket.h>
#include "../headers/Server.h"
#include <cstring>
#include <unistd.h>
#include <iostream>
#define BACKLOG 100
Server::Server(server_settings settings)
{
  server_settings_ = settings;
  if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    fprintf(stderr, "Socket failure!!\n");
    exit(1);
  }
  int yes = 1;
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("error in setsockopt");
    exit(1);
  }
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_settings_.port);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if((bind(server_socket, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))) == -1)
  {
    fprintf(stderr, "Binding Failure\n");
    exit(1);
  }

  if ((listen(server_socket, BACKLOG))== -1)
  {
    fprintf(stderr, "Listening Failure\n");
    exit(1);
  }
  server_thread_handle = std::thread(server_thread_wrapper, this);
}

Server::~Server()
{
  stop = true;
  for (auto &client : clients)
  {
    delete client;
  }
  close(server_socket);
  server_thread_handle.join();
}

void server_thread_wrapper(Server *server)
{
  server->server_thread();
}

void Server::server_thread()
{
  while(!stop)
  {
    switch(server_state_)
    {
      case server_state::get_clients:
      connect_client();
      break;

      case server_state::deal_cards:

      break;
      case server_state::flip_card:

      break;
      case server_state::give_drinks:

      break;
      case server_state::trust_drinks:

      break;
      case server_state::give_new_cards:

      break;
      case server_state::drink:

      break;
      case server_state::guess_cards:

      break;
    }
  }
}

void Server::connect_client()
{
  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);


  int retval = select(server_socket, &rfds, NULL, NULL, &tv);
  if(retval == -1)
  {
    std::cout << "Error in select()" << std::endl;
  }
  else if(retval)
  {
    //Got a connection
    socklen_t size = sizeof(struct sockaddr_in);
    int client_socket;
    struct sockaddr client_addr;
    if((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &size) == -1))
    {
      perror("error in accept");
      exit(0);
    }
    //Add client to vector
    clients_mtx.lock();
    clients.push_back(new ClientHandler(client_id_counter++, client_socket, client_addr));
    clients_mtx.unlock();
  }
}
