#include <stdio.h>
#include <SDL2/SDL.h>

const int width = 640;
const int height = 480;

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Tetris",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "Could not create SDL renderer Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        fprintf(stderr, "Could not create SDL surface: %s", SDL_GetError());
        return 1;
    }
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 140, 140, 140));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        fprintf(stderr, "Could not create texture from surface: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Surface *cell_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    if (cell_surface == NULL) {
        fprintf(stderr, "Could not create SDL surface: %s", SDL_GetError());
        return 1;
    }
    
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

    SDL_Texture *cell_texture = SDL_CreateTextureFromSurface(renderer, cell_surface);
    if (cell_texture == NULL) {
        fprintf(stderr, "Could not create texture from surface: %s\n", SDL_GetError());
        exit(1);
    }
    
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = width / 3 * 2;
    rect.h = height;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_Rect cell;
    cell.w = cell.h = 16;
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 25; ++j) {
            cell.x = i * 16 + 32;
            cell.y = j * 16 + 16;
            SDL_RenderCopy(renderer, cell_texture, NULL, &cell);
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
    SDL_FreeSurface(cell_surface);
    SDL_DestroyTexture(cell_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
