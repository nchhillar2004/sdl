#include "game.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

#define GC (SDL_Color){0, 100, 0, 255}
#define CELL_SIZE 25

#define CELLS_X WIDTH / CELL_SIZE
#define CELLS_Y HEIGHT / CELL_SIZE

void fill_cell(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0, 160, 0, 255);
    SDL_FRect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_RenderFillRect(renderer, &cell);
}

void draw_pillar(SDL_Renderer *renderer, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 0, 160, 0, 255);
    SDL_FRect dst = {x * CELL_SIZE, y * CELL_SIZE, w * CELL_SIZE, h * CELL_SIZE};
    SDL_RenderFillRect(renderer, &dst);
}

void draw_bird(SDL_Renderer *renderer) {
    fill_cell(renderer, CELLS_X / 3 - 1, CELLS_Y / 2 - 1);
}

void draw_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, GC.r, GC.g, GC.b, GC.a);
    for (int x = 0; x <= WIDTH; x += CELL_SIZE)
        SDL_RenderLine(renderer, x, 0, x, HEIGHT);
    for (int y = 0; y <= HEIGHT; y += CELL_SIZE)
        SDL_RenderLine(renderer, 0, y, WIDTH, y);
}

void poll_events(Game *game) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
            game->running = false;
        if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
            game->running = false;
    }
}

int main() {
    Game game;

    if (!init(&game, "Flappy bird", WIDTH, HEIGHT, 0))
        return -1;

    DynamicText score = create_dynamic_text();

    SDL_DisplayID id = SDL_GetDisplayForWindow(game.window);
    SDL_Rect dis;
    SDL_GetDisplayBounds(id, &dis);

    if (!SDL_SetWindowPosition(game.window, dis.w - WIDTH, 0))
        printf("[WARN] unable to set window pos: %s\n", SDL_GetError());

    game.running = true;

    while (game.running) {
        poll_events(&game);

        SDL_SetRenderDrawColor(game.renderer, 130, 200, 228, 255);
        SDL_RenderClear(game.renderer);

        draw_grid(game.renderer);

        draw_bird(game.renderer);

        draw_pillar(game.renderer, 18, 0, 4, 8);

        SDL_RenderPresent(game.renderer);
    }

    clean_dynamic_text(&score);

    clean(&game);
    return 0;
}
