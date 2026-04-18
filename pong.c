#include "game.h"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#define WIDTH 1200
#define HEIGHT 800

#define PADDING 25

#define BORDER_BREADTH PADDING

#define BALL_SIZE 24

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 120
#define PADDLE_SPEED 15

#define WHITE_COLOR (SDL_Color){255, 255, 255, 255}
#define GREY_COLOR (SDL_Color){180, 180, 180, 255}
#define LIGHT_GREY_COLOR (SDL_Color){80, 80, 80, 255}
#define GREEN_COLOR (SDL_Color){0, 80, 0, 255}

#define DOTTED_RECT_WIDTH 12
#define DOTTED_RECT_HEIGHT 70
#define DOTTED_RECT_GAP 30

typedef struct {
    int x, y;   // position on screen
    int s;      // side - it's a square
    int dx, dy; // movement/ direction
} Ball;

typedef struct {
    int x, y;
    int h, w;
} Paddle;

void draw_rect(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void ball_collision(Ball *ball) {
}

void move_ball(SDL_Renderer *renderer, Ball *ball) {
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

    if (keys[SDL_SCANCODE_W] && paddle[0].y > BORDER_BREADTH)
        paddle[0].y -= PADDLE_SPEED;
    if (keys[SDL_SCANCODE_UP] && paddle[1].y > BORDER_BREADTH)
        paddle[1].y -= PADDLE_SPEED;
    if (keys[SDL_SCANCODE_S] && paddle[0].y < HEIGHT - PADDLE_HEIGHT - BORDER_BREADTH)
        paddle[0].y += PADDLE_SPEED;
    if (keys[SDL_SCANCODE_DOWN] && paddle[1].y < HEIGHT - PADDLE_HEIGHT - BORDER_BREADTH)
        paddle[1].y += PADDLE_SPEED;
}

void reset_game(Ball *ball, Paddle *paddle, int *score_l, int *score_r) {
    ball->x = WIDTH / 2 - BALL_SIZE / 2;
    ball->y = HEIGHT / 2 - BALL_SIZE / 2;

    ball->s = BALL_SIZE;

    // by default ball will go right after start
    ball->dx = 1;
    ball->dy = 0;

    paddle[0].x = PADDING;
    paddle[0].y = (HEIGHT / 2) - PADDLE_HEIGHT / 2;

    paddle[1].x = WIDTH - PADDLE_WIDTH - PADDING;
    paddle[1].y = (HEIGHT / 2) - PADDLE_HEIGHT / 2;

    paddle[0].w = PADDLE_WIDTH;
    paddle[0].h = PADDLE_HEIGHT;
    paddle[1].w = PADDLE_WIDTH;
    paddle[1].h = PADDLE_HEIGHT;

    *score_l = 0;
    *score_r = 0;
}

int main() {
    Game game;

    if (!init(&game, "Pong", WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP))
        return -1;

    Ball ball = {0};
    Paddle paddle[2] = {0};
    DynamicText score_l = create_dynamic_text();
    DynamicText score_r = create_dynamic_text();
    TTF_SetFontSizeDPI(game.font, 24, 0, 0);

    reset_game(&ball, paddle, &score_l.current_state, &score_r.current_state);

    game.running = true;
    while (game.running) {
        poll_events(&game);
        handle_input(paddle);

        // update score
        if (ball.x <= DOTTED_RECT_WIDTH)
            score_l.current_state += 1;
        if (ball.x >= WIDTH - DOTTED_RECT_WIDTH)
            score_r.current_state += 1;

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        for (int i = 0; i < HEIGHT; i += DOTTED_RECT_HEIGHT + DOTTED_RECT_GAP) {
            draw_rect(game.renderer, 0, i, DOTTED_RECT_WIDTH, DOTTED_RECT_HEIGHT, GREEN_COLOR);
            draw_rect(game.renderer, (WIDTH / 2) - (DOTTED_RECT_WIDTH / 2), i, DOTTED_RECT_WIDTH, DOTTED_RECT_HEIGHT,
                      LIGHT_GREY_COLOR);
            draw_rect(game.renderer, WIDTH - DOTTED_RECT_WIDTH, i, DOTTED_RECT_WIDTH, DOTTED_RECT_HEIGHT, GREEN_COLOR);
        }

        // draw up/ down border
        draw_rect(game.renderer, 0, 0, WIDTH, BORDER_BREADTH, GREY_COLOR);
        draw_rect(game.renderer, 0, HEIGHT - BORDER_BREADTH, WIDTH, BORDER_BREADTH, GREY_COLOR);

        // draw dotted line in middle

        draw_rect(game.renderer, paddle[0].x, paddle[0].y, paddle[0].w, paddle[0].h, WHITE_COLOR); // left paddle
        draw_rect(game.renderer, paddle[1].x, paddle[1].y, paddle[1].w, paddle[1].h, WHITE_COLOR); // right paddle
        draw_rect(game.renderer, ball.x, ball.y, ball.s, ball.s, WHITE_COLOR);                     // draw pong ball

        render_dynamic_text(&score_l, "Player 1", (float)WIDTH / 4 - score_l.width / 2, BORDER_BREADTH + PADDING, &game,
                            GREY_COLOR);
        render_dynamic_text(&score_r, "Player 2", (float)(WIDTH - (float)WIDTH / 4) - score_r.width / 2,
                            BORDER_BREADTH + PADDING, &game, GREY_COLOR);

        SDL_RenderPresent(game.renderer);
    }

    clean_dynamic_text(&score_l);
    clean_dynamic_text(&score_r);

    clean(&game);
    return 0;
}
