#include "../headers/ClientHandler.h"
#include <unistd.h>
#include <iostream>
#define SERVER 0
ClientHandler::ClientHandler(int id_, int client_socket_, struct sockaddr client_addr_)
{
  id = id_;
  client_socket = client_socket_;
  client_addr = client_addr_;
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
  while(!stop)
  {
    //get message type
    if ((num = read(client_socket, ((char *)&message)+pointeroffset, sizeof(message)-pointeroffset))== -1)
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
          if(!handle_hello() || !send_id())
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

bool ClientHandler::handle_hello()
{
  //recieve message
  int num;
  hello_message message;
  int pointeroffset = 0;
  do
  {
    if ((num = read(client_socket, ((char *)&message)+pointeroffset, sizeof(hello_message)-pointeroffset))== -1)
    {
        perror("recv error");
        exit(1);
    }
    else if(num == 0)
    { //disconnected
      return false;
    }
    pointeroffset = (pointeroffset + num) % sizeof(hello_message);

  } while(pointeroffset == 0);

  nick_mtx.lock();
  nick = std::string(message.nick);
  nick_mtx.unlock();
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
