#include "../headers/Client.h"
#include "../headers/ClientHandler.h"
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>


Client::Client(client_settings settings)
{
  client_settings_ = settings;
  //setup network
  if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    fprintf(stderr, "Socket Failure!!\n");
    stop = true;
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(client_settings_.port);
  server_addr.sin_addr = *((struct in_addr *)client_settings_.he->h_addr);

  //connect to server
  if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
  {
    std::cout << "could not connect to server" << std::endl;
    stop = true;
  }
  if(!stop)
  {
    client_thread_handle = std::thread(client_thread_wrapper, this);
  }
  std::cout << "created client object" << std::endl;
}

Client::~Client()
{
  stop = true;
  close(server_socket);
  client_thread_handle.join();
  connected = false;
}


void client_thread_wrapper(Client *client)
{
  client->client_thread();
}

void Client::client_thread()
{
  int num;
  network_message message;
  int pointeroffset = 0;
  while(!stop)
  {
    //get message type
    if ((num = read(server_socket, ((char *)&message)+pointeroffset, sizeof(message)-pointeroffset))== -1)
    {
        perror("recv error");
        exit(1);
    }
    else if(num == 0)
    { //disconnected
      connected = false;
      stop = true;
      break;
    }
    pointeroffset = (pointeroffset + num) % sizeof(message);
    if (pointeroffset == 0)
    {
      switch(message.type)
      {
        case message_type::hello:

        break;
        case message_type::new_card:

        break;
        case message_type::accepted:

        break;
        case message_type::denied:

        break;
        case message_type::i_have_card:

        break;
        case message_type::give_id:
          set_id(message.value);
        break;

      }
    }
  }
  close(server_socket);
}

void Client::set_id(int id_)
{
  id_mtx.lock();
  id = id_;
  id_mtx.unlock();
}

int Client::send_hello()
{
  //Send a hello containing the nick
  network_message message = {NEW_CLIENT, hello, (unsigned int)client_settings_.nick_name.size() + 1};
  if (write(server_socket,&message, sizeof(message)) == -1)
  {
    std::cout << "Failure Sending Message" << std::endl;
    stop=true;
    connected = false;
    return 0;
  }
  if (write(server_socket, client_settings_.nick_name.c_str(), (unsigned int)client_settings_.nick_name.size() + 1) == -1)
  {
    std::cout << "Failure Sending Message" << std::endl;
    stop=true;
    connected = false;
    return 0;
  }
  return 1;
}
