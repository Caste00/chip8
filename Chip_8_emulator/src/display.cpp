#include "display.h"

Display::Display(Chip8CPU&   cpu) : cpu(cpu) {
    int err = SDL_Init(SDL_INIT_VIDEO);
    if (err != 0) {
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * DISPLAY_SCALE, SCREEN_HEIGHT * DISPLAY_SCALE, SDL_WINDOW_SHOWN);
    if (!window) {
        throw std::runtime_error("Could not make a window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture) {
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

Display::~Display() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::update() {
    cpu.cycle();
    
    int pitch = SCREEN_WIDTH * static_cast<int>(sizeof(uint32_t));
    SDL_UpdateTexture(texture, nullptr, cpu.getDisplayMemory(), pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Display::processInput() {
    bool quit = false;
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                quit = true;
            } break;
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    } break;
                    case SDLK_x: {
                        cpu.setKeypad(0, 1);
                    } break;
                    case SDLK_1: {
                        cpu.setKeypad(1, 1);
                    } break;
                    case SDLK_2: {
                        cpu.setKeypad(2, 1);
                    } break;
                    case SDLK_3: {
                        cpu.setKeypad(3, 1);
                    } break;
                    case SDLK_q: {
                        cpu.setKeypad(4, 1);
                    } break;
                    case SDLK_w: {
                        cpu.setKeypad(5, 1);
                    } break;
                    case SDLK_e: {
                        cpu.setKeypad(6, 1);
                    } break;
                    case SDLK_a: {
                        cpu.setKeypad(7, 1);
                    } break;
                    case SDLK_s: {
                        cpu.setKeypad(8, 1);
                    } break;
                    case SDLK_d: {
                        cpu.setKeypad(9, 1);
                    } break;
                    case SDLK_z: {
                        cpu.setKeypad(10, 1);
                    } break;
                    case SDLK_c: {
                        cpu.setKeypad(11, 1);
                    } break;
                    case SDLK_4: {
                        cpu.setKeypad(12, 1);
                    } break;
                    case SDLK_r: {
                        cpu.setKeypad(13, 1);
                    } break;
                    case SDLK_f: {
                        cpu.setKeypad(14, 1);
                    } break;
                    case SDLK_v: {
                        cpu.setKeypad(15, 1);
                    } break;
                }
            } break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    } break;
                    case SDLK_x: {
                        cpu.setKeypad(0, 0);
                    } break;
                    case SDLK_1: {
                        cpu.setKeypad(1, 0);
                    } break;
                    case SDLK_2: {
                        cpu.setKeypad(2, 0);
                    } break;
                    case SDLK_3: {
                        cpu.setKeypad(3, 0);
                    } break;
                    case SDLK_q: {
                        cpu.setKeypad(4, 0);
                    } break;
                    case SDLK_w: {
                        cpu.setKeypad(5, 0);
                    } break;
                    case SDLK_e: {
                        cpu.setKeypad(6, 0);
                    } break;
                    case SDLK_a: {
                        cpu.setKeypad(7, 0);
                    } break;
                    case SDLK_s: {
                        cpu.setKeypad(8, 0);
                    } break;
                    case SDLK_d: {
                        cpu.setKeypad(9, 0);
                    } break;
                    case SDLK_z: {
                        cpu.setKeypad(10, 0);
                    } break;
                    case SDLK_c: {
                        cpu.setKeypad(11, 0);
                    } break;
                    case SDLK_4: {
                        cpu.setKeypad(12, 0);
                    } break;
                    case SDLK_r: {
                        cpu.setKeypad(13, 0);
                    } break;
                    case SDLK_f: {
                        cpu.setKeypad(14, 0);
                    } break;
                    case SDLK_v: {
                        cpu.setKeypad(15, 0);
                    } break;
                }   
            } break;
        }
    }

    return quit;
}