#include "../src/Chip8.h"
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

void test_register_copy()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x6114); // V1 = 20
    chip8.execute_opcode(0x8010); // V0 = V1

    if (chip8.get_register(0) != 20) {
        std::cerr << "FAIL: test_register_copy\n";
        std::exit(1);
    }

    std::cout << "PASS: test_register_copy\n";
}

void test_bitwise_or()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600F); // V0 = 00001111
    chip8.execute_opcode(0x61F0); // V1 = 11110000
    chip8.execute_opcode(0x8011); // V0 |= V1

    if (chip8.get_register(0) != 0xFF) { // should be 1111 1111
        std::cerr << "FAIL: test_bitwise_or\n";
        std::exit(1);
    }

    std::cout << "PASS: test_bitwise_or\n";
}

void test_bitwise_and()
{
    Chip8 chip8;

    chip8.execute_opcode(0x60F0); // 11110000
    chip8.execute_opcode(0x615F); // 01011111
    chip8.execute_opcode(0x8012); // V0 &= V1

    if (chip8.get_register(0) != 0x50) { // 0101 0000
        std::cerr << "FAIL: test_bitwise_and\n";
        std::exit(1);
    }

    std::cout << "PASS: test_bitwise_and\n";
}

void test_bitwise_xor()
{
    Chip8 chip8;

    chip8.execute_opcode(0x60FF); // 11111111
    chip8.execute_opcode(0x610F); // 00001111
    chip8.execute_opcode(0x8013); // V0 ^= V1

    if (chip8.get_register(0) != 0xF0) {
        std::cerr << "FAIL: test_bitwise_xor\n";
        std::exit(1);
    }

    std::cout << "PASS: test_bitwise_xor\n";
}

void test_subtraction()
{
    Chip8 chip8;

    // V0 = 10
    chip8.execute_opcode(0x600A);

    // V1 = 3
    chip8.execute_opcode(0x6103);

    // V0 = V0 - V1
    chip8.execute_opcode(0x8015);

    if (chip8.get_register(0) != 7) {
        std::cerr << "FAIL: test_subtraction result\n";
        std::exit(1);
    }

    // VF = 1 because V0 was greater than V1
    if (chip8.get_register(0xF) != 1) {
        std::cerr << "FAIL: test_subtraction VF\n";
        std::exit(1);
    }

    std::cout << "PASS: test_subtraction\n";
}

void test_subtraction_borrow()
{
    Chip8 chip8;

    // V0 = 3
    chip8.execute_opcode(0x6003);

    // V1 = 10
    chip8.execute_opcode(0x610A);

    // V0 = V0 - V1
    chip8.execute_opcode(0x8015);

    if (chip8.get_register(0) != 249) { // uint8_t cant store -7, so it wraps around
        std::cerr << "FAIL: test_subtraction_borrow result\n";
        std::exit(1);
    }

    // VF = 0 because V0 was not greater than V1
    if (chip8.get_register(0xF) != 0) {
        std::cerr << "FAIL: test_subtraction_borrow VF\n";
        std::exit(1);
    }

    std::cout << "PASS: test_subtraction_borrow\n";
}

void test_shift_right()
{
    Chip8 chip8;

    // 00001101 = 13
    chip8.execute_opcode(0x600D);

    // V0 >>= 1
    chip8.execute_opcode(0x8006);

    // 13 >> 1 = 6
    if (chip8.get_register(0) != 6) {
        std::cerr << "FAIL: test_shift_right result\n";
        std::exit(1);
    }

    // original rightmost bit was 1
    if (chip8.get_register(0xF) != 1) {
        std::cerr << "FAIL: test_shift_right VF\n";
        std::exit(1);
    }

    std::cout << "PASS: test_shift_right\n";
}

void test_reverse_subtraction()
{
    Chip8 chip8;

    // V0 = 3
    chip8.execute_opcode(0x6003);

    // V1 = 10
    chip8.execute_opcode(0x610A);

    // V0 = V1 - V0
    chip8.execute_opcode(0x8017);

    if (chip8.get_register(0) != 7) {
        std::cerr << "FAIL: test_reverse_subtraction result\n";
        std::exit(1);
    }

    if (chip8.get_register(0xF) != 1) {
        std::cerr << "FAIL: test_reverse_subtraction VF\n";
        std::exit(1);
    }

    std::cout << "PASS: test_reverse_subtraction\n";
}


void test_shift_left()
{
    Chip8 chip8;

    // 10000001 = 129
    chip8.execute_opcode(0x6081);

    // V0 <<= 1
    chip8.execute_opcode(0x800E);

    // 10000001 << 1 = 00000010
    if (chip8.get_register(0) != 2) {
        std::cerr << "FAIL: test_shift_left result\n";
        std::exit(1);
    }

    // Original leftmost bit was 1
    if (chip8.get_register(0xF) != 1) {
        std::cerr << "FAIL: test_shift_left VF\n";
        std::exit(1);
    }

    std::cout << "PASS: test_shift_left\n";
}

void test_jump()
{
    Chip8 chip8;

    chip8.execute_opcode(0x1234);

    if (chip8.get_pc() != 0x234) {
        std::cerr << "FAIL: test_jump\n";
        std::exit(1);
    }

    std::cout << "PASS: test_jump\n";
}

void test_skip_if_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x300A); // skip next instruction if V0 == 10

    if (chip8.get_pc() != 0x202) {
        std::cerr << "FAIL: test_skip_if_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_skip_if_equal\n";
}

void test_no_skip_if_not_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x300B); // skip if V0 == 11

    if (chip8.get_pc() != 0x200) {
        std::cerr << "FAIL: test_no_skip_if_not_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_no_skip_if_not_equal\n";
}

void test_skip_if_not_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x400B); // skip if V0 != 11

    if (chip8.get_pc() != 0x202) {
        std::cerr << "FAIL: test_skip_if_not_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_skip_if_not_equal\n";
}

void test_no_skip_if_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x400A); // skip if V0 != 10

    if (chip8.get_pc() != 0x200) {
        std::cerr << "FAIL: test_no_skip_if_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_no_skip_if_equal\n";
}

void test_skip_if_registers_equal() {
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x610A); // V1 = 10
    chip8.execute_opcode(0x5010); // skip if V0 == V1;

    if (chip8.get_pc() != 0x202) {
        std::cerr << "FAIL: test_skip_if_registers_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_skip_if_registers_equal\n";
}

void test_no_skip_if_registers_not_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x610B); // V1 = 11
    chip8.execute_opcode(0x5010); // skip if V0 == V1

    if (chip8.get_pc() != 0x200) {
        std::cerr << "FAIL: test_no_skip_if_registers_not_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_no_skip_if_registers_not_equal\n";
}

void test_skip_if_registers_not_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x610B); // V1 = 11
    chip8.execute_opcode(0x9010); // skip if V0 != V1

    if (chip8.get_pc() != 0x202) {
        std::cerr << "FAIL: test_skip_if_registers_not_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_skip_if_registers_not_equal\n";
}

void test_no_skip_if_registers_equal()
{
    Chip8 chip8;

    chip8.execute_opcode(0x600A); // V0 = 10
    chip8.execute_opcode(0x610A); // V1 = 10
    chip8.execute_opcode(0x9010); // skip if V0 != V1

    if (chip8.get_pc() != 0x200) {
        std::cerr << "FAIL: test_no_skip_if_registers_equal\n";
        std::exit(1);
    }

    std::cout << "PASS: test_no_skip_if_registers_equal\n";
}

void test_jump_with_v0()
{
    Chip8 chip8;

    chip8.execute_opcode(0x6005); // V0 = 5
    chip8.execute_opcode(0xB234); // pc = 0x234 + V0

    if (chip8.get_pc() != 0x239) {
        std::cerr << "FAIL: test_jump_with_v0\n";
        std::exit(1);
    }

    std::cout << "PASS: test_jump_with_v0\n";
}

void test_subroutine_call_and_return()
{
    Chip8 chip8;

    // 0x200: 2208 -> Call subroutine at 0x208
    chip8.set_memory(0x200, 0x22); // memory[0x200] = 0x22
    chip8.set_memory(0x201, 0x08);

    // 0x208: 6105 -> V1 = 5
    chip8.set_memory(0x208, 0x61);
    chip8.set_memory(0x209, 0x05);

    // 0x20A: 00EE -> Return from subroutine
    chip8.set_memory(0x20A, 0x00);
    chip8.set_memory(0x20B, 0xEE);

    // Execute 2208
    chip8.cycle();

    if (chip8.get_pc() != 0x208) {
        std::cerr << "FAIL: subroutine did not jump to 0x208\n";
        std::exit(1);
    }

    // Execute 6105
    chip8.cycle();

    if (chip8.get_register(1) != 5) {
        std::cerr << "FAIL: subroutine did not execute correctly\n";
        std::exit(1);
    }

    // Execute 00EE
    chip8.cycle();

    if (chip8.get_pc() != 0x202) {
        std::cerr << "FAIL: subroutine did not return to 0x202\n";
        std::exit(1);
    }

    std::cout << "PASS: test_subroutine_call_and_return\n";
}

int main()
{
    test_register_assignment();
    test_register_addition();
    test_addition_with_carry();
    test_addition_without_carry();
    test_register_copy();
    test_bitwise_or();
    test_bitwise_and();
    test_bitwise_xor();
    test_subtraction();
    test_subtraction_borrow();
    test_shift_right();
    test_reverse_subtraction();
    test_shift_left();
    test_jump();

    test_skip_if_equal();
    test_no_skip_if_not_equal();

    test_skip_if_not_equal();
    test_no_skip_if_equal();

    test_skip_if_registers_equal();
    test_no_skip_if_registers_not_equal();

    test_skip_if_registers_not_equal();
    test_no_skip_if_registers_equal();

    test_jump_with_v0();
    test_subroutine_call_and_return();

    std::cout << "All tests passed!\n";

    return 0;
}