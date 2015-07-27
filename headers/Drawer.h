#pragma once
#include "Timer.h"
#include "TextDrawer.h"
#include "EventHandler.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <mutex>
#define WINDOWSIZE_X 750
#define WINDOWSIZE_Y 600
#define DRAWFPS 20
class drawer
{
  private:
    SDL_Window *window;
    SDL_Renderer* renderer;
    Timer timer;
    bool stop = false;
    EventHandler *e_handler;
  public:
    drawer();
    ~drawer();
    void loop();
};

#pragma once

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

enum class server_state
{
  game,
  guess_cards
};

enum class client_state
{
  game,
  guess_cards
};
