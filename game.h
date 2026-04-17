#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// Setup for all games
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    bool running;
} Game;

typedef struct {
    int prev_state;
    int current_state;
    SDL_Texture *texture;
    float width;
    float height;
} DynamicText;

// common setup for all games. sdl init, ttf init, create window and renderer
bool init(Game *game, const char *title, int w, int h, SDL_WindowFlags flags);
// destroy everything, cleanup
void clean(Game *game);

// Utils
// for displaying score, fps, etc...
DynamicText create_dynamic_text();
void render_dynamic_text(DynamicText *dt, char* prefix, float x, float y, Game* game, SDL_Color color);
void clean_dynamic_text(DynamicText *dt);

#endif
