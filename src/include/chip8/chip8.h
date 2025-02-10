// chip8.h
#ifndef chip8_h
#define chip8_h
#include "graphics.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <map>


class Chip8
{
    unsigned char memory[4096]{};       // 4KB of memory
    unsigned short pc;                  // 16-bit program counter
    unsigned short index;               // 16-bit index register
    unsigned short stack[16]{};   // 64 16-bit addresses
    unsigned char sp;

    unsigned char font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };        

    public:
        unsigned char display[64][32]{};// 64 x 32 monochrome display
        unsigned char v[16]{};          // 16 8-bit general-purpose variable registers
        bool drawFlag;                  //Draw Flag
        Graphics graphics;
        unsigned int pressedKey;
        unsigned char delay_timer;          // 8-bit delay timer
        unsigned char sound_timer;          // 8-bit sound timer
        std::map<SDL_Keycode, unsigned int> keymap;
        std::string console[50]{};   // 64 16-bit addresses

        Chip8();
        void initGraphics();
        void pushLog(std::string log);
        void pressKey(SDL_Keycode key);
        void loadROM(std::string fileName);
        void unLoadROM();
        void cycle();
        void xFinstructions(unsigned short opcode);
        void x8instructions(unsigned short opcode);
        void draw(unsigned short opcode);
        void updateDisplay();
        void destroyGraphics();
};

#endif
