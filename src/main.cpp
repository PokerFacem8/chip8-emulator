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
    //FreeConsole(); (Use for Release)

    std::filesystem::current_path(std::filesystem::path(__FILE__).parent_path());

    //Init Random Seed
    srand(time(0));

    //New Chip8 Instance (Reset Values + Init Graphics)
    Chip8 chip8 = Chip8();

    //Map of ROMS    
    std::map<std::string, std::string> roms;

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

        //Add to Map
        roms[entry.path().filename().string()] = entry.path().string();
    }

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
            ImGui_ImplSDL2_ProcessEvent(&event); // Forward your event to backend
            switch (event.type) 
            {
                case SDL_KEYDOWN:
                    chip8.pressKey(event.key.keysym.scancode);
                    if(event.key.keysym.scancode == SDL_SCANCODE_F1)
                    {
                        if (chip8.debugMode)
                        {
                            chip8.debugMode = false;
                            chip8.graphics.fullscreen(true);
                        } else {
                            chip8.graphics.fullscreen(false);
                            chip8.debugMode = true;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    chip8.pressKey(-1);
                    break;
                case SDL_QUIT:
                    quit = true;
                    break;
            }
            
        }


        //Update the texture
        SDL_RenderPresent(chip8.graphics.renderer);

        // Update ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        

        //Game Display
        ImGui::SetNextWindowSize(ImVec2(chip8.graphics.WIDTH + 20, chip8.graphics.HEIGHT + 40));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Game", nullptr, chip8.debugMode ? 0 : ImGuiWindowFlags_NoTitleBar);
        ImGui::Image((ImTextureID)chip8.graphics.texture, ImVec2(chip8.graphics.WIDTH, chip8.graphics.HEIGHT));
        ImGui::End();

        if(chip8.debugMode) {

            //--------------------------------------------//

            //Console Debug
            ImGui::SetNextWindowSize(ImVec2(660, 360));
            ImGui::SetNextWindowPos(ImVec2(0, 361));
            ImGui::Begin("Debug Console", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
            //Print Console Stack
            for (int i = 45; i >= 0; i--)
            {
                ImGui::Text(chip8.console[i].c_str());
            }
            ImGui::End();

            //--------------------------------------------//

            //Emulator Memory
            ImGui::SetNextWindowSize(ImVec2(282, 360));
            ImGui::SetNextWindowPos(ImVec2(661, 0));
            ImGui::Begin("Memory");
            ImGui::Columns(2);
            //Column for Registers
            ImGui::Text("Registers");
            for (int i = 0; i < 16; i++)
            {
                ImGui::Text("V%X: %X", i, chip8.v[i]);
            }
            ImGui::NextColumn();
            //Column for Timers
            ImGui::Text("Counters/Timers");
            ImGui::Text("PC: %X", chip8.pc);
            ImGui::Text("I: %X", chip8.index);
            ImGui::Text("SP: %X", chip8.sp);
            ImGui::Text("Last Opcode: %X", chip8.lastOpcode);
            ImGui::Text("Draw Flag: %s", chip8.drawFlag ? "True" : "False");
            ImGui::Text("Pressed Key: %X", chip8.pressedKey);
            ImGui::Text("Delay Timer: %X", chip8.delay_timer);
            ImGui::Text("Sound Timer: %X", chip8.sound_timer);
            ImGui::End();

        

            //--------------------------------------------//

            //Menu to Load ROM
            ImGui::SetNextWindowSize(ImVec2(300, 360));
            ImGui::SetNextWindowPos(ImVec2(942, 0));
            ImGui::Begin("ROMS");
            for (const auto & [key, value] : roms)
            {
                if(ImGui::Button(key.c_str()))
                {
                    //Print
                    //cout << "Loading ROM: " << value << endl;
                    chip8.pushLog("Loading ROM: " + value);
                    chip8.unLoadROM();
                    chip8.loadROM(value);
                }
            }
            ImGui::End();

        }

        //--------------------------------------------//

        ImGui::Render();

        SDL_SetRenderDrawColor(chip8.graphics.renderer, 0, 0, 0, 255);
        SDL_RenderClear(chip8.graphics.renderer);

        // Render Behind

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), chip8.graphics.renderer);

        // Render Front

        SDL_RenderPresent(chip8.graphics.renderer);

















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
                chip8.pushLog("BEEP");
                Beep(523,100); // 523 hertz (C5) for 500 milliseconds 
            }
            chip8.sound_timer--;
        }
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    chip8.destroyGraphics();

    return 0;
}