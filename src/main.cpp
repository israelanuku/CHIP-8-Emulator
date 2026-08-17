#include "Chip8.h"
#include <iostream>

int main()
{
    Chip8 chip8;

    chip8.load_rom("roms/test.ch8");

    std::cout << std::hex;

    // Before doing anything
    std::cout << "PC before cycles: 0x"
              << chip8.get_pc() << '\n';

    // Cycle 1: fetch and execute 0x00E0
    chip8.cycle();

    std::cout << "PC after cycle 1: 0x"
              << chip8.get_pc() << '\n';

    // Show the next instruction in memory
    std::cout << "Memory[0x202]: 0x"
              << static_cast<int>(chip8.get_memory(0x202)) << '\n';

    std::cout << "Memory[0x203]: 0x"
              << static_cast<int>(chip8.get_memory(0x203)) << '\n';

    // Cycle 2: fetch and execute the instruction at 0x202
    chip8.cycle();

    std::cout << "PC after cycle 2: 0x"
              << chip8.get_pc() << '\n';

    return 0;
}