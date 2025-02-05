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

    windowUI = SDL_CreateWindow("DEBUG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    rendererUI = SDL_CreateRenderer(windowUI, -1, SDL_RENDERER_ACCELERATED);
    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(windowUI, rendererUI);
    ImGui_ImplSDLRenderer2_Init(rendererUI);


    //Background Color
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderClear(renderer);
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

