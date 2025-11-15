#pragma once

#include <cstdint>
#include <array>
#include <fstream>
#include <random>
#include <iostream>

#define MEMORY_SIZE 0x1000
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define START_ADDRESS 0x200

class Chip8CPU {
    std::array<uint8_t, MEMORY_SIZE> memory;
    std::array<uint32_t, SCREEN_HEIGHT * SCREEN_WIDTH> displayMemory;   // potenzialmente ottimizzabile (1 byte per 8 pixels)
    std::array<uint16_t, 16> stack;
    std::array<uint8_t, 16> v;
    std::array<uint8_t, 16> keypad;
    uint16_t pc;
    uint16_t index;
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t opcode;
    // tabella con puntatori a funzione per ogni opcode
    typedef void (Chip8CPU::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

public:
    Chip8CPU();
    void reset();
    void loadROM(const char* filename);
    void cycle();
    uint32_t* getDisplayMemory();
    void setKeypad(int i, int value);

private:
    void loadCharacters();
    void writeOnMemory(uint16_t addr, uint8_t byte);
    uint8_t randomByte();

    // OPCODE
    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();

    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void noOperation();
}; 