#include "game.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <stdbool.h>

#define WIDTH 1280
#define HEIGHT 720

#define PADDING 20

#define BALL_RADIUS 14

#define PADDLE_WIDTH 12
#define PADDLE_HEIGHT 84
#define PADDLE_SPEED 15

typedef struct {
    int x, y;   // position on screen
    int r;      // radius
    int dx, dy; // movement/ direction
} Ball;

typedef struct {
    int score_l; // left player score
    int score_r; // right player score
} Score;

typedef struct {
    int x, y;
    int h, w;
} Paddle;

void draw_paddle(SDL_Renderer *renderer, Paddle *paddle) {
    SDL_FRect p_rect = {paddle->x, paddle->y, paddle->w, paddle->h};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &p_rect);
}

void draw_ball(SDL_Renderer *renderer, Ball *ball) {
    SDL_FRect b_rect = {ball->x, ball->y, ball->r, ball->r};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &b_rect);
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

void handle_input(Paddle *paddle) {
    const bool *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W] && paddle[0].y > PADDING/2)
        paddle[0].y -= PADDLE_SPEED;
    if (keys[SDL_SCANCODE_UP] && paddle[1].y > PADDING/2)
        paddle[1].y -= PADDLE_SPEED;
    if (keys[SDL_SCANCODE_S] && paddle[0].y < HEIGHT - PADDLE_HEIGHT - PADDING/2)
        paddle[0].y += PADDLE_SPEED;
    if (keys[SDL_SCANCODE_DOWN] && paddle[1].y < HEIGHT - PADDLE_HEIGHT - PADDING/2)
        paddle[1].y += PADDLE_SPEED;
}

void reset_game(Ball *ball, Paddle *paddle, Score *score) {
    ball->x = WIDTH / 2;
    ball->y = HEIGHT / 2;

    ball->r = BALL_RADIUS;

    ball->dx = 1;
    ball->dy = 0;

    paddle[0].x = PADDING;
    paddle[0].y = (HEIGHT / 2) - PADDLE_HEIGHT;

    paddle[1].x = WIDTH - PADDLE_WIDTH - PADDING;
    paddle[1].y = (HEIGHT / 2) - PADDLE_HEIGHT;

    paddle[0].w = PADDLE_WIDTH;
    paddle[0].h = PADDLE_HEIGHT;
    paddle[1].w = PADDLE_WIDTH;
    paddle[1].h = PADDLE_HEIGHT;

    score->score_l = 0;
    score->score_r = 0;
}

int main() {
    Game game;

    if (!init(&game, "Pong", WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP))
        return -1;

    Ball ball = {0};
    Paddle paddle[2] = {0};
    DynamicText score_l = create_dynamic_text();
    DynamicText score_r = create_dynamic_text();
    DynamicText fps = create_dynamic_text();

    Score score = {0};

    reset_game(&ball, paddle, &score);

    game.running = true;
    while (game.running) {
        poll_events(&game);
        handle_input(paddle);

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        draw_paddle(game.renderer, &paddle[0]); // left paddle
        draw_ball(game.renderer, &ball);
        draw_paddle(game.renderer, &paddle[1]); // right paddle

        SDL_RenderPresent(game.renderer);
    }

    clean_dynamic_text(&score_l);
    clean_dynamic_text(&score_r);
    clean_dynamic_text(&fps);

    clean(&game);
    return 0;
}
