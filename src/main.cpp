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

// square-wave audio generation based on concepts from SDL2 Audio API documentation:
// https://wiki.libsdl.org/SDL2/CategoryAudio
void generate_beep(float* buffer, int samples, int sample_rate)
{
    const float frequency = 440.0f; // pitch of the beep in hz
    const float amplitude = 0.2f; // controls how loud the samples are

    // phase keeps track of where we currently are in the sound wave
    // static means it keeps its value between calls to this function
    static float phase = 0.0f;

    for (int i = 0; i < samples; ++i) {

        // create a square wave by switching between a positive and negative amplitude
        // the first half of the wave is positive and the second half is negative
        buffer[i] = (phase < 0.5f) ? amplitude : -amplitude;

        // move through the wave based on the frequency and sample rate
        // a higher frequency makes the pitch higher
        phase += frequency / sample_rate;

        // once we reach the end of one wave cycle, start again from the beginning
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}

int main()
{
    Chip8 chip8;
    chip8.load_rom("roms/superpong.ch8");

    // initialize both the video and audio systems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
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

    // configure the audio device
    SDL_AudioSpec audio_spec{};

    audio_spec.freq = 44100; // 44,100 audio samples per second
    audio_spec.format = AUDIO_F32SYS; // each sample is a 32-bit floating-point value
    audio_spec.channels = 1; // mono audio
    audio_spec.samples = 512; // number of samples processed at a time
    audio_spec.callback = nullptr; // we will manually queue audio instead

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

    // start playing audio from the device's queue
    SDL_PauseAudioDevice(audio_device, 0);

    bool running{true};

    // keep separate timers for the cpu, chip-8 timers, and screen rendering
    std::uint32_t last_timer_tick = SDL_GetTicks();
    std::uint32_t last_cpu_tick = SDL_GetTicks();
    std::uint32_t last_render_tick = SDL_GetTicks();

    // chip-8 timers run at 60 hz
    // cpu runs at 500 hz
    // screen renders at 60 hz
    const std::uint32_t timer_interval = 1000 / 60;
    const std::uint32_t cpu_interval = 1000 / 500;
    const std::uint32_t render_interval = 1000 / 60;

    while(running) {
        SDL_Event event;

        // process all events that have happened since the previous loop
        while (SDL_PollEvent(&event)) {

            // close the emulator when the window's close button is pressed
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // handle both key presses and key releases
            else if (event.type == SDL_KEYDOWN or event.type == SDL_KEYUP) {
                int key = get_chip8_key(event.key.keysym.sym);

                // ignore keyboard keys that are not part of the chip-8 keypad
                if (key != -1) {

                    // true when pressed, false when released
                    chip8.set_key(key, event.type == SDL_KEYDOWN);
                }
            }
        }

        std::uint32_t current_time = SDL_GetTicks();

        // update the delay and sound timers at exactly 60 hz
        if (current_time - last_timer_tick >= timer_interval) {
            last_timer_tick = current_time;
            chip8.tick_timers();
        }

        // execute chip-8 instructions at the cpu frequency
        // FX0A pauses execution until a key is pressed
        if (!chip8.is_waiting_for_key() && current_time - last_cpu_tick >= cpu_interval) {

            last_cpu_tick = current_time;
            chip8.cycle();
        }

        // only generate more audio if the chip-8 sound timer is active
        // and the audio queue does not already contain too much audio
        if (chip8.get_sound_timer() > 0 && SDL_GetQueuedAudioSize(audio_device) < 2048) {
            std::cout << "SOUND TIMER: " << static_cast<int>(chip8.get_sound_timer()) << '\n';
            float audio_buffer[512];

            // fill the buffer with one chunk of the square-wave beep
            generate_beep(audio_buffer, 512, 44100);

            // add the generated samples to SDL's audio queue
            SDL_QueueAudio(
                audio_device,
                audio_buffer,
                sizeof(audio_buffer)
            );
        }

        // render the display at 60 hz
        if (current_time - last_render_tick >= render_interval) {
            last_render_tick = current_time;

            // clear the previous frame
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            const auto& display = chip8.get_display();

            // set the drawing color to white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for (int y{0}; y < 32; y++) {
                for (int x{0}; x < 64; x++) {

                    // convert the 2d x/y coordinate into the 1d display array index
                    std::size_t index = 64 * y + x;

                    // only draw pixels that are turned on
                    if (display[index]) {

                        // each chip-8 pixel becomes a 10x10 square
                        // this scales the 64x32 display to 640x320
                        SDL_Rect pixel{
                            x * 10,
                            y * 10,
                            10,
                            10
                        };

                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }

            // send the completed frame to the window
            SDL_RenderPresent(renderer);
        }
    }

    // clean up SDL resources before exiting
    SDL_CloseAudioDevice(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}