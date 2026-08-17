#include "Chip8.h"
#include <algorithm> // for std::fill
#include <cstdint>
#include <fstream>
#include <iostream>

std::uint16_t Chip8::fetch_opcode()
{
    std::uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
    return opcode;
}

void Chip8::execute_opcode(std::uint16_t opcode)
{
    const std::uint8_t instruction = opcode >> 12; //1st digit (from hexademical form) being extracted
    const std::uint8_t x = (opcode >> 8) & 0xF; // 2nd digit (from hexademical form) being extracted
    const std::uint8_t value = opcode & 0xFF; // last 2 digits (from hexademical form) being extracted

    const std::uint8_t y = (opcode >> 4) & 0xF; // 3rd digit being (from hexademical form) being extracted
    const std::uint8_t n = opcode & 0xF; // last digit (from hexademical form) being extracted
    const std::uint16_t address = opcode & 0xFFF; // Keep the lowest 12 bits of the opcode (the NNN portion)

    switch(instruction) {
        case 0x0:
            // 00E0 / 00EE
            if (opcode == 0x00E0) {
                std::fill(display.begin(), display.end(), 0); // clear the display
            }

            else if (opcode == 0x00EE) {
                --sp;
                pc = stack[sp];
            }
            break;

        case 0x1:
            // 1NNN
            pc = address;
            break;

        case 0x2:
            // 2NNN
            stack[sp] = pc;
            sp++;
            pc = address;
            break;

        case 0x3:
            // 3XNN
            if (V[x] == value)
                pc += 2;
            break;

        case 0x4:
            // 4XNN
            if (V[x] != value)
                pc += 2;
            break;

        case 0x5:
            // 5XY0
            if (n == 0 && V[x] == V[y])
                pc += 2;
            break;

        case 0x6:
            // 6XNN
            V[x] = value;
            break;

        case 0x7:
            // 7XNN
            V[x] += value;
            break;

        case 0x8:
            // 8XY0 through 8XYE
            switch(n) {
                case 0x0:
                    // 8XY0
                    V[x] = V[y];
                    break;

                case 0x1:
                    // 8XY1
                    V[x] |= V[y];
                    break;

                case 0x2:
                    // 8XY2
                    V[x] &= V[y]; // bitwise AND operation
                    break;

                case 0x3:
                    // 8XY3
                    V[x] ^= V[y]; // produces 1 when the bits are different and 0 when they are the same
                    break;

                case 0x4:
                    // 8XY4
                    {
                        std::uint16_t sum = V[x] + V[y];
                        V[0xF] = (sum > 255) ? 1 : 0; // did we have a carry? (1 if carry, 0 if no carry)
                        V[x] = sum & 0xFF; // Store the result in VX with only the least significant 8 bits
                    }
                    break;

                case 0x5:
                    // 8XY5
                    V[0xF] = (V[x] > V[y]) ? 1 : 0; // Tells us if borrow occurred (1 if no borrow, 0 if borrow)
                    V[x] -= V[y];
                    break;

                case 0x6:
                    // 8XY6
                    V[0xF] = V[x] & 0x1; // Store least significant bit in VF (rightmost)
                    V[x] >>= 1;
                    break;

                case 0x7:
                    // 8XY7
                    V[0xF] = (V[y] > V[x]) ? 1 : 0; 
                    V[x] = V[y] - V[x];
                    break;

                case 0xE:
                    // 8XYE
                    V[0xF] = (V[x] >> 7) & 0x1; // Store most significant bit in VF (leftmost)
                    V[x] <<= 1;
            }
            break;

        case 0x9:
            // 9XY0
            if (n == 0 && V[x] != V[y])
                pc += 2;
            break;

        case 0xA:
            // ANNN
            I = address; //dissassociating the sprite with the address
            break;

        case 0xB:
            // BNNN
            pc = address + V[0];
            break;

        case 0xC:
            // CXNN
            V[x] = (std::rand() % 256) & value; // Generate a random 8 bit number, then AND it with the value (last 2 digits of the opcode)
            break;

        case 0xD:
            // DXYN - drawing an n-byte sprite at (vx, vy) - reference - D235 : x = v[2], y = v[3], number of rows = 5
            

            V[0xF] = 0;

            for (std::uint8_t row{0};  row < n; row++ ) { // each row contains 8 bits
                

                // Get the current row of the sprite from memory.
                // Each row is one byte containing 8 pixels.
                std::uint8_t spriterow{memory[I + row]};

                for (std::uint8_t column{0}; column < 8; column++) {

                    //Check whether the current sprite bit is non-zero so we can draw
                    bool check = (spriterow & (0x80 >> column)) != 0; // 0x80 is 10000000 - moved it to the right according to the element in the row
                    //example:
                    //sprite = 1111 1111
                    //0x80 = 1000 0000, changed to 0100 0000 because we were in row 1
                    //we can check if the value of a singular bit is 1 or 0 - therefore, OFF/ON

                    if(check) { // does the sprite contain a pixel here?
                        std::uint8_t xpos = (V[x] + column) % 64;
                        std::uint8_t ypos = (V[y] + row) % 32;

                        // Convert 2D (x, y) coordinates into a 1D display index.
                        // Each display row contains 64 pixels.

                        std::size_t index = 64 * ypos + xpos;

                        if (display[index] == 1) { //is there already a pixel on the screen here?
                            V[0xF] = 1; //notify VF, to track collisions

                        }
                        display[index] ^= 1; //XOR'ing it to turn it off if it was already on
                        
                        
                        

                    }






                }
                
            }
            break;

        case 0xE:
            // EX9E / EXA1 : Skip the next instruction depending on key pressed
            if (value == 0x9E) {
                if(keypad[V[x]]) { //if key is pressed
                    pc += 2;
                }
            }
            else if (value == 0xA1) {
                if(!keypad[V[x]]) { //if key isnt pressed
                    pc += 2;
                }
            }

            break;

        case 0xF:
            // FX07, FX0A, FX15, etc. note they all start with F3
            switch(value) {
                case 0x07:
                    V[x] = delay_timer;
                    break;

                case 0x15:
                    delay_timer = V[x];
                    break;
                
                case 0x18:
                    sound_timer = V[x];
                    break;
                
                case 0x1E:
                    I += V[x];
                    break;

                case 0x33:
                    memory[I] = V[x] / 100;
                    memory[I + 1] = (V[x] /10) % 10;
                    memory[I + 2] = V[x] % 10;
                    break;
                
                case 0x55:
                    for(int i{0}; i <= x; i++) {
                        memory[I + i] = V[i];
                    }
                    break;
                
                case 0x65:
                    for(int i{0}; i <= x; i++) {
                        V[i] = memory[I + i];
                    }
                    break;

                case 0x0A:
                    waiting_for_key = true;
                    key_register = x; 
                    break;

            }

        break;

        default:
            // Invalid opcode
            break;
    }
}

void Chip8::cycle() {
    std::uint16_t opcode{fetch_opcode()};
    execute_opcode(opcode);
}

void Chip8::load_rom(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary); //for raw binary numbers, since it won't be a text file

    if (!file) {
        std::cerr << "Failed to open ROM\n";
        return;
    }

    file.read(
        reinterpret_cast<char*>(&memory[0x200]), //file.read requires char*, so change it from an 8 bit int
        memory.size() - 0x200
    ); //where to put data, how many bytes to read


}

std::uint8_t Chip8::get_memory(std::uint16_t address) {
    return memory[address];
}

std::uint16_t Chip8::get_pc() {
    return pc;
}

std::uint8_t Chip8::get_register(std::uint8_t index) {
    return V[index];
}

const std::array<std::uint8_t, 64*32>& Chip8::get_display() const {
    return display;
}

void Chip8::test_display() {
    // 3x3 test display

    display[0] = 1;
    display[1] = 1;
    display[2] = 1;
    display[64] = 1;
    display[65] = 1;
    display[66] = 1;
    display[128] = 1;
    display[129] = 1;
    display[130] = 1;
}

void Chip8::set_key(std::uint8_t key, bool pressed) {
    keypad[key] = pressed;

    if (waiting_for_key && pressed) {
        waiting_for_key = false;
        V[key_register] = key; // set the register corresponding to the 2nd digit in the opcode to key pressed
    }
}

bool Chip8::is_waiting_for_key() const {
    return waiting_for_key;
}

void Chip8::tick_timers() {
    if (sound_timer > 0) {
        --sound_timer;
    }

    std::cout << "BEFORE: " << static_cast<int>(delay_timer) << '\n';

    if (delay_timer > 0) {
        --delay_timer;
    }

    std::cout << "AFTER: " << static_cast<int>(delay_timer) << '\n';

}

std::uint8_t Chip8::get_delay_timer() const
{
    return delay_timer;
}