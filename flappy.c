#include "game.h"
#include <SDL3/SDL.h>

#define WIDTH 1280
#define HEIGHT 720

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

    game.running = true;

    while (game.running) {
        poll_events(&game);

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);
        SDL_RenderPresent(game.renderer);
    }

    clean_dynamic_text(&score);

    clean(&game);
    return 0;
}
