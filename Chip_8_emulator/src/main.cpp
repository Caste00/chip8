#include "cpu.h"
#include "display.h"
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "You must put the path of a valid ROM and a video delay" << std::endl;
        exit(EXIT_FAILURE);
    }

    char* const romFilename = argv[1];
    int cycleDelay = std::stoi(argv[2]);
    
    Chip8CPU chip8;
    chip8.loadROM(romFilename);
    Display display(chip8);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = display.processInput();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        
        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            display.update();
        }
    }

    return 0;
}