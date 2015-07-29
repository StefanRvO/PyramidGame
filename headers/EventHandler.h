#pragma once
#include <SDL2/SDL.h>
#include "Timer.h"
#include <vector>
enum class EventHandler_state
{
  main_menu,

};


class EventHandler
{
    private:
        SDL_Window *window;
        bool *glob_stop;
        SDL_Renderer* renderer;
    public:
        EventHandler(SDL_Window *window_, SDL_Renderer* renderer_, bool *glob_stop_, void *drawer_);
        void handleEvents(EventHandler_state Estate);
        void stateHandler(EventHandler_state Estate);
        int detectClickInBox(std::vector<SDL_Rect> &rects); // check if the user has clicked inside one of these boxes
        void NumberInput(std::string &str, int max);
    private:
        void handleKeyUpEvent(SDL_Event &event, EventHandler_state Estate);
        void handleKeyDownEvent(SDL_Event &event, EventHandler_state Estate);
        void handleMouseDownEvent(SDL_Event &event, EventHandler_state Estate);
        void handleWindowEvent(SDL_Event &event, EventHandler_state Estate);
};

bool isInsideBox(SDL_Rect &rect, int x, int y);
