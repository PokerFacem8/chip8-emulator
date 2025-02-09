#include <graphics.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <bits/algorithmfwd.h>


Graphics::Graphics(){
    SCALE = 10;
    WIDTH = 64;
    HEIGHT = 32;
};

void Graphics::init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("CHIP8 EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 320);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls


    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void Graphics::drawDisplay(unsigned char display[64][32]) {

    // Set the target texture
    SDL_SetRenderTarget(renderer, texture);

    // Clear the texture (black background)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the game pixels onto the texture
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

    // Reset the target texture
    SDL_SetRenderTarget(renderer, NULL);
}

void Graphics::clear(unsigned char display[64][32]) {
    //Clear display matrix
    SDL_RenderClear(renderer);
}

