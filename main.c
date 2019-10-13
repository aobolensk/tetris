#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int CELL_SIZE = 16;
const int BORDER_SIZE = 1;
const int FIELD_OFFSET_X = 32;
const int FIELD_OFFSET_Y = 16;
const int FIELD_WIDTH = 20;
const int FIELD_HEIGHT = 20;

#define trace_assert(expression)                                                            \
    if (!(expression)) {                                                                    \
        fprintf(stderr, "Assertion failed: %s at %s:%d", #expression, __FILE__, __LINE__);  \
        exit(1);                                                                            \
    }                                                                                       \

enum CellState {
    CellStateEmpty,
    CellStateRed,
    CellStateGreen,
    CellStateBlue,
    CellStateYellow,
    CellStatePurple
};

SDL_Texture *get_cell(SDL_Renderer *renderer, int r, int g, int b) {
    SDL_Surface *cell_surface = NULL;
    trace_assert(cell_surface = SDL_CreateRGBSurface(0, CELL_SIZE, CELL_SIZE, 32, 0, 0, 0, 0));
    SDL_FillRect(cell_surface, NULL, SDL_MapRGB(cell_surface->format, r, g, b));
    SDL_Rect border;
    border.x = border.y = 0;
    border.w = CELL_SIZE;
    border.h = BORDER_SIZE;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.y = CELL_SIZE - BORDER_SIZE;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.x = border.y = 0;
    border.w = BORDER_SIZE;
    border.h = CELL_SIZE;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.x = CELL_SIZE - BORDER_SIZE;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    SDL_Texture *cell_texture = NULL;
    trace_assert(cell_texture = SDL_CreateTextureFromSurface(renderer, cell_surface));
    SDL_FreeSurface(cell_surface);
    return cell_texture;
}

typedef struct {
    int x, y;
} Point;

Point tetromino[6][4] = {
    {
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0}
    },
    {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 2, .y = 0},
        {.x = 3, .y = 0}
    },
    {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 0, .y = 1},
        {.x = 1, .y = 1}
    },
    {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 2, .y = 0},
        {.x = 2, .y = 1}
    },
    {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 1, .y = 1},
        {.x = 2, .y = 1}
    },
    {
        {.x = 0, .y = 0},
        {.x = 0, .y = 1},
        {.x = 0, .y = 2},
        {.x = 1, .y = 1}
    }
};

Point spawn_tetromino(int **field) {
    int type = rand() % 5 + 1;
    int c = (FIELD_WIDTH - 1) / 2;
    for (int i = 0; i < 4; ++i) {
        field[c + tetromino[type][i].x][0 + tetromino[type][i].y] = type;
    }
    return (Point){
        .x = c,
        .y = 0
    };
}

Uint32 timer_callback(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 1;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

int main(int argc, char **argv) {
    srand((unsigned)time(0));
    trace_assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window *window = NULL;
    trace_assert(window = SDL_CreateWindow("Tetris",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN));
    SDL_Renderer *renderer = NULL;
    trace_assert(renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    SDL_Surface *surface = NULL;
    trace_assert(surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0));
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 140, 140, 140));

    SDL_Texture *texture = NULL;
    trace_assert(texture = SDL_CreateTextureFromSurface(renderer, surface));

    int **field = NULL;
    trace_assert(field = calloc(FIELD_WIDTH, sizeof(int*)));
    for (int i = 0; i < FIELD_WIDTH; ++i) {
        trace_assert(field[i] = calloc(FIELD_HEIGHT, sizeof(int)));
    }

    Point floating_tetromino = spawn_tetromino(field);

    SDL_Texture *empty_cell = get_cell(renderer, 140, 140, 140);
    SDL_Texture *red_cell = get_cell(renderer, 255, 0, 0);
    SDL_Texture *green_cell = get_cell(renderer, 0, 255, 0);
    SDL_Texture *blue_cell = get_cell(renderer, 0, 0, 255);
    SDL_Texture *yellow_cell = get_cell(renderer, 0, 255, 255);
    SDL_Texture *purple_cell = get_cell(renderer, 255, 0, 255);
    SDL_Rect cell_rect;
    cell_rect.w = cell_rect.h = CELL_SIZE;

    SDL_TimerID timer_id = SDL_AddTimer(100, timer_callback, NULL);
    int quit = 0;
    SDL_Event event;

    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_USEREVENT: {
            if (event.user.code == 1) {
                int x = floating_tetromino.x;
                int y = floating_tetromino.y;
                int type = field[floating_tetromino.x][floating_tetromino.y];
                bool move = true;
                bool ok = true;
                for (int i = 0; i < 4; ++i) {
                    if (y + tetromino[type][i].y + 1 >= FIELD_HEIGHT) {
                        ok = false;
                        break;
                    }
                    bool check = true;
                    for (int j = 0; j < 4; ++j) {
                        if (x + tetromino[type][i].x == x + tetromino[type][j].x &&
                            y + tetromino[type][i].y + 1 == y + tetromino[type][j].y) {
                            check = false;
                            break;
                        }
                    }
                    if (!check) {
                        continue;
                    }
                    if (field[x + tetromino[type][i].x][y + tetromino[type][i].y + 1] != CellStateEmpty) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) {
                    move = false;
                }
                if (move) {
                    for (int i = 0; i < 4; ++i) {
                        field[x + tetromino[type][i].x][y + tetromino[type][i].y] = CellStateEmpty;
                    }
                    ++floating_tetromino.y, ++y;
                    for (int i = 0; i < 4; ++i) {
                        field[x + tetromino[type][i].x][y + tetromino[type][i].y] = type;
                    }
                } else {
                    floating_tetromino = spawn_tetromino(field);
                }
            }
        } break;
        case SDL_QUIT: {
            quit = 1;
        } break;
        }
        for (int i = 0; i < FIELD_WIDTH; ++i) {
            for (int j = 0; j < FIELD_HEIGHT; ++j) {
                cell_rect.x = i * CELL_SIZE + FIELD_OFFSET_X;
                cell_rect.y = j * CELL_SIZE + FIELD_OFFSET_Y;
                switch (field[i][j]) {
                case CellStateEmpty:
                    SDL_RenderCopy(renderer, empty_cell, NULL, &cell_rect);
                    break;
                case CellStateRed:
                    SDL_RenderCopy(renderer, red_cell, NULL, &cell_rect);
                    break;
                case CellStateGreen:
                    SDL_RenderCopy(renderer, green_cell, NULL, &cell_rect);
                    break;
                case CellStateBlue:
                    SDL_RenderCopy(renderer, blue_cell, NULL, &cell_rect);
                    break;
                case CellStateYellow:
                    SDL_RenderCopy(renderer, yellow_cell, NULL, &cell_rect);
                    break;
                case CellStatePurple:
                    SDL_RenderCopy(renderer, purple_cell, NULL, &cell_rect);
                    break;
                }
            }
        }
        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < FIELD_WIDTH; ++i) {
        free(field[i]);
    }
    free(field);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(empty_cell);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
