#include <graphics.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>


Graphics::Graphics(){
    SCALE = 10;
    WIDTH = 64;
    HEIGHT = 32;
};

void Graphics::init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("CHIP8 EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64 * SCALE, 32 * SCALE);

    // Create a texture to render the game
    gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 320);

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
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    //Background Color
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderClear(renderer);
}

void Graphics::drawDisplay(unsigned char display[64][32]) {


    // Step 1: Create an SDL texture to render the game
    SDL_SetRenderTarget(renderer, gameTexture);

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

    SDL_SetRenderTarget(renderer, NULL);
    
    // Step 2: Clear the renderer for ImGui
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Step 3: Start ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Step 4: Render game texture inside ImGui window
    ImGui::Begin("CHIP-8 Display", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    // Calculate scaling and centering
    float scaleX = contentSize.x / (64.0f * SCALE);
    float scaleY = contentSize.y / (32.0f * SCALE);
    float scale = std::min(scaleX, scaleY);

    ImVec2 windowPos = ImGui::GetCursorScreenPos();
    SDL_Rect destRect;
    destRect.w = static_cast<int>(64 * SCALE * scale);
    destRect.h = static_cast<int>(32 * SCALE * scale);
    destRect.x = static_cast<int>(windowPos.x + (contentSize.x - destRect.w) / 2);
    destRect.y = static_cast<int>(windowPos.y + (contentSize.y - destRect.h) / 2);

    // Render `gameTexture` to the SDL renderer
    SDL_RenderCopy(renderer, gameTexture, NULL, &destRect);

    ImGui::End();

    // Step 5: Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Step 6: Present the renderer
    SDL_RenderPresent(renderer);

    

}



void Graphics::clear(unsigned char display[64][32]) {
    //Clear display matrix

    SDL_RenderClear(renderer);
}

