#pragma once
#include "Server.h"

enum class client_state
{
	connecting,
	get_cards,
  game,
  guess_cards
};

struct client_settings
{
	int port;
	std::string server_adress;
	std::string nick_name;
	hostent *he;
};

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
		bool stop = false;
		bool connected = false;
		std::mutex id_mtx;
		void set_id(int);
		void recieve_client_list(int);
  public:
		std::vector<client_info> getClientList();
    void client_thread();
    Client(client_settings settings);
    ~Client();
    bool isConnected();
    void disconnect();
    std::string &GetNick();
		int send_hello();
};

void client_thread_wrapper(Client *client);
