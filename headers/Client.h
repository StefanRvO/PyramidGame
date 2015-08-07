#pragma once
#include "ClientHandler.h"

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
    sockaddr_in server_addr;
    int server_socket;
    std::thread client_thread_handle;
    client_settings client_settings_;
    int id;
		bool stop = false;
		bool connected = false;
		std::mutex id_mtx;
		void set_id(int);
  public:
    void client_thread();
    Client(client_settings settings);
    ~Client();
    bool isConnected();
    void disconnect();
    std::string &GetNick();
		int send_hello();
};

void client_thread_wrapper(Client *client);
