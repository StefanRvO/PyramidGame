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
          else if(decision == main_menu_options::quit) return;
          else
            std::cout << "ERROR in main_menu." << std::endl;
        break;
      }
      case game_state::server_menu:
      {
        auto decision = server_menu();
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
      {
        auto decision = client_menu();
        if(decision == 1) //back
          game_state_ = game_state::main_menu;
        else if(decision == 2) //forward
          game_state_ = game_state::client;
        break;
      }
      case game_state::server:

        break;
      case game_state::client:

        break;
    }
  }
}

int drawer::client_menu()
{
  while(!stop)
  {
    switch (client_menu_state_)
    {
      case client_menu_state::select_options:
      {
        auto decision = client_select_options();
        if(decision == 1) // backward
          return 1;
        else if(decision == 2) // forward
        {
          client_menu_state_ = client_menu_state::validate_hostname;
        }
        break;
      }
      case client_menu_state::validate_hostname:
      {
        auto decision = client_validate_hostname();
        if(decision == 1) // forward
          client_menu_state_ = client_menu_state::connect;
        else if(decision == 2) // backward
        {
          client_menu_state_ = client_menu_state::select_options;
        }
        break;
      }

      case client_menu_state::connect:
      {
        client = new Client(client_settings_);
        if(client->send_hello())
        {
          int time_waited = 0;
          while(!client->isConnected())
          {
            time_waited += 5;
            usleep(5000);
            if(time_waited > 10000)
            {
              client_menu_state_ = client_menu_state::select_options;
              delete client;
            }
          }
          if(time_waited <= 10000)
            client_menu_state_ = client_menu_state::connected;
        }
        else
        {
          delete client;
          client_menu_state_ = client_menu_state::select_options;
        }
        break;
      }
      case client_menu_state::connected:
      {
        auto decision = client_connected();
        if(decision == 1) // forward
          {}
        else if(decision == 2) // backward
        {
          delete client;
          client_menu_state_ = client_menu_state::select_options;
        }
        break;
      }
    }
  }
  return 0;
}

int drawer::client_waiting_for_other_clients()
{
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  TextDrawer TDrawer_small("FreeSans.ttf", h/40);
  TDrawer_small.DrawTextCenter(renderer, "Connected", w/10 * 5, h/10 * 3, 100, 100, 100, 255);
  SDL_SetRenderDrawColor(renderer,127,200,200,255);

  std::vector<SDL_Rect> boxes;
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Cancel", w/10 * 1, h/10 * 9, 100, 100, 100, 255,1));
  auto ready_set_rect = TDrawer_small.DrawTextCenter(renderer, "I am ready: ", w/10 * 8, h/10 * 1, 100, 100, 100, 255);
  SDL_Rect ready_box;
  ready_box.x = ready_set_rect.x + ready_set_rect.w + w / 40;
  ready_box.w = w/30;
  ready_box.y = ready_set_rect.y;
  ready_box.h = ready_set_rect.h;
  SDL_DrawCrossBox(renderer, ready_box, client->isReady());
  boxes.push_back(ready_box);
  //print connected clients
  auto client_vector = client->getClientList();
  for(unsigned int j = 0; j < client_vector.size(); j++ )
  {
    auto T_rect = TDrawer_small.DrawText(renderer, client_vector[j].nick, w/10 * 0.5, h/10 * 2 + h/20 * j, 100 + j * 10, 200 - j * 10, 100, 255);
    SDL_Rect ready_rect;
    ready_rect.x = T_rect.x + T_rect.w + w / 40;
    ready_rect.w = w/30;
    ready_rect.y = T_rect.y;
    ready_rect.h = T_rect.h;
    SDL_DrawCrossBox(renderer, ready_rect, client_vector[j].ready);

  }
  int clickedBox = e_handler->detectClickInBox(boxes);
  showscreen();

  if(clickedBox == -1 )   return 0;
  if(clickedBox == 0 )   return 2;
  if(clickedBox == 1) client->set_ready(!client->isReady());
  return 0;
}

int drawer::client_connected()
{
  if(!client->isConnected()) return 2;
  switch(client->getState())
  {
    case client_state::waiting_for_start:
      client_waiting_for_other_clients();
    break;

    case client_state::get_cards:
    case client_state::game:
    case client_state::guess_cards:
    break;
  }
  return 0;
}
int drawer::client_validate_hostname()
{
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  TextDrawer TDrawer_small("FreeSans.ttf", h/40);
  TDrawer_small.DrawText(renderer, "validating hostname...", w/10 * 0.1, h/10 * 3.7, 0, 0, 0, 255);
  showscreen();
  if((client_settings_.he = gethostbyname(client_settings_.server_adress.c_str())) == NULL)
  {
    return 2;
  }
  return 1;

}
int drawer::client_select_options()
{

  static void *selected_box = NULL;
  SDL_SetRenderDrawColor(renderer,255,255,255,255);
  SDL_RenderClear(renderer);
  int w, h;
  SDL_GetWindowSize(window,&w,&h);
  TextDrawer TDrawer_small("FreeSans.ttf", h/40);
  TextDrawer TDrawer_verySmall("FreeSans.ttf", h/30);

  std::string port_str = std::to_string(client_settings_.port);
  std::vector<SDL_Rect> boxes;
  SDL_SetRenderDrawColor(renderer,255,100,100,255);

  TDrawer_verySmall.DrawText(renderer, "port", w/10 * 0.1, h/10 * 3.7, 0, 0, 0, 255);
  TDrawer_verySmall.DrawText(renderer, "server adress / domain", w/10 * 0.1, h/10 * 4.7, 0, 0, 0, 255);
  TDrawer_verySmall.DrawText(renderer, "Nick_name", w/10 * 0.1, h/10 * 5.75, 0, 0, 0, 255);

  boxes.push_back(TDrawer_small.DrawText(renderer, port_str.c_str(), w/10 * 0.1, h/10 * 4.3, 100, 100, 100, 255, 1));

  //size selection box
  boxes.push_back(TDrawer_small.DrawText(renderer, client_settings_.server_adress.c_str(), w/10 * .1, h/10 * 5.2, 100, 100, 100, 255, 1));

  //percentage selection box
  boxes.push_back(TDrawer_small.DrawText(renderer, client_settings_.nick_name.c_str(), w/10 * .1, h/10 * 6.4, 100, 100, 100, 255, 1));

  //backward
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Back", w/10 * 1, h/10 * 8, 100, 100, 100, 255, 1));

  //forward
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Connect", w/10 * 9, h/10 * 8, 100, 100, 100, 255, 1));

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
                    selected_box = &client_settings_.port;
                    SDL_StopTextInput();
                  }
                  else if (i == 1)
                  {
                    selected_box = &client_settings_.server_adress;
                    SDL_StartTextInput();
                  }
                  else if (i == 2)
                  {
                    selected_box = &client_settings_.nick_name;
                    SDL_StartTextInput();
                  }
                  else if (i == 3)
                  {
                    SDL_StopTextInput();
                    return 1;
                  }
                  else if(i == 4)
                  {
                    SDL_StopTextInput();
                    return 2;
                  }
                  inBox = 1;
                }
                i++;
              }
              if(!inBox)
              {
                selected_box = NULL;
                SDL_StopTextInput();
              }
              break;
            }
            case SDL_KEYDOWN:
                if(event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9')
                {
                  if(selected_box == &client_settings_.port)
                  {
                    client_settings_.port *= 10;
                    client_settings_.port += event.key.keysym.sym - '0';
                  }
                }
                else if(event.key.keysym.sym == SDLK_BACKSPACE)
                {
                  if(selected_box ==  &client_settings_.port)
                    client_settings_.port /=10;
                  else if(selected_box == &client_settings_.server_adress || selected_box == &client_settings_.nick_name)
                  {
                    if(((std::string *)selected_box)->size())
                      ((std::string *)selected_box)->pop_back();
                  }
                }

            break;

            case SDL_TEXTINPUT:
              if(selected_box == &client_settings_.server_adress || selected_box == &client_settings_.nick_name)
              {
                ((std::string *)selected_box)->append(event.text.text);
              }
            break;
            case SDL_TEXTEDITING:
              if(selected_box == &client_settings_.server_adress || selected_box == &client_settings_.nick_name)
              {
                ((std::string *)selected_box)->append(event.edit.text);
              }
            break;
            }
    }
    showscreen();
    return 0;
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
        {
          server_menu_state_ = server_menu_state::game_in_progress;
          server->setState(server_state::deal_cards);
        }
        else if(decision == 2) // backward
        {
          server_menu_state_ = server_menu_state::select_options;
          delete server;
        }
        break;
      }
      case server_menu_state::game_in_progress:
      {
        auto decision = server_game_in_progress();
        if(decision == 1) // forward
          {}
        else if(decision == 2) // backward
        {
          server_menu_state_ = server_menu_state::select_options;
          delete server;
        }
        break;
      }
    }
  }
  return 0;
}

int drawer::server_game_in_progress()
{
  switch(server->getState())
  {
    case server_state::get_clients:
      //nothing to show here.. handled elsewhere
    break;

    case server_state::deal_cards:
    //nothing to show here.. handled elsewhere

    break;
    case server_state::flip_card:

    break;
    case server_state::give_drinks:

    break;
    case server_state::trust_drinks:

    break;
    case server_state::give_new_cards:

    break;
    case server_state::drink:

    break;
    case server_state::guess_cards:

    break;
    case server_state::wait_for_ready_to_flip:
      //std::cout << "in wait state" << std::endl;
    break;
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
              if(event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9')
              {
                *selected_box *=10;
                *selected_box += event.key.keysym.sym - '0';
              }
              else if(event.key.keysym.sym == SDLK_BACKSPACE)
                *selected_box /=10;
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

  bool allready = true;
  //print connected clients
  auto client_vector = server->getClientList();
  for(unsigned int j = 0; j < client_vector.size(); j++ )
  {
    auto T_rect = TDrawer_small.DrawText(renderer, client_vector[j].nick, w/10 * 0.5, h/10 * 2 + h/20 * j, 100 + j * 10, 200 - j * 10, 100, 255);
    SDL_Rect ready_rect;
    ready_rect.x = T_rect.x + T_rect.w + w / 40;
    ready_rect.w = w/30;
    ready_rect.y = T_rect.y;
    ready_rect.h = T_rect.h;
    if(!client_vector[j].ready) allready = false;
    SDL_DrawCrossBox(renderer, ready_rect, client_vector[j].ready);
  }

  std::vector<SDL_Rect> boxes;
  boxes.push_back(TDrawer_small.DrawTextCenter(renderer, "Cancel", w/10 * 1, h/10 * 9, 100, 100, 100, 255,1));
  if(allready && client_vector.size() >= 2)
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
