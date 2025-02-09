#include <iostream>
#include <fstream>
#include <chip8.h>
#include <filesystem>
#include <windows.h> // WinApi header 


namespace fs = std::filesystem;
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

    std::filesystem::current_path(std::filesystem::path(__FILE__).parent_path());

    //Init Random Seed
    srand(time(0));

    //New Chip8 Instance (Reset Values + Init Graphics)
    Chip8 chip8 = Chip8();


    //Map of ROMS    
    std::map<int, std::string> roms;

    //List Available ROMS
    cout << "Available ROMS: " << endl;

    //Get Current Directory
    std::filesystem::path romsPath = std::filesystem::current_path() / "../roms";
    std::filesystem::path resolvedPath = std::filesystem::absolute(romsPath);


    for (const auto & entry : fs::directory_iterator(resolvedPath))
    {   
        //Ignore .gitignore
        if(entry.path().filename() == ".gitignore")
        {
            continue;
        }

        int index = roms.size() + 1;
        //Add to Map
        roms[index] = entry.path().string();
        cout << index << " - " << entry.path().filename() << endl;
    }

    //Choose ROM
    int choice;
    while (true)
    {
        cout << "Choose ROM: ";
        cin >> choice;

        if(choice > 0 && choice <= roms.size())
        {
            break;
        }
        else
        {
            cout << "Invalid Choice!" << endl;
        }
    }
    
    //Load ROM into memory
    chip8.loadROM(roms[choice]);

    
    


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
                        }
                    }
                    break;
            }
        }


        //Emulator Loop (Frame) 
        chip8.cycle();

        //Update Display
        if(chip8.drawFlag)
        {
            chip8.drawFlag = false;
            chip8.updateDisplay();
        }
        
        if (!chip8.graphics.window) {
            quit = true;
        }
 
        //Sleep
        SDL_Delay(16);

        //Delay Timer
        if(chip8.delay_timer > 0) {
            chip8.delay_timer--;
        }
        
        //Sound Timer
        if(chip8.sound_timer > 0) {
            if (chip8.sound_timer == 1)
            {
                cout << "BEEP!" << endl;
                Beep(523,500); // 523 hertz (C5) for 500 milliseconds 
            }
            chip8.sound_timer--;
        }
    }


    chip8.destroyGraphics();

    return 0;
}