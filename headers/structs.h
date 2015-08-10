#pragma once

#include <string>
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <mutex>
#include <thread>
#include <vector>

#define JACK 11
#define QUEEN 12
#define KING 13

#define HEARTS 0
#define TILES 1
#define CLOVERS 2
#define PIKES 3

struct client_info
{
	char nick[100];
	int id;
	bool ready;
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
	set_ready,
	state_update,
};

struct network_message
{
  int origin; //where is this message from? clients have numbers 1.., server is 0, new client is -1
  message_type type; //message type
  unsigned int value; //indicates the lenght of the following message, or other info.
};

struct server_settings
{
	int percent_flipped;
	int base_size;
	int port;
};

enum class server_state
{
	get_clients,
	deal_cards,
	flip_card,
	give_drinks,
	trust_drinks,
	give_new_cards,
	drink,
  guess_cards,
	wait_for_ready_to_flip,
};

enum class client_state
{
	waiting_for_start,
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

struct Card
{
  uint8_t color;
  uint8_t value;
};

struct PyramidCard
{
  Card card;
  bool flipped;
  bool shown;
};
struct PyramidRow
{
  PyramidCard *cards;
  uint8_t number_of_cards;
};

struct PlayerCard
{
  Card card;
  bool shown;
  uint8_t number;
};
