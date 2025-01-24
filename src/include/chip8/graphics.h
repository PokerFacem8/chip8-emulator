// chip8.h
#ifndef graphics_h
#define graphics_h
#include <SDL.h>

class Graphics {

    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        int SCALE;
        int WIDTH;
        int HEIGHT;

        Graphics();
        void init();
        void drawDisplay(unsigned char display[64][32]);
        void clear(unsigned char display[64][32]);
};

#endif
