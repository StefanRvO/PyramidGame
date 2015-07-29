#include "../headers/TextDrawer.h"
#include <iostream>

TextDrawer::TextDrawer(const char* fontpath,int fontsize)
{
    font=TTF_OpenFont(fontpath,fontsize);
}
TextDrawer::~TextDrawer()
{
    TTF_CloseFont(font);
}

SDL_Rect TextDrawer::DrawText(SDL_Renderer *render,const char* text,int x,int y,int r,int g,int b, int a, bool boxed)
{
    SDL_Color color;
    color.r=r;
    color.g=g;
    color.b=b;
    color.a=a;
    SDL_Rect rect;
    rect.x=x;
    rect.y=y;
    int x_ = 0;
    int y_ = 0;
    TTF_SizeUTF8(font,text,&rect.w,&rect.h);
    SDL_Surface *textSurface=TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *textTexture=SDL_CreateTextureFromSurface(render, textSurface);
    SDL_QueryTexture(textTexture, NULL, NULL, &x_, &y_);
    SDL_Rect return_rect = rect;
    return_rect.w = x_;
    return_rect.h = y_;
    if (boxed)
    {
      SDL_RenderFillRect(render, &return_rect);
    }
    SDL_RenderCopy(render,textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return return_rect;
}

SDL_Rect TextDrawer::DrawTextCenter(SDL_Renderer *render,const char* text,int x,int y,int r,int g,int b, int a, bool boxed)
{
    SDL_Color color;
    color.r=r;
    color.g=g;
    color.b=b;
    color.a=a;
    SDL_Rect rect;
    int x_ = 0;
    int y_ = 0;
    TTF_SizeUTF8(font,text,&rect.w,&rect.h);
    SDL_Surface *textSurface=TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *textTexture=SDL_CreateTextureFromSurface(render, textSurface);
    SDL_QueryTexture(textTexture, NULL, NULL, &x_, &y_);
    rect.x = x - x_/2;
    rect.y = y - y_/2;
    SDL_Rect return_rect = rect;
    return_rect.w = x_;
    return_rect.h = y_;
    if (boxed)
    {
      SDL_RenderFillRect(render, &return_rect);
    }
    SDL_RenderCopy(render,textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return return_rect;
}
