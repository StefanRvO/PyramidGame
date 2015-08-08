#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include "ClientHandler.h"
#include <mutex>
#include <thread>
#include <vector>

#define SERVER 0;

struct server_settings
{
	int percent_flipped;
	int base_size;
	int port;
};

enum class server_state
{
	get_clients,
	deal_cards,
	flip_card,
	give_drinks,
	trust_drinks,
	give_new_cards,
	drink,
  guess_cards
};


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
  public:
		void send_clientlist();
		std::vector<client_info> getClientList();
    Server(server_settings settings);
		void server_thread();
    ~Server();
};

void server_thread_wrapper(Server *server);
