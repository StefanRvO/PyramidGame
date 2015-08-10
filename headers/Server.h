#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include "ClientHandler.h"
#include <mutex>
#include <thread>
#include <vector>
#include "structs.h"
#include "PyramidGame.h"

#define SERVER 0;



class Server
{
  private:
		std::mutex clients_mtx;
    std::vector<ClientHandler *> clients;
		socklen_t size;
		struct sockaddr_in server_addr, client_addr;
    int server_socket, client_socket;
    server_state server_state_ = server_state::get_clients;
		bool stop = false;
		std::thread server_thread_handle;
		server_settings server_settings_;
		void connect_client();
		int client_id_counter = 1;
		PyramidGame *game = NULL;
  public:
		void send_clientlist();
		std::vector<client_info> getClientList();
    Server(server_settings settings);
		void server_thread();
		server_state getState();
		void setState(server_state state_);
		void send_state_update(client_state state_);
		void deal_cards();
    ~Server();
};

void server_thread_wrapper(Server *server);
