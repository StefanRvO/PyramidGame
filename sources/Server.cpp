#include <cstdio>
#include <sys/socket.h>
#include "../headers/Server.h"
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#define BACKLOG 100

bool client_info_sort( client_info &ci_1, client_info &ci_2)
{
  return ci_1.id  < ci_2.id;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

Server::Server(server_settings settings)
{
  server_settings_ = settings;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
     error("ERROR opening socket");
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(server_settings_.port);

  if (bind(server_socket, (struct sockaddr *) &server_addr,
           sizeof(server_addr)) < 0)
           error("ERROR on binding");

  listen(server_socket,5);
  size = sizeof(client_addr);
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
  FD_ZERO(&rfds);
  FD_SET(server_socket, &rfds);


  int retval = select(server_socket + 1, &rfds, NULL, NULL, NULL);
  if(retval == -1)
  {
    std::cout << "Error in select()" << std::endl;
  }
  else if(retval)
  {
    //Got a connection
    client_socket = accept(server_socket,
                (struct sockaddr *) &client_addr,
                &size);
    if (client_socket < 0)
         error("ERROR on accept");

    //Add client to vector
    clients_mtx.lock();
    clients.push_back(new ClientHandler(client_id_counter++, client_socket, client_addr, this));
    clients_mtx.unlock();
  }
}

std::vector<client_info> Server::getClientList()
{
  std::vector<client_info> client_list;
  clients_mtx.lock();
  for(auto client : clients)
  {
    if(client->isConnected())
    {
      client_info tmp;
      strncpy(tmp.nick, client->getNick().c_str(), sizeof(tmp.nick));
      tmp.id = client->id;
      tmp.nick[sizeof(tmp.nick) - 1] = '\0';
      client_list.push_back(tmp);
    }
  }
  clients_mtx.unlock();
  sort(client_list.begin(), client_list.end(), client_info_sort);
  return client_list;
}

void Server::send_clientlist()
{
  auto client_list = getClientList();
  clients_mtx.lock();
  for(auto client : clients)
  {
    if(client->isConnected())
    {
      client->send_clientlist(client_list);
    }
  }
  clients_mtx.unlock();
}
