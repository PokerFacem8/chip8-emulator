#include <iostream>
#include <fstream>
#include <chip8.h>




using namespace std; 




/*
Specifications
    CHIP-8 has the following components:

    Memory: CHIP-8 has direct access to up to 4 kilobytes of RAM

    Display: 64 x 32 pixels (or 128 x 64 for SUPER-CHIP) monochrome, ie. black or white (Why are you scaling individual graphics objects? Just render to a bitmap and scale that.)
    
    A program counter, often called just “PC”, which points at the current instruction in memory
    
    One 16-bit index register called “I” which is used to point at locations in memory
    
    A stack for 16-bit addresses, which is used to call subroutines/functions and return from them
    
    An 8-bit delay timer which is decremented at a rate of 60 Hz (60 times per second) until it reaches 0
    
    An 8-bit sound timer which functions like the delay timer, but which also gives off a beeping sound as long as it’s not 0
    
    16 8-bit (one byte) general-purpose variable registers numbered 0 through F hexadecimal, ie. 0 through 15 in decimal, called V0 through VF
        VF is also used as a flag register; many instructions will set it to either 1 or 0 based on some rule, for example using it as a carry flag
*/

/* 
Memory Map:
    +---------------+= 0xFFF (4095) End of Chip-8 RAM
    |               |
    |               |
    |               |
    |               |
    |               |
    | 0x200 to 0xFFF|
    |     Chip-8    |
    | Program / Data|
    |     Space     |
    |               |
    |               |
    |               |
    +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
    |               |
    |               |
    |               |
    +---------------+= 0x200 (512) Start of most Chip-8 programs
    | 0x000 to 0x1FF|
    | Reserved for  |
    |  interpreter  |
    +---------------+= 0x000 (0) Start of Chip-8 RAM
*/

/*

Keyboard
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F

1 2 3 4		
Q W E R		
A S D F		
Z X C V		

*/


int main(int argv, char** args)
{   

    //Init Random Seed
    srand(time(0));

    //New Chip8 Instance (Reset Values + Init Graphics)
    Chip8 chip8 = Chip8();

    //Load ROM into memory
    chip8.loadROM("E:\\Personal\\Projects\\chip8-emulator\\roms\\1-chip8-logo.ch8");


    /**
     * 
     * outer loop:

        1 DEcrement timers and render display
        2 execute inner loop
        3 goto 1

        inner loop:

        execute 11ish opcodes, unless you execute a 
        dxyn in which case you do that opcode and then 
        finish the loop early.

     */

        // (After event loop)



   

    bool quit = false;
    while (!quit)
    {

        //Wait for Key Press
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) 
            {
                case SDL_KEYDOWN:
                    cout << "Pressed Key: " << endl;
                    chip8.pressKey(event.key.keysym.scancode);
                    break;
                case SDL_KEYUP:
                    chip8.pressKey(0);
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        if (event.window.windowID == SDL_GetWindowID(chip8.graphics.window)) {
                            std::cout << "Window 1 closed" << std::endl;
                            SDL_DestroyWindow(chip8.graphics.window);
                            chip8.graphics.window = nullptr;
                        } else if (event.window.windowID == SDL_GetWindowID(chip8.graphics.windowUI)) {
                            std::cout << "Window 2 closed" << std::endl;
                            SDL_DestroyWindow(chip8.graphics.windowUI);
                            chip8.graphics.windowUI = nullptr;
                        }
                    }
                    break;
            }
            ImGui_ImplSDL2_ProcessEvent(&event); // Forward your event to backend
        }

        //Emulator Loop (Frame) 
        chip8.cycle();

        //Update Display
        if(chip8.drawFlag)
        {
            chip8.drawFlag = false;
            chip8.updateDisplay();
        }


   
        //SDL_RenderPresent(chip8.graphics.rendererUI);



        if (!chip8.graphics.window && !chip8.graphics.windowUI) {
            quit = true;
        }

        
        //Sleep
        SDL_Delay(16);

        
    }
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    chip8.destroyGraphics();



    return 0;
}