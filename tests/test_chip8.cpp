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

void test_addition_with_carry() {
    Chip8 chip8;

    // V0 = 200
    chip8.execute_opcode(0x60C8);

    // V1 = 100
    chip8.execute_opcode(0x6164);

    // V0 = V0 + V1
    chip8.execute_opcode(0x8014);

    // 200 + 100 = 300
    // CHIP-8 stores only the lower 8 bits: sum & 0xFF binary addition
    // 300 - 256 = 44
    if (chip8.get_register(0) != 44) { // is V0 44?
        std::cerr << "FAIL: V0 should be 44\n";
        std::exit(1);
    }

    // VF should indicate that a carry occurred
    if (chip8.get_register(0xF) != 1) {
        std::cerr << "FAIL: VF should be 1 after carry\n";
        std::exit(1);
    }

    std::cout << "PASS: test_addition_with_carry\n";
}

void test_addition_without_carry()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x6105); // V1 = 5
    chip8.execute_opcode(0x8014); // V0 += V1

    if (chip8.get_register(0) != 15) {
        std::cerr << "FAIL: V0 should be 15\n";
        std::exit(1);
    }

    if (chip8.get_register(0xF) != 0) {
        std::cerr << "FAIL: VF should be 0 without carry\n";
        std::exit(1);
    }

    std::cout << "PASS: test_addition_without_carry\n";
}

int main()
{
    test_register_assignment();
    test_register_addition();
    test_addition_with_carry();
    test_addition_without_carry();

    std::cout << "All tests passed!\n";

    return 0;
}