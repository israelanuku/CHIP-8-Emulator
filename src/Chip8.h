#pragma once

#include <array>
#include <cstdint>
#include <string>

class Chip8 {
private:
    std::array<std::uint8_t, 4096> memory{};
    std::uint16_t pc{0x200};

    std::array<std::uint8_t, 16> V{}; //represents the 16 registers (V0 to VF) of the Chip-8 system, each capable of holding an 8-bit value.
    std::uint16_t I{0};

    std::array<std::uint16_t, 16> stack{};
    std::uint8_t sp{0};

    std::uint8_t delay_timer{0};
    std::uint8_t sound_timer{0};

    std::array<std::uint8_t, 64 * 32> display{};
    std::array<bool, 16> keypad{};

    bool waiting_for_key{false};
    std::uint8_t key_register{0};

public:
    std::uint16_t fetch_opcode();
    void execute_opcode(std::uint16_t opcode);
    void cycle();
    void load_rom(const std::string& filename);
    std::uint8_t get_memory(std::uint16_t address);
    std::uint16_t get_pc();
};