#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    bool running;
} Game;

bool init(Game* game, const char *title, int w, int h, SDL_WindowFlags flags);

void clean(Game* game);

#endif
