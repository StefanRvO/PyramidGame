#include "../headers/ClientHandler.h"
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#define SERVER 0
ClientHandler::ClientHandler(int id_, int client_socket_, struct sockaddr_in client_addr_)
{
  id = id_;
  client_socket = client_socket_;
  client_addr = client_addr_;
  client_thread_handle = std::thread(client_handler_thread_wrapper, this);
}

void ClientHandler::disconnect()
{
  stop = true;
  close(client_socket);
  client_thread_handle.join();
  connected = false;
}


void ClientHandler::client_handler_thread()
{
  int num;
  network_message message;
  int pointeroffset = 0;
  char network_str[100];

  inet_ntop(AF_INET, &client_addr.sin_addr, network_str, sizeof(network_str));
  std::cout  << " connection from " << network_str
    << ":" << client_addr.sin_port << std::endl;
  while(!stop)
  {
    //get message type
    if ((num = read(client_socket, ((char *)&message)+pointeroffset, sizeof(network_message)-pointeroffset)) == -1)
    {
      std::cout <<"recv error" << std::endl;
        exit(1);
    }
    else if(num == 0)
    { //disconnected
      connected = false;
      stop = true;
      std::cout << "disconnected" << std::endl;
      break;
    }
    pointeroffset = (pointeroffset + num) % sizeof(message);
    if (pointeroffset == 0)
    {
      switch(message.type)
      {
        case message_type::hello:
          if(!handle_hello(message) || !send_id())
          {
            connected = false;
            stop = true;
          }
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

        break;
      }
    }
  }
  close(client_socket);
}

bool ClientHandler::handle_hello(network_message msg)
{
  //recieve message
  int num;
  char *name = new char[msg.value];
  int pointeroffset = 0;
  do
  {
    if ((num = read(client_socket, name+pointeroffset, msg.value-pointeroffset))== -1)
    {
        std::cout <<"recv error" << std::endl;
        exit(1);
    }
    else if(num == 0)
    { //disconnected
      std::cout <<"Disconnected" << std::endl;
      return false;
    }
    pointeroffset = (pointeroffset + num) % msg.value;
  } while(pointeroffset != 0);
  nick_mtx.lock();
  nick = std::string(name);
  nick_mtx.unlock();
  delete name;
  connected = true;
  return true;
}

bool ClientHandler::send_id()
{
  network_message message;
  message.origin = SERVER;
  message.type = message_type::give_id;
  message.value = id;
  if ((write(client_socket, &message, sizeof(network_message)) == -1))
  {
    return 0;
  }

  return 1;
}



void client_handler_thread_wrapper(ClientHandler *CHandler)
{
  CHandler->client_handler_thread();
}
