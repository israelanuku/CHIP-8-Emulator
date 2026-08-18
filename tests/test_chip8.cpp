#include "Chip8.h"
#include <iostream>
#include <cstdlib>

void test_register_assignment()
{
    Chip8 chip8;

    chip8.execute_opcode(0x603C);

    if (chip8.get_register(0) != 60) { // is V0 = 60? 0x3C is 60
        std::cerr << "FAIL: test_register_assignment\n";
        std::exit(1); // indicate something went wrong
    }

    std::cout << "PASS: test_register_assignment\n";
}

void test_register_addition()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // makes V[0] = 10
    chip8.execute_opcode(0x7005); // adds last 2 digits to V[0], so V0 +=5

    if (chip8.get_register(0) != 15) { // V0 = 15
        std::cerr << "FAIL: test_register_addition\n";
        std::exit(1);
    }

    std::cout << "PASS: test_register_addition\n";
}

int main()
{
    test_register_assignment();
    test_register_addition();

    std::cout << "All tests passed!\n";

    return 0;
}