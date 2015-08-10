#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <mutex>
#include <thread>
#include <vector>
#include "structs.h"

class Client
{
  private:
		std::mutex write_mtx;
		std::vector<client_info> clients;
		std::mutex clients_mtx;
    sockaddr_in server_addr;
    int server_socket;
    std::thread client_thread_handle;
    client_settings client_settings_;
    int id;
		bool ready = false;
		bool stop = false;
		bool connected = false;
		std::mutex id_mtx;
		void set_id(int);
		void recieve_client_list(int);
		client_state state;
		void handle_state_update(unsigned int state_);
		std::vector<PlayerCard> cards;
    void handle_new_card();
  public:
		void set_ready(bool ready_val);
		std::vector<client_info> getClientList();
    void client_thread();
    Client(client_settings settings);
    ~Client();
    bool isConnected();
    void disconnect();
    std::string &GetNick();
		int send_hello();
		bool isReady();
		void set_state(client_state state_);
		client_state getState();
    void insertcard(PlayerCard card);
    void hideCards();

};

void client_thread_wrapper(Client *client);
