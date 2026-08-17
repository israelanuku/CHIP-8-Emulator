#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "Chip8.h"
#include <iostream>

int main()
{
    Chip8 chip8;
    chip8.load_rom("roms/test.ch8");
    chip8.test_display();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: "
                  << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        320,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: "
                  << SDL_GetError() << '\n';

        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: "
                  << SDL_GetError() << '\n';

        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL window and renderer created successfully!\n";

    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const auto& display = chip8.get_display();
    std::cout << "Pixel 0: " << static_cast<int>(display[0]) << '\n';
    std::cout << "Pixel 1: " << static_cast<int>(display[1]) << '\n';
    std::cout << "Pixel 64: " << static_cast<int>(display[64]) << '\n';
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int y{0}; y < 32; y++) {
        for (int x{0}; x <64; x++) {
                std::size_t index = 64 * y + x;

                if (display[index]) { // if this pixel is ON
                    SDL_Rect pixel{
                        x * 10, // scale the 64 to 640
                        y * 10,
                        10,
                        10
                    };
                    
                    SDL_RenderFillRect(renderer, &pixel);
                }


        }
    }

    SDL_RenderPresent(renderer);

    SDL_Delay(3000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}