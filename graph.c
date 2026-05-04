#include "game.h"
#include <stdio.h>

#define WIDTH 1024
#define HEIGHT 768

#define GAP 64

#define X_BOXES WIDTH/GAP
#define Y_BOXES HEIGHT/GAP

typedef struct{
    float x;
    float y;
} Pos;

void add_text(SDL_Renderer* renderer, TTF_Font* font, char* text, Pos *pos) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, 0, (SDL_Color){255, 255, 255, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FRect dest = {pos->x, pos->y, surface->w, surface->h};
    SDL_RenderTexture(renderer, texture, NULL, &dest);

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_graph(SDL_Renderer* renderer, TTF_Font* font) {
    int x = -X_BOXES;
    int y = Y_BOXES;
    for (int i = 64; i < WIDTH; i += GAP) { 
        SDL_SetRenderDrawColor(renderer, 50, 180, 255, 255);
        SDL_RenderLine(renderer, i, 0, i, HEIGHT);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, i, (float)HEIGHT/2-8, i, (float)HEIGHT/2+8);

        Pos pos = {i-6, (float)HEIGHT/2+12};
        char text[4];
        x += 2;
        sprintf(text, "%d", x);
        add_text(renderer, font, text, &pos);
    }
    for (int i = 64; i < HEIGHT; i += GAP) {
        SDL_SetRenderDrawColor(renderer, 50, 180, 255, 255);
        SDL_RenderLine(renderer, 0, i, WIDTH, i);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, (float)WIDTH/2-8, i, (float)WIDTH/2+8, i);

        Pos pos = {(float)WIDTH/2+12, i-6};
        char text[4];
        y -= 2;
        sprintf(text, "%d", y);
        add_text(renderer, font, text, &pos);
    }
}

void draw_mini_graph(SDL_Renderer* renderer) {
    for (int i = GAP/2; i < WIDTH; i += GAP) {
        SDL_SetRenderDrawColor(renderer, 20, 30, 60, 255);
        SDL_RenderLine(renderer, i, 0, i, HEIGHT);

        SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
        SDL_RenderLine(renderer, i, (float)HEIGHT/2-4, i, (float)HEIGHT/2+4);
    }
    for (int i = GAP/2; i < HEIGHT; i += GAP) {
        SDL_SetRenderDrawColor(renderer, 20, 30, 60, 255);
        SDL_RenderLine(renderer, 0, i, WIDTH, i);

        SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
        SDL_RenderLine(renderer, (float)WIDTH/2-4, i, (float)WIDTH/2+4, i);
    }
}

void parse(Pos *pos) {
    pos->x = (float)WIDTH/2 + (pos->x * GAP/2);
    pos->y = (float)HEIGHT/2 - (pos->y * GAP/2);
}

void draw_line(SDL_Renderer* renderer, Pos *origin, Pos *dest) {
    SDL_RenderLine(renderer, origin->x, origin->y, dest->x, dest->y); 
}

int main() {
    Game game = {0};

    if (!init(&game, "Graph", WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP))
        return -1;

    Pos origin0 = {0, 0};
    Pos dest1 = {2, 4};
    Pos dest2 = {6, -2};
    Pos dest3 = {8, 2};
    parse(&origin0);
    parse(&dest1);
    parse(&dest2);
    parse(&dest3);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) 
            if (e.type == SDL_EVENT_QUIT || (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE))
                running = false;

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        draw_mini_graph(game.renderer);
        draw_graph(game.renderer, game.font);

        SDL_SetRenderDrawColor(game.renderer, 255, 255, 255, 255);
        SDL_RenderLine(game.renderer, (float)WIDTH/2, 0, (float)WIDTH/2, HEIGHT);
        SDL_RenderLine(game.renderer, 0, (float)HEIGHT/2, WIDTH, (float)HEIGHT/2);

        SDL_SetRenderDrawColor(game.renderer, 255, 255, 0, 1);
        draw_line(game.renderer, &origin0, &dest1);
        
        SDL_SetRenderDrawColor(game.renderer, 255, 40, 225, 1);
        draw_line(game.renderer, &origin0, &dest2);
        
        SDL_SetRenderDrawColor(game.renderer, 20, 255, 20, 1);
        draw_line(game.renderer, &dest1, &dest3);

        SDL_RenderPresent(game.renderer);
    }

    clean(&game);
    return 0;
}
