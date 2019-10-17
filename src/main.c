#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "trace_assert.h"
#include "config.h"

enum CellState {
    CellStateEmpty,
    CellStateRed,
    CellStateGreen,
    CellStateBlue,
    CellStateLightBlue,
    CellStatePurple
};

typedef struct {
    int x, y;
} Point;

SDL_Texture *get_cell(SDL_Renderer *renderer, SDL_Color cell_color, SDL_Color border_color) {
    SDL_Surface *cell_surface = NULL;
    trace_assert(cell_surface = SDL_CreateRGBSurface(0, CELL_SIZE, CELL_SIZE, 32, 0, 0, 0, 0));
    SDL_FillRect(cell_surface, NULL, SDL_MapRGB(cell_surface->format, border_color.r, border_color.g, border_color.b));
    SDL_Rect border;
    border.x = border.y = BORDER_SIZE;
    border.w = border.h = CELL_SIZE - 2 * BORDER_SIZE;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, cell_color.r, cell_color.g, cell_color.b));
    SDL_Texture *cell_texture = NULL;
    trace_assert(cell_texture = SDL_CreateTextureFromSurface(renderer, cell_surface));
    SDL_FreeSurface(cell_surface);
    return cell_texture;
}

Point tetromino[24][4] = {
    {
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0}
    },
    {
        {.x = 0, .y = 0},   // #
        {.x = 1, .y = 0},   // #
        {.x = 2, .y = 0},   // #
        {.x = 3, .y = 0}    // #
    },
    {
        {.x = 0, .y = 0},   // ##
        {.x = 1, .y = 0},   // ##
        {.x = 0, .y = 1},   //
        {.x = 1, .y = 1}    //
    },
    {
        {.x = 0, .y = 0},   // ##
        {.x = 1, .y = 0},   // #
        {.x = 2, .y = 0},   // #
        {.x = 0, .y = 1}    //
    },
    {
        {.x = 0, .y = 0},   // #
        {.x = 1, .y = 0},   // ##
        {.x = 1, .y = 1},   //  #
        {.x = 2, .y = 1}    //
    },
    {
        {.x = 0, .y = 0},   // ###
        {.x = 0, .y = 1},   //  #
        {.x = 0, .y = 2},   //
        {.x = 1, .y = 1}    //
    },
    {
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0},
        {.x = 0, .y = 0}
    },
    {
        {.x = 0, .y = 0},   // #
        {.x = 1, .y = 0},   // #
        {.x = 2, .y = 0},   // #
        {.x = 3, .y = 0}    // #
    },
    {
        {.x = 0, .y = 0},   // ##
        {.x = 1, .y = 0},   // ##
        {.x = 0, .y = 1},   //
        {.x = 1, .y = 1}    //
    },
    {
        {.x = 0, .y = 0},   // ##
        {.x = 0, .y = 1},   //  #
        {.x = 1, .y = 1},   //  #
        {.x = 2, .y = 1}    //
    },
    {
        {.x = 0, .y = 1},   //  #
        {.x = 1, .y = 0},   // ##
        {.x = 1, .y = 1},   // #
        {.x = 2, .y = 0}    //
    },
    {
        {.x = 0, .y = 1},   //  #
        {.x = 1, .y = 0},   // ###
        {.x = 1, .y = 1},   //
        {.x = 1, .y = 2}    //
    },
};

int type;

Point spawn_tetromino(int **field) {
    int r = rand() % 10 + 1;
    if (r <= 1) {
        type = 1;
    } else if (r <= 3) {
        type = 2;
    } else if (r <= 6) {
        type = 3;
    } else if (r <= 8) {
        type = 4;
    } else if (r <= 10) {
        type = 5;
    } else {
        trace_assert(0);
    }
    type = rand() % 5 + 1;
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
    return 500 - (*(int*)param) / 10;
}

int main(int argc, char **argv) {
    #if defined(__GNUC__) && defined(__linux__)
    signal(SIGSEGV, trace_signal);
    #endif  // defined(__GNUC__) && defined(__linux__)
    trace_assert(FIELD_WIDTH >= 8);
    trace_assert(FIELD_HEIGHT >= 8);

    tetromino[15][0].x = 0;
    tetromino[15][0].y = 0;
    tetromino[15][1].x = 1;
    tetromino[15][1].y = 0;
    tetromino[15][2].x = 2;
    tetromino[15][2].y = 0;
    tetromino[15][3].x = 2;
    tetromino[15][3].y = 1;

    tetromino[21][0].x = 0;
    tetromino[21][0].y = 1;
    tetromino[21][1].x = 1;
    tetromino[21][1].y = 1;
    tetromino[21][2].x = 2;
    tetromino[21][2].y = 0;
    tetromino[21][3].x = 2;
    tetromino[21][3].y = 1;

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
    bool just_spawned = true;
    int tetromino_state = 0;

    SDL_Color border_color = {.r = 180, .g = 180, .b = 180, .a = 255};
    SDL_Texture *empty_cell = get_cell(renderer, (SDL_Color){.r = 140, .g = 140, .b = 140, .a = 255}, border_color);
    SDL_Texture *red_cell = get_cell(renderer, (SDL_Color){.r = 230, .g = 0, .b = 0, .a = 255}, border_color);
    SDL_Texture *green_cell = get_cell(renderer, (SDL_Color){.r = 50, .g = 230, .b = 50, .a = 255}, border_color);
    SDL_Texture *blue_cell = get_cell(renderer, (SDL_Color){.r = 0, .g = 0, .b = 230, .a = 255}, border_color);
    SDL_Texture *light_blue_cell = get_cell(renderer, (SDL_Color){.r = 0, .g = 230, .b = 230, .a = 255}, border_color);
    SDL_Texture *purple_cell = get_cell(renderer, (SDL_Color){.r = 210, .g = 30, .b = 210, .a = 255}, border_color);
    SDL_Rect cell_rect;
    cell_rect.w = cell_rect.h = CELL_SIZE;

    int score = 0;
    int bonus = 0;
    int lines = 0;
    bool paused = 0;

    SDL_TimerID timer_id = SDL_AddTimer(500, timer_callback, &lines);
    int quit = 0;
    SDL_Event event;

game_start:
    score = 0;
    bonus = 0;
    lines = 0;
    paused = 0;
    while (!quit) {
        SDL_WaitEvent(&event);
        int *x = &floating_tetromino.x;
        int *y = &floating_tetromino.y;
        switch (event.type) {
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_a:
            case SDLK_LEFT: {
                if (!paused) {
                    if (floating_tetromino.x - 1 >= 0) {
                        for (int i = 0; i < 4; ++i) {
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = CellStateEmpty;
                        }
                        --floating_tetromino.x;
                        bool move = true;
                        for (int i = 0; i < 4; ++i) {
                            if (field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] != CellStateEmpty) {
                                move = false;
                                break;
                            }
                        }
                        if (!move) {
                            ++floating_tetromino.x;
                        }
                        for (int i = 0; i < 4; ++i) {
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = type;
                        }
                    }
                }
            } break;
            case SDLK_d:
            case SDLK_RIGHT: {
                if (!paused) {
                    bool move = true;
                    for (int i = 0; i < 4; ++i) {
                        if (*x + tetromino[type][i].x + 1 >= FIELD_WIDTH) {
                            move = false;
                            break;
                        }
                    }
                    if (move) {
                        for (int i = 0; i < 4; ++i) {
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = CellStateEmpty;
                        }
                        ++floating_tetromino.x;
                        for (int i = 0; i < 4; ++i) {
                            if (field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] != CellStateEmpty) {
                                move = false;
                                break;
                            }
                        }
                        if (!move) {
                            --floating_tetromino.x;
                        }
                        for (int i = 0; i < 4; ++i) {
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = type;
                        }
                    }
                }
            } break;
            case SDLK_w:
            case SDLK_UP: {
                if (!paused) {
                    bool move = true;
                    for (int i = 0; i < 4; ++i) {
                        field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = CellStateEmpty;
                    }
                    if (tetromino_state % 2 == 0) {
                        for (int i = 0; i < 4; ++i) {
                            int t = tetromino[type][i].x;
                            tetromino[type][i].x = tetromino[type][i].y;
                            tetromino[type][i].y = t;
                        }
                        ++tetromino_state;
                    } else {
                        type += 6;
                        if (type % 6 == 3)
                            type %= 24;
                        else
                            type %= 12;
                        ++tetromino_state;
                    }
                    for (int i = 0; i < 4; ++i) {
                        if (*x + tetromino[type][i].x < 0 ||
                            *x + tetromino[type][i].x >= FIELD_WIDTH ||
                            *y + tetromino[type][i].y < 0 ||
                            *y + tetromino[type][i].y >= FIELD_HEIGHT) {
                            move = false;
                            break;
                        }
                        if (field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] != CellStateEmpty) {
                            move = false;
                            break;
                        }
                    }
                    if (!move) {
                        if (tetromino_state % 2 == 1) {
                            for (int i = 0; i < 4; ++i) {
                                int t = tetromino[type][i].x;
                                tetromino[type][i].x = tetromino[type][i].y;
                                tetromino[type][i].y = t;
                            }
                            --tetromino_state;
                        } else {
                            type -= 6;
                            if (type % 6 == 3)
                                type += 24;
                            else
                                type += 12;
                            if (type % 6 == 3)
                                type %= 24;
                            else
                                type %= 12;
                            --tetromino_state;
                        }
                    }
                    for (int i = 0; i < 4; ++i) {
                        field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = type;
                    }
                }
            } break;
            case SDLK_s:
            case SDLK_DOWN: {
                if (!paused) {
                    bool move = true;
                    while (move) {
                        for (int i = 0; i < 4; ++i) {
                            if (*y + tetromino[type][i].y >= FIELD_HEIGHT - 1) {
                                move = false;
                                break;
                            }
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = CellStateEmpty;
                        }
                        ++floating_tetromino.y;
                        for (int i = 0; i < 4; ++i) {
                            if (field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] != CellStateEmpty) {
                                move = false;
                                break;
                            }
                        }
                        if (!move) {
                            --floating_tetromino.y;
                        }
                        for (int i = 0; i < 4; ++i) {
                            field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = type;
                        }
                    }
                }
            } break;
            case SDLK_ESCAPE: {
                if (!paused) {
                    SDL_RemoveTimer(timer_id);
                } else {
                    timer_id = SDL_AddTimer(500, timer_callback, &lines);
                }
                paused = !paused;
            } break;
            default:
                break;
            }
        } break;
        case SDL_USEREVENT: {
            if (event.user.code == 1) {
                bool move = true;
                bool ok = true;
                for (int i = 0; i < 4; ++i) {
                    if (*y + tetromino[type][i].y + 1 >= FIELD_HEIGHT) {
                        ok = false;
                        break;
                    }
                    bool check = true;
                    for (int j = 0; j < 4; ++j) {
                        if (*x + tetromino[type][i].x == *x + tetromino[type][j].x &&
                            *y + tetromino[type][i].y + 1 == *y + tetromino[type][j].y) {
                            check = false;
                            break;
                        }
                    }
                    if (!check) {
                        continue;
                    }
                    if (field[*x + tetromino[type][i].x][*y + tetromino[type][i].y + 1] != CellStateEmpty) {
                        if (*y + tetromino[type][i].y < 3 && just_spawned) {
                            printf("Game over!\nYour score: %d\n", score);
                            for (int i = 0; i < FIELD_WIDTH; ++i) {
                                for (int j = 0; j < FIELD_HEIGHT; ++j) {
                                    field[i][j] = 0;
                                }
                            }
                            floating_tetromino = spawn_tetromino(field);
                            goto game_start;
                        }
                        ok = false;
                        break;
                    }
                }
                if (!ok) {
                    move = false;
                }
                if (move) {
                    for (int i = 0; i < 4; ++i) {
                        field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = CellStateEmpty;
                    }
                    ++floating_tetromino.y;
                    for (int i = 0; i < 4; ++i) {
                        field[*x + tetromino[type][i].x][*y + tetromino[type][i].y] = type;
                    }
                    just_spawned = false;
                } else {
                    floating_tetromino = spawn_tetromino(field);
                    just_spawned = true;
                    tetromino_state = 0;
                }
            }
        } break;
        case SDL_QUIT: {
            quit = 1;
        } break;
        }
        bool changed = false;
        int dels_in_a_row = 0;
        do {
            for (int j = 0; j < FIELD_HEIGHT; ++j) {
                bool del_row = true;
                for (int i = 0; i < FIELD_WIDTH; ++i) {
                    if (field[i][j] == CellStateEmpty) {
                        del_row = false;
                        break;
                    }
                }
                if (del_row) {
                    for (int k = j; k > 1; --k) {
                        for (int i = 0; i < FIELD_WIDTH; ++i) {
                            field[i][k] = field[i][k-1];
                        }
                    }
                    changed = true;
                } else {
                    changed = false;
                }
                if (changed) {
                    ++dels_in_a_row;
                    break;
                }
            }
        } while (changed);
        switch (dels_in_a_row) {
        case 0:
            break;
        case 1:
            bonus = 0;
            score += 150;
            printf("Score: %d\n", score);
            break;
        case 2:
            bonus = 0;
            score += 350;
            printf("Score: %d\n", score);
            break;
        case 3:
            bonus = 0;
            score += 750;
            printf("Score: %d\n", score);
            break;
        default:
            score += 1550 + bonus;
            bonus += 50;
            printf("Score: %d\n", score);
            break;
        }
        lines += dels_in_a_row;
        for (int i = 0; i < FIELD_WIDTH; ++i) {
            for (int j = 0; j < FIELD_HEIGHT; ++j) {
                cell_rect.x = i * CELL_SIZE + FIELD_OFFSET_X;
                cell_rect.y = j * CELL_SIZE + FIELD_OFFSET_Y;
                switch (field[i][j] % 6) {
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
                case CellStateLightBlue:
                    SDL_RenderCopy(renderer, light_blue_cell, NULL, &cell_rect);
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

    SDL_RemoveTimer(timer_id);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(empty_cell);
    SDL_DestroyTexture(red_cell);
    SDL_DestroyTexture(green_cell);
    SDL_DestroyTexture(blue_cell);
    SDL_DestroyTexture(light_blue_cell);
    SDL_DestroyTexture(purple_cell);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
