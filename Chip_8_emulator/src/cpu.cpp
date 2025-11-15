#include "cpu.h"

void Chip8CPU::Table0() {
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8CPU::Table8() {
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8CPU::TableE() {
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8CPU::TableF() {
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8CPU::noOperation() {}

Chip8CPU::Chip8CPU() {
    reset();

    table[0x0] = &Chip8CPU::Table0;
    table[0x1] = &Chip8CPU::OP_1nnn;
    table[0x2] = &Chip8CPU::OP_2nnn;
    table[0x3] = &Chip8CPU::OP_3xkk;
    table[0x4] = &Chip8CPU::OP_4xkk;
    table[0x5] = &Chip8CPU::OP_5xy0;
    table[0x6] = &Chip8CPU::OP_6xkk;
    table[0x7] = &Chip8CPU::OP_7xkk;
    table[0x8] = &Chip8CPU::Table8;
    table[0x9] = &Chip8CPU::OP_9xy0;
    table[0xA] = &Chip8CPU::OP_Annn;
    table[0xB] = &Chip8CPU::OP_Bnnn;
    table[0xC] = &Chip8CPU::OP_Cxkk;
    table[0xD] = &Chip8CPU::OP_Dxyn;
    table[0xE] = &Chip8CPU::TableE;
    table[0xF] = &Chip8CPU::TableF;

    for (int i = 0; i <= 0xE; i++) {
        table0[i] = &Chip8CPU::noOperation;
        table8[i] = &Chip8CPU::noOperation;
        tableE[i] = &Chip8CPU::noOperation;
    }

    table0[0x0] = &Chip8CPU::OP_00E0;
    table0[0xE] = &Chip8CPU::OP_00EE;

    table8[0x0] = &Chip8CPU::OP_8xy0;
    table8[0x1] = &Chip8CPU::OP_8xy1;
    table8[0x2 ]= &Chip8CPU::OP_8xy2;
    table8[0x3] = &Chip8CPU::OP_8xy3;
    table8[0x4] = &Chip8CPU::OP_8xy4;
    table8[0x5] = &Chip8CPU::OP_8xy5;
    table8[0x6] = &Chip8CPU::OP_8xy6;
    table8[0x7] = &Chip8CPU::OP_8xy7;
    table8[0xE] = &Chip8CPU::OP_8xyE;

    tableE[0xE] = &Chip8CPU::OP_Ex9E;
    tableE[0x1] = &Chip8CPU::OP_ExA1;

    for (int i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8CPU::noOperation;
    }

    tableF[0x07] = &Chip8CPU::OP_Fx07;
    tableF[0x0A] = &Chip8CPU::OP_Fx0A;
    tableF[0x15] = &Chip8CPU::OP_Fx15;
    tableF[0x18] = &Chip8CPU::OP_Fx18;
    tableF[0x1E] = &Chip8CPU::OP_Fx1E;
    tableF[0x29] = &Chip8CPU::OP_Fx29;
    tableF[0x33] = &Chip8CPU::OP_Fx33;
    tableF[0x55] = &Chip8CPU::OP_Fx55;
    tableF[0x65] = &Chip8CPU::OP_Fx65;
}

void Chip8CPU::loadCharacters() {
    const uint8_t fontset[80] = {
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

    for (int i = 0; i < 80; i++) {
        memory[0x50 + i] = fontset[i];
    }
}

void Chip8CPU::reset() {
    memory.fill(0);
    displayMemory.fill(0);
    stack.fill(0);
    v.fill(0);
    keypad.fill(0);
    pc = START_ADDRESS;
    sp = 0;
    index = 0;
    loadCharacters();
    // delay_timer -> se caricato con un valore viene decrementato con frequenza 60Hz
    delay_timer = 0;
    // sound_timer -> come delay_timer ma suona se non è zero
    sound_timer = 0;
}

void Chip8CPU::writeOnMemory(uint16_t addr, uint8_t byte) {
    if (addr < START_ADDRESS || addr >= MEMORY_SIZE) {
        throw std::runtime_error("Invalid address");
    }
    memory[addr] = byte;
}

uint8_t Chip8CPU::randomByte() {
    static std::mt19937 gen(std::random_device{}());    // random seed for randomByte()
    static std::uniform_int_distribution<uint8_t> dist(0, 255u);
    
    return static_cast<uint8_t>(dist(gen));
}

void Chip8CPU::loadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        uint8_t byte;
        uint16_t addr = START_ADDRESS;

        while (file.read(reinterpret_cast<char*>(&byte), 1)) {
            if (addr >= MEMORY_SIZE) {
                throw std::runtime_error("ROM excede memory");
            }
            writeOnMemory(addr++, byte);
        }
    } else {
        throw std::runtime_error("Error file not found");
    }
}

void Chip8CPU::cycle() {
    if (pc + 1 >= MEMORY_SIZE) {
        throw std::runtime_error("PC out of bounds during fetch");
    }
    // fetch
    opcode = (memory[pc] << 8) | memory[pc+1];
    pc += 2;

    // decode
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // timer and sound
    if (delay_timer > 0) {
        delay_timer--;
    }
    if (sound_timer > 0) {
        sound_timer--;
    }
}

void Chip8CPU::setKeypad(int i, int value) {
    keypad[i] = value;
}

uint32_t* Chip8CPU::getDisplayMemory() {
    return displayMemory.data();
}

void Chip8CPU::OP_00E0() {
    displayMemory.fill(0);
}

void Chip8CPU::OP_00EE() {
    if (sp == 0) {
        throw std::runtime_error("Stack underflow on RET (00EE)");
    }
    pc = stack[--sp];
}

void Chip8CPU::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8CPU::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = pc;
    sp++;
    pc = address;
}

void Chip8CPU::OP_3xkk() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    
    if (v[x] == byte) {
        pc += 2;
    }
}

void Chip8CPU::OP_4xkk() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (v[x] != byte) {
        pc += 2;
    }
}

void Chip8CPU::OP_5xy0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (v[x] == v[y]) {
        pc += 2;
    }
}

void Chip8CPU::OP_6xkk() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    
    v[x] = byte;
}

void Chip8CPU::OP_7xkk() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    
    v[x] += byte;
}

void Chip8CPU::OP_8xy0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    
    v[x] = v[y];
}

void Chip8CPU::OP_8xy1() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    
    v[x] |= v[y]; 
}

void Chip8CPU::OP_8xy2() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    
    v[x] &= v[y];
}

void Chip8CPU::OP_8xy3() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    
    v[x] ^= v[y];
}

void Chip8CPU::OP_8xy4() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = v[x] + v[y];

    v[0xF] = sum >> 8;
    v[x] = sum & 0xFFu;
}

void Chip8CPU::OP_8xy5() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
 
    if (v[x] > v[y]) {
        v[0xF] = 1;
    } else {
        v[0xF] = 0;
    }

    v[x] -= v[y];
}

void Chip8CPU::OP_8xy6() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    v[0xF] = v[x] & 0x1u;
    v[x] >>= 1;
}

void Chip8CPU::OP_8xy7() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (v[y] > v[x]) {
        v[0xF] = 1;
    } else {
        v[0xF] = 0;
    }

    v[x] = v[y] - v[x];
} 

void Chip8CPU::OP_8xyE() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    
    v[0xF] = (v[x] & 0x80) >> 7;
    v[x] <<= 1;
}

void Chip8CPU::OP_9xy0() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;

    if (v[x] == v[y]) {
        pc += 2;
    }
}

void Chip8CPU::OP_Annn() {
    uint16_t address = opcode & 0x0FFF;
    index = address;
}

void Chip8CPU::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFF;
    pc = address + v[0];
}

void Chip8CPU::OP_Cxkk() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    v[x] = randomByte() & byte;
}

void Chip8CPU::OP_Dxyn() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t y = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000F;
    uint8_t xPos = v[x] % SCREEN_WIDTH;
    uint8_t yPos = v[y] % SCREEN_HEIGHT;
    v[0xF] = 0;

    for (int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[index + row];
        for (int col = 0; col < 8; col++) {
            if ((spriteByte & (0x80u >> col)) != 0) {
                int sx = (xPos + col) % SCREEN_WIDTH;
                int sy = (yPos + row) % SCREEN_HEIGHT;
                size_t idx = sy * SCREEN_WIDTH + sx;
                uint32_t &screenPixel = displayMemory[idx];
                if (screenPixel == 0xFFFFFFFFu) {
                    v[0xF] = 1;
                }
                screenPixel ^= 0xFFFFFFFFu;
            }
        }
    }
}

void Chip8CPU::OP_Ex9E() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = v[x];

    if (keypad[key]) {
        pc += 2;
    }
}

void Chip8CPU::OP_ExA1() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t key = v[x];

    if (!keypad[key]) {
        pc += 2;
    }
}

void Chip8CPU::OP_Fx07() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    v[x] = delay_timer;
}

void Chip8CPU::OP_Fx0A() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    if (keypad[0]) 
        v[x] = 0;
    else if (keypad[1])
        v[x] = 1;
    else if (keypad[2])
        v[x] = 2;
    else if (keypad[3])
        v[x] = 3;
    else if (keypad[4])
        v[x] = 4;
    else if (keypad[5])
        v[x] = 5;
    else if (keypad[6])
        v[x] = 6;
    else if (keypad[7])
        v[x] = 7;
    else if (keypad[8])
        v[x] = 8;
    else if (keypad[9])
        v[x] = 9;
    else if (keypad[10])
        v[x] = 10;
    else if (keypad[11])
        v[x] = 11;
    else if (keypad[12])
        v[x] = 12;
    else if (keypad[13])
        v[x] = 13;
    else if (keypad[14])
        v[x] = 14;
    else if (keypad[15])
        v[x] = 15;
    else
        pc -= 2;
}

void Chip8CPU::OP_Fx15() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    delay_timer = v[x];
}

void Chip8CPU::OP_Fx18() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    sound_timer = v[x];
}

void Chip8CPU::OP_Fx1E() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    index += v[x];
}

void Chip8CPU::OP_Fx29() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t character = v[x];

    index = 0x50 + (character * 5);
}

void Chip8CPU::OP_Fx33() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t value = v[x];

    memory[index + 2] = value % 10;
    value /= 10;
    memory[index + 1] = value % 10;
    value /= 10;
    memory[index] = value % 10;
}

void Chip8CPU::OP_Fx55() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= x; i++) {
        memory[index + i] = v[i];
    }
}

void Chip8CPU::OP_Fx65() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= x; i++) {
        v[i] = memory[index + i];
    }
}