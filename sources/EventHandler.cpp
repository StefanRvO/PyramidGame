#include "../headers/EventHandler.h"
#include "../headers/Drawer.h"
#include <iostream>
static drawer *drawer_ptr = nullptr; //crappy, i know.

EventHandler::EventHandler(SDL_Window *window_, SDL_Renderer* renderer_, bool *glob_stop_, void *drawer_)
{
    drawer_ptr = (drawer *)drawer_;
    window=window_;
    renderer=renderer_;
    glob_stop = glob_stop_;
}

void EventHandler::stateHandler(__attribute__((unused)) EventHandler_state Estate)
{
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    //Keyboard
    //int lenght;
    //const Uint8* keyState=SDL_GetKeyboardState(&lenght);
    //Mouse
    int xpos,ypos;
    if(SDL_GetMouseState(&xpos,&ypos) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
    }
}

void EventHandler::handleEvents(__attribute__((unused)) EventHandler_state Estate)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                *glob_stop = true;
                break;
            case SDL_KEYDOWN:
                handleKeyDownEvent(event, Estate);
                break;
            case SDL_KEYUP:
                handleKeyUpEvent(event, Estate);
                break;
            case SDL_WINDOWEVENT:
                handleWindowEvent(event, Estate);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleMouseDownEvent(event, Estate);
                break;
        }
    }
}

void EventHandler::handleKeyDownEvent(__attribute__((unused)) SDL_Event &event, __attribute__((unused)) EventHandler_state Estate)
{

}

void EventHandler::handleKeyUpEvent(__attribute__((unused)) SDL_Event &event, __attribute__((unused)) EventHandler_state Estate)
{

}
void EventHandler::handleMouseDownEvent(__attribute__((unused)) SDL_Event &event, __attribute__((unused)) EventHandler_state Estate)
{

}
void EventHandler::handleWindowEvent(__attribute__((unused)) SDL_Event &event, __attribute__((unused)) EventHandler_state Estate)
{

}

int EventHandler::detectClickInBox(std::vector<SDL_Rect> &rects) // check if the user has clicked inside one of these boxes
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
      switch (event.type)
      {
          case SDL_QUIT:
              *glob_stop = true;
              return -1;
          case SDL_MOUSEBUTTONDOWN:
              int i = 0;
              for(auto &rect : rects)
              {
                int x = event.button.x;
                int y = event.button.y;
                if (isInsideBox(rect, x, y))
                  return i;
                i++;
              }
              break;
      }
  }
  return -1;
}

bool isInsideBox(SDL_Rect &rect, int x, int y)
{
  return (x >= rect.x && x <= rect.x+ rect.w && y > rect.y && y < rect.y + rect.h);
}
