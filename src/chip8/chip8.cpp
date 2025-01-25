#include <chip8.h>
#include <graphics.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stack>

using namespace std; 


Chip8::Chip8() : graphics(){
    pc = 0x200;
    index = 0x0000;
    delay_timer = 0x000;
    sound_timer = 0x000;
    drawFlag = false;
    initGraphics();
}

void Chip8::initGraphics(){
    graphics.init();
}

void Chip8::loadROM(string fileName){

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

    //Dump Memory
    /*for (int i = 0; i < length; i++)
    {
        cout << hex << (int)memory[i + pc] << endl;
    }*/

    //Close File
    infile.close();
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

        //Get First Code from Opcode
        //Opcode :              1010 0000 1111 0000  (0xAF0)
        //& 0xF000:             1111 0000 0000 0000
        //                      1010 0000 0000 0000
        //Shift 12 bits:        0000 0000 0000 1010
        unsigned short code = (opcode & 0xF000) >> 12;

        //PRINT OPCODE
        cout << hex << "Opcode: " << opcode << endl;

        //PRINT CODE
        //cout << hex << code << endl;  

        //Increment Program Counter
        pc = pc + 2;

        //----DECODE----
        switch(code) 
        {
            case 0x0: //CLS + RET

                if((opcode & 0x000F) == 0){ //CLS
                    graphics.clear(display);
                } else { //RET
                    pc = stack.top();
                    stack.pop();
                }
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

            case 0x2: //CALL

                stack.push(pc);
                pc = opcode & 0x0FFF;
                break;

            case 0x3: //SE Vx, byte

                if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                    pc = pc + 2; 
                }
                break;

            case 0x4: //SNE Vx, byte

                if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                    pc = pc + 2; 
                }
                break;

            case 0x5: //SE Vx, Vy

                if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4]) {
                    pc = pc + 2; 
                }
                break;

            case 0x6: //LD Vx, byte
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

            case 0x7: //ADD Vx, byte

                v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + opcode & 0x00FF;
                break;

            case 0x8:

                switch (opcode & 0x000F)
                {
                    case 0: // LD Vx, Vy

                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
                        break;

                    case 1: // OR Vx, Vy

                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] | v[(opcode & 0x0F00) >> 8];
                        break;

                    case 2: // AND Vx, Vy

                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] & v[(opcode & 0x0F00) >> 8];
                        break;

                    case 3: // XOR Vx, Vy

                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] ^ v[(opcode & 0x0F00) >> 8];
                        break;

                    case 4: //ADD Vx, Vy

                        if ((v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4]) > 0x00FF) {
                            v[(opcode & 0x0F00) >> 8] = (v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4]) & 0x00FF;
                            v[15] = 1;
                        }else {
                            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];
                            v[15] = 0;
                        }
                        break;

                    case 5: // SUB Vx, Vy

                        v[15] = 1;
                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] - v[(opcode & 0x00F0) >> 4];
                        if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]) { // IF Vx < Vy
                            v[15] = 0;
                        }
                        break;

                    case 6:

                        break;

                    case 7: //  SUBN Vx, Vy

                        v[15] = 1;
                        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
                        if(v[(opcode & 0x00F0) >> 4] < v[(opcode & 0x0F00) >> 8]) { // IF Vx > Vy
                            v[15] = 0;
                        }
                        break;



                    default:
                        cout << "OX8 Failed" << endl;
                        break;
                }



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
    SDL_Quit();
}