#include "game.h"
#include <stdio.h>

// common setup for SDL3 and SDL3_ttf
bool init(Game* game, const char *title, int w, int h, SDL_WindowFlags flags) {
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("[ERROR] SDL_Init: %s", SDL_GetError());
        return false;
    }
    if (!TTF_Init()) {
        SDL_Log("[ERROR] TTF_Init: %s", SDL_GetError());
        return false;
    }
    game->window = SDL_CreateWindow(title, w, h, flags);
    if (!game->window) {
        SDL_Log("[ERROR] SDL_CreateWindow: %s", SDL_GetError());
        return false;
    }
    if (!SDL_SetWindowPosition(game->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED))
        SDL_Log("[WARNING] SDL_SetWindowPosition: %s", SDL_GetError());

    SDL_SetWindowResizable(game->window, false);

    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        SDL_Log("[ERROR] SDL_CreateRenderer: %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderVSync(game->renderer, true);

    game->font = TTF_OpenFont("res/font.ttf", 16);
    if (!game->font) {
        SDL_Log("[ERROR] TTF_OpenFont: %s", SDL_GetError());
        return false;
    }

    SDL_Log("[SUCCESS] Init done.");

    return true;
}

void clean(Game* game) {
    if (game->font) {
        TTF_CloseFont(game->font);
        game->font = NULL;
    }

    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
        game->renderer = NULL;
    }

    if (game->window) {
        SDL_DestroyWindow(game->window);
        game->window = NULL;
    }

    TTF_Quit();
    SDL_Quit();

    printf("[SUCCESS] All clean.\n");
    return;
}
