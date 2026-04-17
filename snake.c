#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

#define WIDTH 1250
#define HEIGHT 750

#define CELL_SIZE 25
#define LINE_WIDTH 1
#define CELLS_X (WIDTH / CELL_SIZE)  // number of cells in one row
#define CELLS_Y (HEIGHT / CELL_SIZE) // number of cells in one col
#define TOTAL_CELLS (CELLS_X * CELLS_Y)
#define TEXT_COLOR (SDL_Color){200, 200, 200, 255}
#define WHITE_COLOR (SDL_Color){255, 255, 255, 255}
#define RED_COLOR (SDL_Color){0, 255, 255, 255}
#define BLACK_COLOR (SDL_Color){0, 0, 0, 255}
// grid color (SDL_Color)
#define GC (SDL_Color){16, 16, 16, 255}

#define DEFAULT_SNAKE_SPEED 100
#define FAST_SNAKE_SPEED 80

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    // body[0] = head of snake
    Position body[TOTAL_CELLS]; // max snake length = total cells in grid
    // body[length - 1] = tail
    int length; // current snake length

    // direction snake is facing
    int dx;
    int dy;
} Snake;

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, float *w,
                                 float *h) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, color);
    if (!surface)
        return NULL;

    *w = surface->w;
    *h = surface->h;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

/* OLD
void draw_grid(SDL_Surface *surface) {
    SDL_Rect row_line = {0, 0, WIDTH, LINE_WIDTH};
    for (row_line.y = 0; row_line.y <= HEIGHT; row_line.y += CELL_SIZE)
        SDL_FillSurfaceRect(surface, &row_line, GRID_COLOR);

    SDL_Rect col_line = {0, 0, LINE_WIDTH, HEIGHT};
    for (col_line.x = 0; col_line.x <= WIDTH; col_line.x += CELL_SIZE)
        SDL_FillSurfaceRect(surface, &col_line, GRID_COLOR);
}*/

// NEW
void draw_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, GC.r, GC.g, GC.b, GC.a);
    for (int x = 0; x <= WIDTH; x += CELL_SIZE)
        SDL_RenderLine(renderer, x, 0, x, HEIGHT);
    for (int y = 0; y <= HEIGHT; y += CELL_SIZE)
        SDL_RenderLine(renderer, 0, y, WIDTH, y);
}

void fill_cell(SDL_Renderer *renderer, int x, int y, SDL_Color color) {
    SDL_FRect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &cell);
}

void draw_snake(SDL_Renderer *renderer, Snake *snake) {
    for (int i = 0; i < snake->length; i++) {
        fill_cell(renderer, snake->body[i].x, snake->body[i].y, WHITE_COLOR);
    }
}

void move_snake(SDL_Renderer *renderer, Snake *snake) {
    Position tail = snake->body[snake->length - 1];
    fill_cell(renderer, tail.x, tail.y, BLACK_COLOR);

    // shift snake body
    for (int i = snake->length - 1; i > 0; i--)
        snake->body[i] = snake->body[i - 1];

    // move head
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;
}

void gen_food(Position *food, Snake *snake) {
    int valid = 0;

    while (!valid) {
        valid = 1;
        food->x = SDL_rand(CELLS_X);
        food->y = SDL_rand(CELLS_Y);
        for (int i = 0; i < snake->length; i++) {
            if (snake->body[i].x == food->x && snake->body[i].y == food->y) {
                valid = 0;
                break;
            }
        }
    }
}

void poll_events(bool *running, Snake *snake, bool *input_locked) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
            *running = false;
        if (e.type != SDL_EVENT_KEY_DOWN)
            return;
        if (e.key.key == SDLK_ESCAPE)
            *running = false;
        if (*input_locked != false) {
            switch (e.key.key) {
            case (SDLK_RIGHT):
                if (snake->dx != -1) {
                    snake->dx = 1;
                    snake->dy = 0;
                    *input_locked = true;
                }
                break;
            case (SDLK_LEFT):
                if (snake->dx != 1) {
                    snake->dx = -1;
                    snake->dy = 0;
                    *input_locked = true;
                }
                break;
            case (SDLK_UP):
                if (snake->dy != 1) {
                    snake->dx = 0;
                    snake->dy = -1;
                    *input_locked = true;
                }
                break;
            case (SDLK_DOWN):
                if (snake->dy != -1) {
                    snake->dx = 0;
                    snake->dy = 1;
                    *input_locked = true;
                }
                break;
            default:
                break;
            }
        }
    }
}

bool food_collision(Snake *snake, Position *food) {
    if (snake->body[0].x == food->x && snake->body[0].y == food->y)
        return true;
    return false;
}

bool wall_collision(Snake *snake) {
    if (snake->body[0].x < 0 || snake->body[0].y < 0)
        return true;
    if (snake->body[0].x >= CELLS_X || snake->body[0].y >= CELLS_Y)
        return true;
    return false;
}

bool snake_collision(Snake *snake) {
    for (int i = 1; i < snake->length; i++)
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y)
            return true;

    return false;
}

void reset_game(SDL_Renderer *renderer, Snake *snake, Position *food, int *score, bool *input_locked) {
    fill_cell(renderer, food->x, food->y, BLACK_COLOR);

    if (snake->length > 1)
        for (int i = 0; i < snake->length; i++)
            fill_cell(renderer, snake->body[i].x, snake->body[i].y, BLACK_COLOR);

    snake->length = 1;
    snake->body[0] = (Position){CELLS_X / 2, CELLS_Y - 4};
    // facing up
    snake->dx = 0;
    snake->dy = -1;

    *input_locked = false;
    *score = 0;
    gen_food(food, snake);
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Snake game", WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
    SDL_SetWindowResizable(window, false);

    // SDL_Surface *surface = SDL_GetWindowSurface(window);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderVSync(renderer, true);

    TTF_Font *font = TTF_OpenFont("res/font.ttf", 16);
    if (!font)
        return -1;

    printf("Cells in each row: %d\n", CELLS_X);
    printf("Cells in each col: %d\n", CELLS_Y);
    printf("Total cells: %d\n", TOTAL_CELLS);

    Snake snake = {0};
    Position food = {0};
    int score = 0;
    int high_score = 0;
    bool input_locked = false;

    reset_game(renderer, &snake, &food, &score, &input_locked);

    Uint64 last_move = 0;
    int snake_speed = DEFAULT_SNAKE_SPEED;

    int fps = 0;
    int frame_count = 0;
    Uint64 last_fps_time = SDL_GetTicks();

    // chache vars
    int prev_score = -1, prev_high = -1, prev_fps = -1;
    SDL_Texture *score_tex = NULL, *high_tex = NULL, *fps_tex = NULL;
    float score_w, score_h, high_w, high_h, fps_w, fps_h;

    Uint64 key_hold_start = 0;
    bool holding = false;

    bool running = true;
    char buffer[128];
    // game loop
    while (running) {
        Uint64 now = SDL_GetTicks();

        // poll events
        poll_events(&running, &snake, &input_locked);
        const bool *state = SDL_GetKeyboardState(NULL);

        bool is_holding =
            state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT];

        if (is_holding) {
            if (!holding) {
                holding = true;
                key_hold_start = now;
            }

            if (now - key_hold_start > 200)
                snake_speed = FAST_SNAKE_SPEED;
            else
                snake_speed = DEFAULT_SNAKE_SPEED;
        } else {
            holding = false;
            snake_speed = DEFAULT_SNAKE_SPEED;
        }

        // update
        if (now > last_move + snake_speed) {
            move_snake(renderer, &snake);
            // unlock input afte snake has successfully moved
            input_locked = 1;
            if (wall_collision(&snake) || snake_collision(&snake))
                reset_game(renderer, &snake, &food, &score, &input_locked);

            if (food_collision(&snake, &food)) {
                score += 1;

                if (score > high_score)
                    high_score = score;

                snake.body[snake.length] = snake.body[snake.length - 1];
                snake.length++;

                gen_food(&food, &snake);
            }

            last_move = now;
        }

        frame_count++;
        if (now > last_fps_time + 1000) {
            fps = frame_count;
            frame_count = 0;
            last_fps_time = now;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // render
        draw_grid(renderer);
        draw_snake(renderer, &snake);
        fill_cell(renderer, food.x, food.y, RED_COLOR);

        if (score != prev_score || !score_tex) {
            if (score_tex)
                SDL_DestroyTexture(score_tex);
            sprintf(buffer, "Score: %d", score);
            score_tex = create_text_texture(renderer, font, buffer, TEXT_COLOR, &score_w, &score_h);
            prev_score = score;
        }
        if (high_score != prev_high || !high_tex) {
            if (high_tex)
                SDL_DestroyTexture(high_tex);
            sprintf(buffer, "High: %d", high_score);
            high_tex = create_text_texture(renderer, font, buffer, TEXT_COLOR, &high_w, &high_h);
            prev_high = high_score;
        }
        if (fps != prev_fps || !fps_tex) {
            if (fps_tex)
                SDL_DestroyTexture(fps_tex);
            sprintf(buffer, "FPS: %d", fps);
            fps_tex = create_text_texture(renderer, font, buffer, TEXT_COLOR, &fps_w, &fps_h);
            prev_fps = fps;
        }

        SDL_FRect dst_score = {10, 10, score_w, score_h};
        SDL_RenderTexture(renderer, score_tex, NULL, &dst_score);

        SDL_FRect dst_high = {10, 30, high_w, high_h};
        SDL_RenderTexture(renderer, high_tex, NULL, &dst_high);

        SDL_FRect dst_fps = {10, 50, fps_w, fps_h};
        SDL_RenderTexture(renderer, fps_tex, NULL, &dst_fps);

        // SDL_UpdateWindowSurface(window);
        SDL_RenderPresent(renderer);
        //        SDL_Delay(16);
    }

    // Clean
    if (score_tex)
        SDL_DestroyTexture(score_tex);
    if (high_tex)
        SDL_DestroyTexture(high_tex);
    if (fps_tex)
        SDL_DestroyTexture(fps_tex);

    TTF_CloseFont(font);
    // SDL_DestroySurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
