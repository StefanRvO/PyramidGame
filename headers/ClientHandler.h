#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <vector>

#define NEW_CLIENT -1

struct client_info
{
	char nick[100];
	int id;
};


enum message_type
{
	new_card,
	accepted,
	denied,
	i_have_card,
  give_id,
	hello,
	client_list,


};

struct network_message
{
  int origin; //where is this message from? clients have numbers 1.., server is 0, new client is -1
  message_type type; //message type
  unsigned int value; //indicates the lenght of the following message
};

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
};

void client_handler_thread_wrapper(ClientHandler *CHandler);
