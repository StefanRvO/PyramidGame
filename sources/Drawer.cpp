#include "../headers/Drawer.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include "../headers/Primitives.h"
#include <string>

drawer::drawer()
:timer(Timer(DRAWFPS))
{
  //Init SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0)
  {
      std::cout << "SDL_INIT Error: " << SDL_GetError() << std::endl;
      exit(0);
  }
  // Initialize SDL_ttf library
  if (TTF_Init() != 0)
  {
      std::cerr << "TTF_Init() Failed: " << TTF_GetError() << std::endl;
      SDL_Quit();
      TTF_Quit();
      exit(1);
  }

  //Create a window
  window = SDL_CreateWindow("Pyramid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      WINDOWSIZE_X, WINDOWSIZE_Y,
                      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(window == nullptr)
  {
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    TTF_Quit();
    exit(0);
  }
  //Create Renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(renderer == nullptr)
  {
    SDL_DestroyWindow(window);
    std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    TTF_Quit();
  }
  //Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "SDL error in PNG module!" << std::endl;
  }


  e_handler = new EventHandler(window, renderer, &stop, (void *)this);

}

drawer::~drawer()
{
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}


void drawer::loop()
{
  while(!stop)
  {
    switch(game_state_)
    {
      case game_state::main_menu:
      {
        auto decision = main_menu();
          if(decision == main_menu_options::none)
            break;
          else if(decision == main_menu_options::server)
            game_state_ = game_state::server_menu;
          else if(decision == main_menu_options::client)
            game_state_ = game_state::client_menu;
          else
            std::cout << "ERROR in main_menu." << std::endl;
        break;
      }
      case game_state::server_menu:
      {
        int decision = server_menu();
        if(decision == 1) // back
        {
          game_state_ = game_state::main_menu;
        }
        else if(decision == 2) // forward
        {
          game_state_ = game_state::server;
        }
        break;
      }
      case game_state::client_menu:

        break;
      case game_state::server:

        break;
      case game_state::client:

        break;
    }
  }
}

int drawer::server_menu()
{
  while(!stop)
  {
    switch (server_menu_state_)
    {
      case server_menu_state::select_options:
      {
        auto decision = server_select_options();
        if(decision == 1) // backward
          return 1;
        else if(decision == 2) // forward
        {
          server_menu_state_ = server_menu_state::client_connect;
          server = new Server(server_settings_);
        }
        break;
      }
      case server_menu_state::client_connect:
      {
        auto decision = server_client_connect();
        if(decision == 1) // forward
          server_menu_state_ = server_menu_state::deal_cards;
        else if(decision == 2) // backward
        {
          server_menu_state_ = server_menu_state::select_options;
          delete server;
        }
        break;
      }
      case server_menu_state::deal_cards:
      {/*
        auto success = server_deal_cards();
        if(success == 1) // forward
          return 1;
        else if(decision == 2) // backward
        {
          std::cout << "ERROR WHILE DEALING CARDS" << std::endl;
          return 2;
        }*/
      break;
      }
    }
  }
  return 0;
}

int drawer::server_select_options()
{
  static int *selected_box = NULL;
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  std::vector<SDL_Rect> boxes;
  std::string portString = std::to_string(server_settings_.port);
  std::string percentString = std::to_string(server_settings_.percent_flipped);
  std::string base_sizeString = std::to_string(server_settings_.base_size);
  TextDrawer TDrawer_small("FreeSans.ttf", h/40);
  TextDrawer TDrawer_verySmall("FreeSans.ttf", h/30);

  //show option selection
  SDL_SetRenderDrawColor(renderer,255,100,100,255);
  //port selection box
  boxes.push_back(TDrawer_small.DrawText(renderer, portString.c_str(), w/10 * 0.1, h/10 * 4.3, 100, 100, 100, 255, 1));

  //size selection box
  boxes.push_back(TDrawer_small.DrawText(renderer, percentString.c_str(), w/10 * .1, h/10 * 5.2, 100, 100, 100, 255, 1));

  //percentage selection box
  boxes.push_back(TDrawer_small.DrawText(renderer, base_sizeString.c_str(), w/10 * .1, h/10 * 6.4, 100, 100, 100, 255, 1));

  //backward
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Back", w/10 * 1, h/10 * 8, 100, 100, 100, 255, 1));

  //forward
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Next", w/10 * 9, h/10 * 8, 100, 100, 100, 255, 1));

  TDrawer_verySmall.DrawText(renderer, "port", w/10 * 0.1, h/10 * 3.7, 0, 0, 0, 255);
  TDrawer_verySmall.DrawText(renderer, "Percent of cards flipped", w/10 * 0.1, h/10 * 4.7, 0, 0, 0, 255);
  TDrawer_verySmall.DrawText(renderer, "Pyramid size", w/10 * 0.1, h/10 * 5.75, 0, 0, 0, 255);

  //this is pretty specialised, so we don't use the EventHandler.

  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
      switch (event.type)
      {
          case SDL_QUIT:
              stop = true;
              return -1;
          case SDL_MOUSEBUTTONDOWN:
          {
              int i = 0;
              bool inBox = 0;
              for(auto &rect : boxes)
              {
                int x = event.button.x;
                int y = event.button.y;
                if (isInsideBox(rect, x, y))
                {
                  if(i == 0)
                  {
                    selected_box = &server_settings_.port;
                  }
                  else if (i == 1)
                  {
                    selected_box = &server_settings_.percent_flipped;
                  }
                  else if (i == 2)
                  {
                    selected_box = &server_settings_.base_size;
                  }
                  else if (i == 3)
                    return 1;
                  else if(i == 4)
                    return 2;
                  inBox = 1;
                }
                i++;
              }
              if(!inBox)
              {
                selected_box = NULL;
              }
              break;
            }
            case SDL_KEYDOWN:
              switch(event.key.keysym.sym)
              {
                case SDLK_0:
                  *selected_box *=10;
                  *selected_box += 0;
                break;

                case SDLK_1:
                  *selected_box *=10;
                  *selected_box += 1;
                break;

                case SDLK_2:
                  *selected_box *=10;
                  *selected_box += 2;
                break;

                case SDLK_3:
                  *selected_box *=10;
                  *selected_box += 3;
                break;

                case SDLK_4:
                  *selected_box *=10;
                  *selected_box += 4;
                break;

                case SDLK_5:
                  *selected_box *=10;
                  *selected_box += 5;
                break;

                case SDLK_6:
                  *selected_box *=10;
                  *selected_box += 6;
                break;
                case SDLK_7:
                  *selected_box *=10;
                  *selected_box += 7;
                break;

                case SDLK_8:
                  *selected_box *=10;
                  *selected_box += 8;
                break;

                case SDLK_9:
                  *selected_box *=10;
                  *selected_box += 9;
                break;

                case SDLK_BACKSPACE:
                  *selected_box /=10;
                break;

              }
              break;
      }

  }
  if(server_settings_.port > 65535) server_settings_.port = 65535;
  if(server_settings_.percent_flipped > 100) server_settings_.percent_flipped = 100;
  if(server_settings_.base_size > 8) server_settings_.base_size = 8;
  showscreen();
  return 0;
}

int drawer::server_client_connect()
{
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  TextDrawer TDrawer_small("FreeSans.ttf", h/40);
  TDrawer_small.DrawTextCenter(renderer, "Waiting for clients", w/10 * 5, h/10 * 3, 100, 100, 100, 255);
  //make fancy animation with dots
  static int i = 2;
  static int direction = 1;
  if(i <= 1 || i >= 30)
  {
    direction *= -1;
  }
  i+= direction;
  std::string period_str;
  for (int j = 0; j < i; j++) period_str.push_back('.');
  TDrawer_small.DrawTextCenter(renderer, period_str.c_str(), w/10 * 5, h/10 * 4, 100, 100, 100, 255);
  SDL_SetRenderDrawColor(renderer,127,200,200,255);

  std::vector<SDL_Rect> boxes;
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Cancel", w/10 * 1, h/10 * 9, 100, 100, 100, 255,1));
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Start", w/10 * 9, h/10 * 9, 100, 100, 100, 255,1));

  int clickedBox = e_handler->detectClickInBox(boxes);
  showscreen();

  if(clickedBox == -1 )   return 0;
  if(clickedBox == 0 )   return 2;
  if(clickedBox == 1 )   return 1;
  else
  {
    std::cout << "ERROR in client connect click detection!!" << std::endl;
    return 0;
  }
}


main_menu_options drawer::main_menu()
{
  //Draw the main_menu
  std::vector<SDL_Rect> boxes;
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  TextDrawer TDrawer("FreeSans.ttf", w/10);
  TDrawer.DrawTextCenter(renderer, "Main Menu", w/2, h/10 * 3, 100, 100, 100, 255);
  TextDrawer TDrawer_small("FreeSans.ttf", w/15);

  SDL_SetRenderDrawColor(renderer,127,200,200,255);
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Server", w/10 * 3, h/10 * 6, 100, 100, 100, 255, 1));
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Client", w/10 * 7, h/10 * 6, 100, 100, 100, 255, 1));
  SDL_SetRenderDrawColor(renderer,255,100,100,255);
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Quit", w/10 * 5, h/10 * 8, 100, 100, 100, 255, 1));
  showscreen();
  int clickedBox = e_handler->detectClickInBox(boxes);
  if(clickedBox == -1 )   return main_menu_options::none;
  if(clickedBox == 0 )   return main_menu_options::server;
  if(clickedBox == 1 )   return main_menu_options::client;
  if(clickedBox == 2 )   return main_menu_options::quit;
  else
  {
    std::cout << "ERROR in main_menu click detection!!" << std::endl;
    return main_menu_options::quit;
  }
}


void drawer::showscreen()
{
  timer.tick();
  SDL_RenderPresent(renderer);
}
