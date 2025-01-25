// chip8.h
#ifndef chip8_h
#define chip8_h
#include "graphics.h"
#include <iostream>
#include <fstream>
#include <stack>


class Chip8
{
    unsigned char memory[4096]{};       // 4KB of memory
    unsigned short pc;                  // 16-bit program counter
    unsigned short index;               // 16-bit index register
    std::stack<unsigned short> stack;         // 64 16-bit addresses
    unsigned char delay_timer;          // 8-bit delay timer
    unsigned char sound_timer;          // 8-bit sound timer
    unsigned char v[16]{};              // 16 8-bit general-purpose variable registers
    Graphics graphics;

    public:
        unsigned char display[64][32]{};// 64 x 32 monochrome display
        bool drawFlag;                  //Draw Flag

        Chip8();
        void initGraphics();
        void loadROM(std::string fileName);
        void cycle();
        void draw(unsigned short opcode);
        void updateDisplay();
        void destroyGraphics();
};

#endif
