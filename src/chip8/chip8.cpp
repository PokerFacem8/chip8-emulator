#include <chip8.h>
#include <graphics.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stack>

using namespace std; 


Chip8::Chip8() : graphics() {
    pc = 0x200;
    index = 0x0000;
    delay_timer = 0x000;
    sp = 0;
    sound_timer = 0x000;
    drawFlag = false;
    pressedKey = 0;
    keymap[SDL_SCANCODE_1] = 0x1;
    keymap[SDL_SCANCODE_2] = 0x2;
    keymap[SDL_SCANCODE_3] = 0x3;
    keymap[SDL_SCANCODE_4] = 0xC;
    keymap[SDL_SCANCODE_Q] = 0x4;
    keymap[SDL_SCANCODE_W] = 0x5;
    keymap[SDL_SCANCODE_E] = 0x6;
    keymap[SDL_SCANCODE_R] = 0xD;
    keymap[SDL_SCANCODE_A] = 0x7;
    keymap[SDL_SCANCODE_S] = 0x8;
    keymap[SDL_SCANCODE_D] = 0x9;
    keymap[SDL_SCANCODE_F] = 0xE;
    keymap[SDL_SCANCODE_Z] = 0xA;
    keymap[SDL_SCANCODE_X] = 0x0;
    keymap[SDL_SCANCODE_C] = 0xB;
    keymap[SDL_SCANCODE_V] = 0xF;


    //Load Font
    for (int i = 0; i < 80; i++)
    {
        memory[i] = font[i];
    }

    initGraphics();

    pushLog("Chip8 Initialized");
}

void Chip8::pushLog(string log) {

    /*
        [0] - New Log
        [1] - Old Log
        [2] - Old Log
        [3] - Old Log
        [4] - Old Log
        [5] - Old Log
        [6] - Old Log
        [7] - Old Log
        [8] - Old Log
        [9] - Old Log
        [10] - Old Log
        [11] - Old Log
        [12] - Old Log
        [13] - Old Log
        [14] - Old Log
        [15] - Old Log
    */

    //Inverse Loop (Replace Old Logs)
    for (int i = 49; i > 0; i--)
    {
        console[i] = console[i - 1];
    }
    console[0] = log;
}

void Chip8::initGraphics() {
    graphics.init();
}

void Chip8::pressKey(SDL_Keycode key) {
    pressedKey = keymap[key];
}

void Chip8::loadROM(string fileName) {

    //Open File
    std::ifstream infile(fileName, std::ios::in | std::ios::binary);

    // Get length of file
    infile.seekg(0, std::ios::end);
    size_t length = infile.tellg();
    infile.seekg(0, std::ios::beg);

    char buffer[length];

    // Read file
    infile.read(buffer, length);

    //Load file to Memory [Program Data]
    for (int i = 0; i < length; i++)
    {
        memory[i + 512] = buffer[i];
    }

    //Dump Memory
    //cout << "Dump Memory: " << endl;
    //for (int i = 0; i < length; i++)
    //{
    //    cout << (i + pc) << " - " << hex << (int)memory[i + pc] << endl;
    //}
    //cout << "------------------" << endl;
    //Close File
    infile.close();
}

void Chip8::unLoadROM() {

    //Reset Memory (ROM)
    for (int i = 0; i < 4096; i++)
    {
        memory[i + 512] = 0;
    }

    //Reset Display
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            display[i][j] = 0;
        }
    }

    //Reset Counters/Timers
    pc = 0x200;
    index = 0x0000;
    delay_timer = 0x000;
    sp = 0;
    sound_timer = 0x000;
    drawFlag = false;

}

void Chip8::cycle(){


    unsigned short ipf = 11;

    /*Note: When you add timers (the delay-timer and the sound-timer) 
    they need to be decremented outside that ipf loop or outside 
    cycle in the main frame-loop.*/
    
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
        lastOpcode = opcode;

        //Get First Code from Opcode
        //Opcode :              1010 0000 1111 0000  (0xAF0)
        //& 0xF000:             1111 0000 0000 0000
        //                      1010 0000 0000 0000
        //Shift 12 bits:        0000 0000 0000 1010
        unsigned short code = (opcode & 0xF000) >> 12;

        //PRINT OPCODE
        cout << hex << "Opcode: " << opcode << endl;

        //Increment Program Counter
        pc = pc + 2;

        //----DECODE----
        switch(code) 
        {

            case 0:

                if ((opcode & 0x00FF) == 0xE0){ // CLS (Validated)
                    graphics.clear(display);
                } else if((opcode & 0x00FF) == 0xEE) { // RET
                    sp--;
                    pc = stack[sp];
                }
                break;

            case 1: //(Validated)
                //cout << "Jump" << endl;

                //Extract Address from Opcode
                //Opcode:   0001 0101 1010 0101  (0x15A5)
                //&0x0FFF:  0000 1111 1111 1111 
                //Result:   0000 0000 0101 1010
                //Set Program Counter to Address
                pc = (opcode & 0x0FFF);
                break;

            case 2: //CALL (Validated)
                
                stack[sp] = pc;
                sp++;
                pc = (opcode & 0x0FFF);
                break;

            case 3: //SE Vx, byte (Validated)

                if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                    pc = pc + 2; 
                }
                break;

            case 4: //SNE Vx, byte (Validated)

                if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                    pc = pc + 2; 
                }
                break;

            case 5: //SE Vx, Vy (Validated)

                if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4]) {
                    pc = pc + 2; 
                }
                break;

            case 6: //LD Vx, byte (Validated)
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
                v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
                break;

            case 7: //ADD Vx, byte (Validated)

                v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF);
                break;

            case 8:

                x8instructions(opcode);
                break;

            case 9: //SNE Vx, Vy (Validated)

                if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]){
                    pc = pc + 2;
                }

                break;

            case 0xA: //LD I, addr (Validated)
                //cout << "Set Index" << endl;

                //Set Index Register
                index = (opcode & 0x0FFF);
                break;

            case 0xB: // JP V0, addr (Validate)

                pc = v[0] + (opcode & 0x0FFF);
                break;

            case 0xC: // RND Vx, byte ((Validated??)

                v[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
                break;

            case 0xD: //(Validated)

                draw(opcode);
                break;

            case 0xE:

                if((opcode & 0x00FF) == 0x009E) { // SKP Vx
                    if (v[(opcode & 0x0F00) >> 8] == pressedKey){
                        pc = pc + 2;
                    }
                } else if((opcode & 0x00FF) == 0x00A1) { // SKNP Vx
                    if (v[(opcode & 0x0F00) >> 8] != pressedKey){
                        pc = pc + 2;
                    }
                }
                break;

            case 0xF:

                xFinstructions(opcode);
                break;
            
            default:
                pushLog("Unknown instruction: " + opcode);
                break; 
        }
        ipf--;
    }
}

void Chip8::xFinstructions(unsigned short opcode) {

    unsigned char h = 0;
    unsigned char t = 0;
    unsigned int value = 0;

    switch (opcode & 0x00FF)
    {
        case 0x07: // LD Vx, DT

            v[(opcode & 0x0F00) >> 8] = delay_timer;
            break;

        case 0x0A: // LD Vx, K

            if(pressedKey == 0x0) {
                pc = pc - 2;
            }
            break;
        
        case 0x15: // LD DT, Vx

            delay_timer = v[(opcode & 0x0F00) >> 8];
            break;

        case 0x18: // LD ST, Vx

            sound_timer = v[(opcode & 0x0F00) >> 8];
            break;

        case 0x1E: // ADD I, Vx

            index = index + v[(opcode & 0x0F00) >> 8];
            break;   

        case 0x29: // LD F, Vx

            index = v[(opcode & 0x0F00) >> 8] * 5;
            break;   

        case 0x33: // LD B, Vx  

            memory[index]     = v[(opcode & 0x0F00) >> 8] / 100;
            memory[index + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[index + 2] = v[(opcode & 0x0F00) >> 8] % 10; 
            break;

        case 0x55: // LD [I], Vx 

            value = index;
            for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++){
                memory[index] = v[i];
                index++;
            }
            index = value;
            break;

        case 0x65: // LD Vx, [I]

            value = index;
            for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++){
                v[i] = memory[index];
                index++;
            }
            index = value;
            break;
            
        default:
            pushLog("Couldn't find 0xF instruction: " + opcode);
            break;
    }

}

void Chip8::x8instructions(unsigned short opcode){

   switch (opcode & 0x000F)
    {
        case 0: // LD Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
            break;

        case 1: // OR Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
            break;

        case 2: // AND Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
            break;

        case 3: // XOR Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
            break;

        case 4: //ADD Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];
            v[15] = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4] > 255 ? 1 : 0;
            break;

        case 5: // SUB Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] - v[(opcode & 0x00F0) >> 4];
            v[15] = v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8] ? 0 : 1;
            break;

        case 6: //SHR Vx {, Vy} (Validated)

            v[15] = v[(opcode & 0x0F00) >> 8] & 0x01;
            v[(opcode & 0x0F00) >> 8] >>= 1;
            break;

        case 7: //  SUBN Vx, Vy (Validated)

            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
            v[15] = v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8] ? 1 : 0;
            break;

        case 0xE: //SHL Vx {, Vy} (Validated)

            v[15] = (v[(opcode & 0x00F0) >> 4] >> 7) & 0x1;
            v[(opcode & 0x0F00) >> 8] <<= 1;
            break;

        default:
            pushLog("Couldn't find 0x8 instruction: " + opcode);
            break;
    }

}

void Chip8::draw(unsigned short opcode) {

    unsigned short coordX;
    unsigned short coordY;
    unsigned short rows;
    unsigned char spriteRow;

    //Extract X and Y coordinates from VX and VY
    coordX = v[(opcode & 0x0F00) >> 8] % 64;
    coordY = v[(opcode & 0x00F0) >> 4] % 32;

    const unsigned short orgX = coordX;

    //V15 = 0
    v[0xF] = 0x0;

    //Extract Nth Byte from Memory
    //D01F -> 1101 0000 0001 1111
    //& 0x000F -> 0000 0000 0000 1111
    //Result: 0000 1111 (15)
    rows = opcode & 0x000F;

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

void Chip8::updateDisplay() {
    graphics.drawDisplay(display);
}

void Chip8::destroyGraphics() {
    SDL_DestroyRenderer(graphics.renderer);
    SDL_DestroyWindow(graphics.window);
    SDL_DestroyTexture(graphics.texture);
    SDL_Quit();
}