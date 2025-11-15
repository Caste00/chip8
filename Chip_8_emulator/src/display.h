#pragma once

#include "cpu.h"
#include <SDL2/SDL.h>
#include <iostream>

#define DISPLAY_SCALE 20

class Display {
    Chip8CPU &cpu;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
public: 
    Display(Chip8CPU &cpu);
    ~Display();
    void update();
    bool processInput();
};
