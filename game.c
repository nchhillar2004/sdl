#include "game.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

// common setup for SDL3 and SDL3_ttf
bool init(Game *game, const char *title, int w, int h, SDL_WindowFlags flags) {
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
    if (game->window == NULL) {
        SDL_Log("[ERROR] SDL_CreateWindow: %s", SDL_GetError());
        return false;
    }
    if (!SDL_SetWindowPosition(game->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED))
        SDL_Log("[WARNING] SDL_SetWindowPosition: %s", SDL_GetError());

    SDL_SetWindowResizable(game->window, false);

    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (game->renderer == NULL) {
        SDL_Log("[ERROR] SDL_CreateRenderer: %s", SDL_GetError());
        return false;
    }

    SDL_SetRenderVSync(game->renderer, true);

    game->font = TTF_OpenFont("res/font.ttf", 16);
    if (game->font == NULL) {
        SDL_Log("[ERROR] TTF_OpenFont: %s", SDL_GetError());
        return false;
    }

    SDL_Log("[SUCCESS] Init done.");

    return true;
}

void clean(Game *game) {
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

DynamicText create_dynamic_text() {
    return (DynamicText){
        .prev_state = -1,
        .current_state = 0,
        .texture = NULL,
        .width = 0,
        .height = 0
    };
}

void render_dynamic_text(DynamicText *dt, char *prefix, float x, float y, Game *game, SDL_Color color) {
    char buffer[64];
    // update text only if it has changed
    if (dt->current_state != dt->prev_state || !dt->texture) {
        // destroy previous texture if there is a change in current and prev state
        if (dt->texture)
            SDL_DestroyTexture(dt->texture);
        snprintf(buffer, sizeof(buffer), "%s: %d", prefix, dt->current_state);
        SDL_Surface *surface = TTF_RenderText_Solid(game->font, buffer, 0, color);
        if (!surface) {
            SDL_Log("[ERROR] TTF_RenderText_Solid: %s", SDL_GetError());
            return;
        }

        dt->width = surface->w;
        dt->height = surface->h;

        dt->texture = SDL_CreateTextureFromSurface(game->renderer, surface);
        dt->prev_state = dt->current_state;

        SDL_DestroySurface(surface);
    }
    SDL_FRect dst = {x, y, dt->width, dt->height};
    SDL_RenderTexture(game->renderer, dt->texture, NULL, &dst);
}

void clean_dynamic_text(DynamicText *dt) {
    if (dt->texture) {
        SDL_DestroyTexture(dt->texture);
        dt->texture = NULL;
    }
}
