#include "game.h"
#include <stdio.h>

#define WIDTH 1024
#define HEIGHT 768

#define GAP 64

#define TICK_SIZE 8
#define LABEL_OFFSET 12

#define X_BOXES (WIDTH / GAP)
#define Y_BOXES (HEIGHT / GAP)

typedef struct {
    float x;
    float y;
} Pos;

// needs to optimized by storing textures in an array and render those stored textures in the main loop
// the current implementation creates and destroys textures everytime
void add_text(SDL_Renderer *renderer, TTF_Font *font, char *text, Pos *pos) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, (SDL_Color){255, 255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FRect dest = {pos->x, pos->y, surface->w, surface->h};
    SDL_RenderTexture(renderer, texture, NULL, &dest);

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_primary_graph_lines(SDL_Renderer *renderer, TTF_Font *font) {
    int x = -X_BOXES;
    int y = Y_BOXES;

    // separate loops so we dont need to set render draw color every time
    //
    // render main graph lines, bright
    SDL_SetRenderDrawColor(renderer, 50, 180, 255, 255);
    for (int i = GAP; i < WIDTH; i += GAP)
        SDL_RenderLine(renderer, i, 0, i, HEIGHT);
    for (int i = GAP; i < HEIGHT; i += GAP)
        SDL_RenderLine(renderer, 0, i, WIDTH, i);

    // render ticks for scale, and show scale number with ticks
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = GAP; i < WIDTH; i += GAP) {
        SDL_RenderLine(renderer, i, HEIGHT * 0.5f - TICK_SIZE, i, HEIGHT * 0.5f + TICK_SIZE);

        // x scale number, -4 -2 0 2 4...
        Pos pos = {i - 6, HEIGHT * 0.5f + LABEL_OFFSET};
        char text[16];
        x += 2;
        sprintf(text, "%d", x);
        add_text(renderer, font, text, &pos);
    }
    for (int i = GAP; i < HEIGHT; i += GAP) {
        SDL_RenderLine(renderer, WIDTH * 0.5f - TICK_SIZE, i, WIDTH * 0.5f + TICK_SIZE, i);

        // y scale number, -4 -2 0 2 4...
        Pos pos = {WIDTH * 0.5f + LABEL_OFFSET, i - 6};
        char text[16];
        y -= 2;
        sprintf(text, "%d", y);
        add_text(renderer, font, text, &pos);
    }
}

void draw_secondary_graph_lines(SDL_Renderer *renderer) {
    // graph lines, darker color, less visible
    SDL_SetRenderDrawColor(renderer, 20, 30, 60, 255);
    for (int i = GAP / 2; i < WIDTH; i += GAP)
        SDL_RenderLine(renderer, i, 0, i, HEIGHT);
    for (int i = GAP / 2; i < HEIGHT; i += GAP)
        SDL_RenderLine(renderer, 0, i, WIDTH, i);

    // small ticks, for scale
    SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
    for (int i = GAP / 2; i < WIDTH; i += GAP)
        SDL_RenderLine(renderer, i, HEIGHT * 0.5f - TICK_SIZE * 0.5f, i, HEIGHT * 0.5f + TICK_SIZE * 0.5f);
    for (int i = GAP / 2; i < HEIGHT; i += GAP)
        SDL_RenderLine(renderer, WIDTH * 0.5f - TICK_SIZE * 0.5f, i, WIDTH * 0.5f + TICK_SIZE * 0.5f, i);
}

void parse(Pos *pos) {
    pos->x = WIDTH * 0.5f + (pos->x * GAP / 2);
    pos->y = HEIGHT * 0.5f - (pos->y * GAP / 2);
}

void draw_line(SDL_Renderer *renderer, Pos *origin, Pos *dest) {
    SDL_RenderLine(renderer, origin->x, origin->y, dest->x, dest->y);
}

int main() {
    // from game.h
    Game game = {0};

    if (!init(&game, "Graph", WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP))
        return -1;

    Pos origin0 = {0, 0};
    Pos dest1 = {2, 4};
    Pos dest2 = {6, -2};
    Pos sum = {8, 2};
    Pos sub = {-4, 6};
    parse(&origin0);
    parse(&dest1);
    parse(&dest2);

    parse(&sum);
    parse(&sub);

    bool running = true;
    // Main loop
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_EVENT_QUIT || (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE))
                running = false;

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        draw_secondary_graph_lines(game.renderer);
        draw_primary_graph_lines(game.renderer, game.font);

        SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 255);
        SDL_RenderLine(game.renderer, WIDTH * 0.5f, 0, WIDTH * 0.5f, HEIGHT);
        SDL_RenderLine(game.renderer, 0, HEIGHT * 0.5f, WIDTH, HEIGHT * 0.5f);

        SDL_SetRenderDrawColor(game.renderer, 255, 255, 0, 1);
        draw_line(game.renderer, &origin0, &dest1);

        SDL_SetRenderDrawColor(game.renderer, 255, 40, 225, 1);
        draw_line(game.renderer, &origin0, &dest2);

        SDL_SetRenderDrawColor(game.renderer, 20, 255, 20, 1);
        draw_line(game.renderer, &dest1, &sum);
        draw_line(game.renderer, &origin0, &sum);

        SDL_SetRenderDrawColor(game.renderer, 80, 80, 120, 1);
        draw_line(game.renderer, &dest1, &sub);
        draw_line(game.renderer, &origin0, &sub);

        SDL_RenderPresent(game.renderer);
    }

    clean(&game);
    return 0;
}
