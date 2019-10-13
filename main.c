#include <stdio.h>
#include <SDL2/SDL.h>

const int width = 640;
const int height = 480;

#define trace_assert(expression)                                                            \
    if (!(expression)) {                                                                    \
        fprintf(stderr, "Assertion failed: %s at %s:%d", #expression, __FILE__, __LINE__);  \
        exit(1);                                                                            \
    }                                                                                       \

SDL_Texture *get_cell(SDL_Renderer *renderer, int r, int g, int b) {
    SDL_Surface *cell_surface = NULL;
    trace_assert(cell_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0));
    SDL_FillRect(cell_surface, NULL, SDL_MapRGB(cell_surface->format, 140, 140, 140));
    SDL_Rect border;
    border.x = border.y = 0;
    border.w = 32;
    border.h = 2;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.y = 32 - 2;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.x = border.y = 0;
    border.w = 2;
    border.h = 32;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    border.x = 32 - 2;
    SDL_FillRect(cell_surface, &border, SDL_MapRGB(cell_surface->format, 255, 255, 255));
    SDL_Texture *cell_texture = NULL;
    trace_assert(cell_texture = SDL_CreateTextureFromSurface(renderer, cell_surface));
    SDL_FreeSurface(cell_surface);
    return cell_texture;
}

int main(int argc, char **argv) {
    trace_assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window *window = NULL;
    trace_assert(window = SDL_CreateWindow("Tetris",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, SDL_WINDOW_SHOWN));
    SDL_Renderer *renderer = NULL;
    trace_assert(renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    SDL_Surface *surface = NULL;
    trace_assert(surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0));
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 140, 140, 140));

    SDL_Texture *texture = NULL;
    trace_assert(texture = SDL_CreateTextureFromSurface(renderer, surface));

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = width / 3 * 2;
    rect.h = height;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_Texture *empty_cell = get_cell(renderer, 140, 140, 140);
    SDL_Rect cell;
    cell.w = cell.h = 16;
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 25; ++j) {
            cell.x = i * 16 + 32;
            cell.y = j * 16 + 16;
            SDL_RenderCopy(renderer, empty_cell, NULL, &cell);
        }
    }
    SDL_RenderPresent(renderer);

    int quit = 0;
    SDL_Event event;

    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            quit = 1;
            break;
        }
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(empty_cell);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
