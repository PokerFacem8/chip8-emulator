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

/* Keyboard
    1	2	3	C
    4	5	6	D
    7	8	9	E
    A	0	B	F
*/


int main(int argv, char** args)
{   

    //New Chip8 Instance (Reset Values + Init Graphics)
    Chip8 chip8 = Chip8();

    //Load ROM into memory
    chip8.loadROM("E:\\Personal\\Projects\\chip8-emulator\\roms\\ibm.ch8");


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


    bool quit = false;
    while (!quit)
    {

        //Wait for Key Press
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }



        

        //Emulator Loop (Frame) 
        chip8.cycle();

        //Sleep
        SDL_Delay(16);

        //Update Display
        if(chip8.drawFlag)
        {
            chip8.drawFlag = false;
            chip8.updateDisplay();
        }
        


      
        
    }

    chip8.destroyGraphics();

    return 0;
}