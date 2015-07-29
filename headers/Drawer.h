#pragma once
#include "Timer.h"
#include "TextDrawer.h"
#include "EventHandler.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <mutex>
#include "../headers/Server.h"
#define WINDOWSIZE_X 750
#define WINDOWSIZE_Y 600
#define DRAWFPS 20
enum main_menu_options
{
	none,
	server,
	client,
	quit
};

enum class game_state
{
	main_menu,
  server_menu,
  client_menu,
  server,
  client
};

enum class server_menu_state
{
  select_options,
  client_connect,
  deal_cards
};

enum class client_menu_state
{
  connecting,
  deal_cards
};


enum class client_state
{
  game,
  guess_cards
};



class drawer
{
  private:
		server_settings server_settings_ = {50, 5, 8585};
    game_state game_state_ = game_state::main_menu;
		server_menu_state server_menu_state_ = server_menu_state::select_options;
		Server *server;
/*    client_menu_state client_menu_state_ = client_menu_state::connecting;
    client_state client_state_ = client_state::game; */
    SDL_Window *window;
    SDL_Renderer* renderer;
    Timer timer;
    bool stop = false;
    EventHandler *e_handler;
    main_menu_options main_menu();
		int server_menu();
		int server_select_options();
		int server_client_connect();
    void showscreen();
  public:
    drawer();
    ~drawer();
    void loop();
};
