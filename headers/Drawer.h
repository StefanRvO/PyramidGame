#pragma once
#include "Timer.h"
#include "TextDrawer.h"
#include "EventHandler.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <mutex>
#include "Server.h"
#include "Client.h"
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
};

enum class client_menu_state
{
  select_options,
	validate_hostname,
	connect,
	connected,
};

class drawer
{
  private:
		server_settings server_settings_ = {50, 5, 8585};

    game_state game_state_ = game_state::main_menu;
		server_menu_state server_menu_state_ = server_menu_state::select_options;
		Server *server = NULL;
		Client *client = NULL;
    client_menu_state client_menu_state_ = client_menu_state::select_options;
		client_settings client_settings_ = {8585, "localhost", "myNick", NULL};
    SDL_Window *window;
    SDL_Renderer* renderer;
    Timer timer;
    bool stop = false;
    EventHandler *e_handler;
    main_menu_options main_menu();
		int server_menu();
		int server_select_options();
		int server_client_connect();
		int client_validate_hostname();
		int client_menu();
		int client_select_options();
    void showscreen();
  public:
    drawer();
    ~drawer();
    void loop();
};
