#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "Chip8.h"
#include <iostream>

int get_chip8_key(SDL_Keycode key)
{
    // turn the keyboard key into chip 8 equivalents:
    // 1  2  3  C
    // 4  5  6  D
    // 7  8  9  E
    // A  0  B  F

    switch (key) {
        case SDLK_1: return 1;
        case SDLK_2: return 2;
        case SDLK_3: return 3;
        case SDLK_4: return 12;

        case SDLK_q: return 4;
        case SDLK_w: return 5;
        case SDLK_e: return 6;
        case SDLK_r: return 13;

        case SDLK_a: return 7;
        case SDLK_s: return 8;
        case SDLK_d: return 9;
        case SDLK_f: return 14;

        case SDLK_z: return 10;
        case SDLK_x: return 0;
        case SDLK_c: return 11;
        case SDLK_v: return 15;

        default: return -1;
    }
}

// Square-wave audio generation based on concepts from SDL2 Audio API documentation:
// https://wiki.libsdl.org/SDL2/CategoryAudio
void generate_beep(float* buffer, int samples, int sample_rate)
{
    const float frequency = 440.0f; // pitch
    const float amplitude = 0.2f; // volume

    static float phase = 0.0f;

    for (int i = 0; i < samples; ++i) {
        buffer[i] = (phase < 0.5f) ? amplitude : -amplitude;

        phase += frequency / sample_rate;

        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}

int main()
{
    Chip8 chip8;
    chip8.load_rom("roms/sound_test.ch8");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { // make sure to initialize video graphics and sound
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
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

    SDL_AudioSpec audio_spec{};

    audio_spec.freq = 44100; // 44,100 audio samples per second
    audio_spec.format = AUDIO_F32SYS; // each sample is a 32-bit floating-point value
    audio_spec.channels = 1;
    audio_spec.samples = 512;
    audio_spec.callback = nullptr;

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(
        nullptr,
        0,
        &audio_spec,
        nullptr,
        0
    );

    if (audio_device == 0) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << '\n';

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudioDevice(audio_device, 0);

    bool running{true};
    std::uint32_t last_timer_tick = SDL_GetTicks();
    std::uint32_t last_cpu_tick = SDL_GetTicks();
    std::uint32_t last_render_tick = SDL_GetTicks();

    const std::uint32_t timer_interval = 1000 / 60;
    const std::uint32_t cpu_interval = 1000 / 500;
    const std::uint32_t render_interval = 1000 / 60;

    while(running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            else if (event.type == SDL_KEYDOWN or event.type == SDL_KEYUP) {
                int key = get_chip8_key(event.key.keysym.sym);
                if (key != -1) {
                    chip8.set_key(key, event.type == SDL_KEYDOWN);
                }
               
            }
        }
        
        std::uint32_t current_time = SDL_GetTicks();

        if (current_time - last_timer_tick >= timer_interval) {
            last_timer_tick = current_time;
            chip8.tick_timers();
        }

        if (!chip8.is_waiting_for_key() &&
            current_time - last_cpu_tick >= cpu_interval) {

            last_cpu_tick = current_time;
            chip8.cycle();
        }

        if (chip8.get_sound_timer() > 0 &&  SDL_GetQueuedAudioSize(audio_device) < 2048) { // Only give SDL another chunk if there isn't already enough audio waiting

            float audio_buffer[512];

            generate_beep(audio_buffer, 512, 44100);

            SDL_QueueAudio(
                audio_device,
                audio_buffer,
                sizeof(audio_buffer)
            );
        }

        if (current_time - last_render_tick >= render_interval) {
            last_render_tick = current_time;
        
            // RENDER

            //clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            const auto& display = chip8.get_display();
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
        }


        
    }

    SDL_CloseAudioDevice(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}