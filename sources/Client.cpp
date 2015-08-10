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
          handle_new_card();
        break;
        case message_type::accepted:

        break;
        case message_type::denied:

        break;
        case message_type::i_have_card:

        break;
        case message_type::give_id:
          set_id(message.value);
          connected = true;
          state = client_state::waiting_for_start;
        break;
        case message_type::client_list:
          recieve_client_list(message.value);
        break;
        case message_type::set_ready:

        break;
        case message_type::state_update:
          handle_state_update(message.value);
        break;


      }
    }
  }
  close(server_socket);
}

std::vector<client_info> Client::getClientList()
{
  clients_mtx.lock();
  auto tmp_vec = clients;
  clients_mtx.unlock();
  return tmp_vec;
}
void Client::recieve_client_list(int len)
{
  int pointeroffset = 0;
  int num = 0;
  client_info *tmp_info_ptr = (client_info *)malloc(len);
  do
  {
    if ((num = read(server_socket, ((char *)tmp_info_ptr)+pointeroffset, len-pointeroffset))== -1)
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
    pointeroffset = (pointeroffset + num) % len;
  } while(pointeroffset != 0);
  clients_mtx.lock();
  clients.clear();
  for(unsigned int i = 0; i < len / sizeof(client_info); i++)
  {
    if (true /*tmp_info_ptr[i].id != id*/)
    {
      clients.push_back(tmp_info_ptr[i]);
    }
  }
  clients_mtx.unlock();
  free(tmp_info_ptr);
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
  write_mtx.lock();
  network_message message = {NEW_CLIENT, hello, (unsigned int)client_settings_.nick_name.size() + 1};
  if (write(server_socket,&message, sizeof(message)) == -1)
  {
    std::cout << "Failure Sending Message" << std::endl;
    stop = true;
    connected = false;
    write_mtx.unlock();
    return 0;
  }
  if (write(server_socket, client_settings_.nick_name.c_str(), (unsigned int)client_settings_.nick_name.size() + 1) == -1)
  {
    std::cout << "Failure Sending Message" << std::endl;
    stop = true;
    connected = false;
    write_mtx.unlock();
    return 0;
  }
  write_mtx.unlock();
  return 1;
}

void Client::set_ready(bool ready_val)
{
  if(ready == ready_val) return;
  write_mtx.lock();
  network_message message = {id, message_type::set_ready, ready_val};

  write_mtx.unlock();
  if (write(server_socket,&message, sizeof(message)) == -1)
  {
    std::cout << "Failure Sending Message" << std::endl;
    stop = true;
    connected = false;
  }
  ready = ready_val;
  write_mtx.unlock();
}


bool Client::isReady()
{
  return ready;
}

client_state Client::getState()
{
  return state;
}

void Client::set_state(client_state state_)
{
  state = state_;
}

bool Client::isConnected()
{
  return connected;
}

void Client::handle_state_update(unsigned int state_)
{
  std::cout << "updated state to " << state_ << std::endl;;
  state =(client_state)state_;
}

void Client::handle_new_card()
{

  int pointeroffset = 0;
  int num = 0;
  Card card;
  do
  {
    if ((num = read(server_socket, ((char *)&card)+pointeroffset, sizeof(Card)-pointeroffset))== -1)
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
    pointeroffset = (pointeroffset + num) % sizeof(Card);
  } while(pointeroffset != 0);
  PlayerCard Pcard;
  Pcard.card = card;
  Pcard.shown = true;
  insertcard(Pcard);
}
void Client::insertcard(PlayerCard card)
{
  std::cout << "started to insert " << (int)card.card.color << "  " << (int)card.card.value << std::endl;
  int i = 1;
  bool found = false;
  while(!found)
  {
    found = true;
    for(auto &cur_card : cards)
    {
      if(cur_card.number == i)
      {
        found = false;
        break;
      }
    }
    i++;
  }
  std::cout << "inserted " << (int)card.card.color << "  " << (int)card.card.value << std::endl;
  card.number = i;
  cards.push_back(card);
}
void Client::hideCards()
{
  for(auto &card : cards)
    card.shown = false;
}
