#include "../headers/ClientHandler.h"
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include "../headers/Server.h"

void ClientHandler::disconnected()
{
  stop = true;
  connected = false;
  ((Server *)server_ptr)->send_clientlist();
}
void ClientHandler::connection_start()
{
  connected = true;
  ((Server *)server_ptr)->send_clientlist();
}
ClientHandler::ClientHandler(int id_, int client_socket_, struct sockaddr_in client_addr_, void *server_ptr_)
{
  id = id_;
  client_socket = client_socket_;
  client_addr = client_addr_;
  server_ptr = server_ptr_;
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
      disconnected();
    }
    else if(num == 0)
    { //disconnected
      disconnected();
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
            disconnected();
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
        case message_type::client_list:

        break;
        case message_type::set_ready:
          handle_ready_set(message.value);
        break;
        case message_type::state_update:

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
        return false;
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
  connection_start();
  return true;
}

bool ClientHandler::send_id()
{
  write_mtx.lock();
  network_message message;
  message.origin = SERVER;
  message.type = message_type::give_id;
  message.value = id;
  if ((write(client_socket, &message, sizeof(network_message)) == -1))
  {
    write_mtx.unlock();
    return 0;
  }
  write_mtx.unlock();
  return 1;
}

void ClientHandler::send_clientlist(std::vector<client_info> &client_list)
{
  write_mtx.lock();
  network_message message;
  message.origin = SERVER;
  message.type = message_type::client_list;
  message.value = client_list.size() * sizeof(client_info);
  if ((write(client_socket, &message, sizeof(network_message)) == -1))
  {
    disconnected();
  }
  if ((write(client_socket, client_list.data(), message.value) == -1))
  {
    disconnected();
  }

  write_mtx.unlock();
}

void ClientHandler::send_state_update(client_state state_)
{
  write_mtx.lock();
  network_message message;
  message.origin = SERVER;
  message.type = message_type::state_update;
  message.value = (unsigned int)state_;
  if ((write(client_socket, &message, sizeof(network_message)) == -1))
  {
    disconnected();
  }
  write_mtx.unlock();
}
void client_handler_thread_wrapper(ClientHandler *CHandler)
{
  CHandler->client_handler_thread();
}

bool ClientHandler::isConnected()
{
  return connected;
}

std::string ClientHandler::getNick()
{
  std::string tmp_str;
  nick_mtx.lock();
  tmp_str = nick;
  nick_mtx.unlock();
  return tmp_str;
}

void ClientHandler::handle_ready_set(int ready_)
{
  ready = ready_;
  ((Server *)server_ptr)->send_clientlist();
}

bool ClientHandler::isReady()
{
  return ready;
}
void ClientHandler::giveCard(Card card)
{
  std::cout << "send card to client " << (int)card.color << "  " << (int)card.value << std::endl;
  write_mtx.lock();
  network_message message;
  message.origin = SERVER;
  message.type = message_type::new_card;
  message.value = sizeof(Card);
  if ((write(client_socket, &message, sizeof(network_message)) == -1))
  {
    disconnected();
  }
  if ((write(client_socket, &card, sizeof(Card)) == -1))
  {
    disconnected();
  }

  write_mtx.unlock();

}
