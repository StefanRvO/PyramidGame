#pragma once
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>


enum message_type
{
	new_card,
	accepted,
	denied,
	hello,
	i_have_card,
  give_id
};

struct hello_message
{
	char nick[100];
};

struct network_message
{
  int origin; //where is this message from? clients have numbers 1.., server is 0
  message_type type; //message type
  int value; //indicates the lenght of the following message
};

class ClientHandler
{
  private:
     int client_socket;
     socklen_t size;
     struct sockaddr client_addr;
     std::thread client_thread_handle;
     bool connected = 0;
     bool ready = 0;
     int id;
     bool stop = false;
     std::string nick;
     std::mutex nick_mtx;
     bool handle_hello();
     bool send_id();
  public:
    void client_handler_thread();
    ClientHandler(int id_, int client_socket_, struct sockaddr client_addr_);
    bool isReady();
    bool isConnected();
    void disconnect();
    std::string &GetNick();
};

void client_handler_thread_wrapper(ClientHandler *CHandler);
