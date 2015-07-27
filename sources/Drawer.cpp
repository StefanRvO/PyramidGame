#include "../headers/Drawer.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include "../headers/Primitives.h"

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

  //load textures for the cards
  /*
  	piece_textures[0][0] = IMG_LoadTexture(renderer, "images/king_black.png");
  	piece_textures[0][1] = IMG_LoadTexture(renderer, "images/knight_black.png");
  	piece_textures[0][2] = IMG_LoadTexture(renderer, "images/pawn_black.png");
  	piece_textures[0][3] = IMG_LoadTexture(renderer, "images/queen_black.png");
  	piece_textures[0][4] = IMG_LoadTexture(renderer, "images/rook_black.png");
    piece_textures[0][5] = IMG_LoadTexture(renderer, "images/bishop_black.png");
  	piece_textures[1][0] = IMG_LoadTexture(renderer, "images/king_white.png");
  	piece_textures[1][1] = IMG_LoadTexture(renderer, "images/knight_white.png");
  	piece_textures[1][2] = IMG_LoadTexture(renderer, "images/pawn_white.png");
  	piece_textures[1][3] = IMG_LoadTexture(renderer, "images/queen_white.png");
  	piece_textures[1][4] = IMG_LoadTexture(renderer, "images/rook_white.png");
    piece_textures[1][5] = IMG_LoadTexture(renderer, "images/bishop_white.png");
*/
  e_handler = new EventHandler(window, renderer, &stop);

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
    timer.tick();
    SDL_Event event; //grab events
    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          return;
          break;
      }
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
}
