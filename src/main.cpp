#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>



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

class Graphics {

    public:

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        const int SCALE = 20;
        const int WIDTH = 64;
        const int HEIGHT = 32;

        bool imguiInit = false;

        void init() {
            SDL_Init(SDL_INIT_EVERYTHING);
            window = SDL_CreateWindow("CHIP8 EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCALE * WIDTH, SCALE * HEIGHT, SDL_WINDOW_SHOWN);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            //Background Color
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            //SDL_RenderPresent(renderer);
        }

        void drawDisplay(unsigned char display[64][32]) {
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

        void clear() {
            SDL_RenderClear(renderer);
        }

       

};


class Chip8 {


    public:

        unsigned char memory[4096]; // 4KB of memory
        unsigned short pc; // 16-bit program counter
        unsigned short index; // 16-bit index register
        unsigned short stack[64]; // 64 16-bit addresses
        unsigned char delay_timer; // 8-bit delay timer
        unsigned char sound_timer; // 8-bit sound timer
        unsigned char v[16]; // 16 8-bit general-purpose variable registers
        unsigned char display[64][32]; // 64 x 32 monochrome display
        Graphics& graphics;
        bool drawFlag = false;

        Chip8(Graphics& _graphics) : graphics(_graphics) {}

        void init() {
            memory[4096] = {};
            pc = 0x200; // Program counter starts at 0x200
            index = 0x0000; // Clear index register
            stack[64] = {};
            delay_timer = 0x000; // Clear delay timer
            sound_timer = 0x000; // Clear sound timer
            v[16] = {};
            display[64][32] = {};
        }

        void loadROM(string fileName){

            char buffer[10000];

            //Open File
            std::ifstream infile(fileName);

            // Get length of file
            infile.seekg(0, std::ios::end);
            size_t length = infile.tellg();
            infile.seekg(0, std::ios::beg);

            // Don't overflow the buffer!
            if (length > sizeof (buffer)) {
                length = sizeof (buffer);
            }

            // Read file
            infile.read(buffer, length);

            //Load file to Memory [Program Data]
            for (int i = 0; i < length; i++)
            {
                memory[i + pc] = buffer[i];
            }

            //Dump Memory as 0x notation
            /*for (int i = 0; i < length; i++)
            {
                cout << hex << (int)memory[i + pc] << endl;
            }*/

            //Close File
            infile.close();
        }

        void cycle(){


            unsigned short ipf = 11;
            
            //Instructions per Frame
            while (ipf > 0)
            {
                //----FETCH----

                //firstByte:     0000 0101
                //Shift 8 bits : 0000 0101 0000 0000
                //| secondByte:            0000 1010
                //Result:        0000 0101 0000 1010
                unsigned char firstByte = memory[pc];
                unsigned char secondByte = memory[pc + 1];  
                unsigned short opcode = (firstByte << 8) | secondByte;

                //Get First Code from Opcode
                //Opcode :              1010 0000 1111 0000  (0xAF0)
                //& 0xF000:             1111 0000 0000 0000
                //                      1010 0000 0000 0000
                //Shift 12 bits:        0000 0000 0000 1010
                unsigned short code = (opcode & 0xF000) >> 12;

                //PRINT OPCODE
                //cout << hex << opcode << endl;

                //PRINT CODE
                //cout << hex << code << endl;  

                //Increment Program Counter
                pc = pc + 2;

                //----DECODE----
                switch(code) {
                    case 0: //Clear Display 
                        cout << "Clear Display" << endl;
                        graphics.clear();
                        break;
                    case 0x1:
                        //cout << "Jump" << endl;

                        //Extract Address from Opcode
                        //Opcode:   0001 0101 1010 0101  (0x15A5)
                        //&0x0FFF:  0000 1111 1111 1111 
                        //Result:   0000 0000 0101 1010
                        //Set Program Counter to Address
                        pc = opcode & 0x0FFF;;

                        break;
                    case 0x6:
                        //cout << "Set Register" << endl;

                        //Extract Register from Opcode
                        //Opcode:       0110 0001 1010 0101  (0x15A5)
                        //&0x0F00:      0000 1111 0000 0000
                        //              0000 0001 0000 0000
                        //Shift 8 bits: 0000 0000 0000 0001

                        //Extract Value from Opcode
                        //Opcode:       0110 0001 1010 0101  (0x15A5)
                        //&0x00FF:      0000 0000 1111 1111
                        //              0000 0000 1010 0101

                        //Set Register
                        v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;

                        break;
                    case 0x7:
                        //cout << "Add Value to Register" << endl;

                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + opcode & 0x00FF;

                        break;
                    case 0xA:
                        //cout << "Set Index" << endl;

                        //Set Index Register
                        index = opcode & 0x0FFF;

                        break;
                    case 0xD:
                        //cout << "Draw" << endl;
                        draw(opcode);
                        break;
                    default:
                        cout << "Unknown opcode" << endl;
                        break; 
                }

                ipf--;
            }
        }

        void draw(unsigned short opcode) {

            //Print Opcode
            cout << hex << opcode << endl;

            unsigned short coordX;
            unsigned short coordY;
            unsigned short rows;
            unsigned char spriteRow;

            //Extract X and Y coordinates from VX and VY
            coordX = v[(opcode & 0x0F00) >> 8] % 64;

            //Print X Coordinate
            cout << hex << "X " << coordX << endl;
            
            coordY = v[(opcode & 0x00F0) >> 4] % 32;

            //Print Y Coordinate
            cout << hex << "Y " << coordY << endl;
            
            const unsigned short orgX = coordX;

            //V15 = 0
            v[0xF] = 0x0;

            //Extract Nth Byte from Memory
            //D01F -> 1101 0000 0001 1111
            //& 0x000F -> 0000 0000 0000 1111
            //Result: 0000 1111 (15)
            rows = opcode & 0x000F;

            //Print Rows
            cout << hex << "Rows " << rows << endl;

            for (int i = 0; i < rows; i++)
            {
                coordX = orgX;
                spriteRow = memory[index + i];
                for (int j = 0; j < 8; j++)
                {
                    if(coordX < 64 && coordY < 32) //Check for bounds
                    { 
                        unsigned int pixel = (spriteRow >> 7 - j) & 1;

                        if (pixel)
                        {
                            if(display[coordX][coordY] == 1)
                            {
                                v[15] = 1;
                                display[coordX][coordY] = 0;
                            } else {
                                display[coordX][coordY] = 1;
                            } 
                        }
                        coordX++;
                    } 
                }
                coordY++;
            }

            drawFlag = true;
        }



};


int main(int argv, char** args)
{   

  

    Graphics graphics = Graphics();
    graphics.init(); //SDL + IMGUI

    Chip8 chip8 = Chip8(graphics);
    chip8.init();

    //Load ROM
    chip8.loadROM("E:\\Personal\\Projects\\chip8-emulator\\roms\\ibm.ch8");

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
            if(graphics.imguiInit){
                ImGui_ImplSDL2_ProcessEvent(&event);
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
            graphics.drawDisplay(chip8.display);
        }
        


      
        
    }

    SDL_DestroyRenderer(graphics.renderer);
    SDL_DestroyWindow(graphics.window);
    SDL_Quit();

    return 0;
}