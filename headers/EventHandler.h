#pragma once
#include <SDL2/SDL.h>
#include "Timer.h"
#define ANGLE_TO_PANTILT ((270 * 4) / 360)

#define SPEED 2000. //changes per second when holding arrow keys
#define CHANGE (SPEED / FPS)
class EventHandler
{
    private:
        SDL_Window *window;
        bool *glob_stop;
        SDL_Renderer* renderer;
    public:
        EventHandler(SDL_Window *window_, SDL_Renderer* renderer_, bool *glob_stop_);
        void handleEvents();
        void stateHandler();
    private:
        void handleKeyUpEvent(SDL_Event &event);
        void handleKeyDownEvent(SDL_Event &event);
        void handleMouseDownEvent(SDL_Event &event);
        void handleWindowEvent(SDL_Event &event);
};
