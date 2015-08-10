#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <vector>
#include "structs.h"
#define NEW_CLIENT -1


class ClientHandler
{
  private:
		 std::mutex write_mtx;

     int client_socket;
     socklen_t size;
     struct sockaddr_in client_addr;
     std::thread client_thread_handle;
     bool connected = 0;
     bool ready = 0;
     bool stop = false;
     std::string nick;
     std::mutex nick_mtx;
     bool handle_hello(network_message);
     bool send_id();
		 void disconnected();
		 void *server_ptr;
		 void connection_start();
		 void handle_ready_set(int ready_);
  public:
		void send_clientlist(std::vector<client_info> &client_list);
		std::string getNick();
		int id;
    void client_handler_thread();
    ClientHandler(int id_, int client_socket_, struct sockaddr_in client_addr_, void *);
    bool isReady();
    bool isConnected();
    void disconnect();
    std::string &GetNick();
		void send_state_update(client_state state_);
		void giveCard(Card card);

};

void client_handler_thread_wrapper(ClientHandler *CHandler);
