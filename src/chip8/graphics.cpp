#include <graphics.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>


Graphics::Graphics(){
    SCALE = 29;
    WIDTH = 64;
    HEIGHT = 32;
};

void Graphics::init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("CHIP8 EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCALE * WIDTH, SCALE * HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void Graphics::drawDisplay(unsigned char display[64][32]) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (display[i][j] == 1)
            {
                SDL_Rect rect;
                rect.x = i * SCALE;
                rect.y = j * SCALE;
                rect.w = SCALE;
                rect.h = SCALE;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Graphics::clear(unsigned char display[64][32]) {
    //Clear display matrix
    SDL_RenderClear(renderer);
}