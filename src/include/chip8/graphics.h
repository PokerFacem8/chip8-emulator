// chip8.h
#ifndef graphics_h
#define graphics_h
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

class Graphics {

    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        SDL_Texture* gameTexture;
        int SCALEX;
        int SCALEY;
        int WIDTH;
        int HEIGHT;
        int SCREENX;
        int SCREENY;
      

        Graphics();
        void init();
        void fullscreen(bool fullscreen);
        void drawDisplay(unsigned char display[64][32]);
        void clear(unsigned char display[64][32]);
};

#endif
