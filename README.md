# CHIP-8 Emulator

A CHIP-8 emulator written in **C++17** with **SDL2** for graphics, keyboard input, and audio.

This project was built to strengthen my understanding of low-level programming concepts including memory management, bitwise operations, instruction decoding, CPU-style fetch/decode/execute cycles, timers, stack-based subroutines, and hardware-style input/output.

## Features

* CHIP-8 CPU emulation
* 4 KB memory
* 16 8-bit general-purpose registers (`V0`–`VF`)
* 16-bit index register (`I`)
* Program counter and stack pointer
* 16-level call stack
* 64 × 32 monochrome display
* CHIP-8 keypad input
* Delay timer running at 60 Hz
* Sound timer running at 60 Hz
* SDL2-based rendering
* SDL2-based square-wave audio
* ROM loading from binary files
* Automated opcode tests

## Supported Instructions

The emulator currently implements the core CHIP-8 instruction set, including:

| Category            | Instructions                   |
| ------------------- | ------------------------------ |
| Display             | `00E0`                         |
| Subroutines         | `00EE`, `2NNN`                 |
| Jumps               | `1NNN`, `BNNN`                 |
| Conditional skips   | `3XNN`, `4XNN`, `5XY0`, `9XY0` |
| Register operations | `6XNN`, `7XNN`                 |
| Arithmetic / logic  | `8XY0`–`8XYE`                  |
| Index register      | `ANNN`, `FX1E`                 |
| Random number       | `CXNN`                         |
| Drawing             | `DXYN`                         |
| Keyboard            | `EX9E`, `EXA1`, `FX0A`         |
| Timers              | `FX07`, `FX15`, `FX18`         |
| Memory / BCD        | `FX33`, `FX55`, `FX65`         |

## Architecture

The emulator is centered around the `Chip8` class.

### Fetch → Decode → Execute

Each CPU cycle follows the CHIP-8 instruction cycle:

```text
Memory
  │
  ▼
Fetch opcode
  │
  ▼
Decode instruction
  │
  ▼
Execute instruction
  │
  ▼
Update CPU state
```

Two bytes are read from memory and combined into a 16-bit opcode:

```cpp
std::uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
pc += 2;
```

The opcode is then broken into its individual components, allowing the emulator to determine which instruction to execute and which registers or values it operates on.

## Memory

CHIP-8 provides 4096 bytes of memory.

Programs are loaded beginning at address `0x200`, which is the standard starting address for CHIP-8 programs.

```text
0x000 ───────────────
       Reserved
0x200 ─────────────── ← ROM starts here
       Program
       ...
0xFFF ───────────────
```

## Registers

The emulator contains sixteen 8-bit registers:

```text
V0 V1 V2 V3 V4 V5 V6 V7
V8 V9 VA VB VC VD VE VF
```

`VF` is also used by several instructions as a flag register, such as indicating arithmetic carries or drawing collisions.

## Stack and Subroutines

CHIP-8 uses a stack to store return addresses when executing subroutines.

For a `2NNN` instruction:

1. The current program counter is stored on the stack.
2. The stack pointer is incremented.
3. The program counter jumps to the subroutine address.

When `00EE` is executed, the stack pointer is decremented and the saved address is restored to the program counter.

This allows subroutines to return to the instruction following the original call.

## Graphics

The CHIP-8 display is 64 × 32.

Sprites are stored as bytes in memory, with each byte representing one row of eight pixels.

The `DXYN` instruction draws the sprite using XOR logic. This also allows the emulator to detect collisions and set `VF` accordingly.

The framebuffer is then rendered through SDL2 and scaled to a larger window (640x320) for visibility.

## Keyboard

The CHIP-8 keypad contains 16 keys:

```text
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

The emulator maps these CHIP-8 keys to a physical keyboard layout:

```text
1 2 3 4
Q W E R
A S D F
Z X C V
```

SDL2 keyboard events are converted into CHIP-8 key states before being passed to the emulator.

## Timers

CHIP-8 has two timers:

* **Delay timer** — general-purpose timer
* **Sound timer** — controls when the CHIP-8 system should produce a sound

Both timers decrement at **60 Hz** until reaching zero.

The CPU execution loop and timer updates are handled separately so that the timers maintain their required frequency rather than depending directly on how quickly CPU instructions are executed.

## Audio

When the CHIP-8 sound timer is greater than zero, the emulator generates a square-wave tone through SDL2's audio system.

The waveform alternates between a positive and negative amplitude:

```text
 +amplitude ──────
                 │
                 │
 -amplitude ──────
```

The frequency determines how quickly the waveform cycles, while the amplitude determines the strength of the generated signal.

A persistent phase value is used so that the waveform continues smoothly between audio buffers rather than restarting at the beginning of every buffer.

## Testing

The project includes a dedicated test suite covering important emulator behavior such as:

* Register assignment and arithmetic
* Carry and borrow behavior
* Bitwise operations
* Register shifts
* Conditional instructions
* Jumps
* Subroutines and returns
* Timers
* Keyboard input
* Display behavior
* Audio behavior

The tests are compiled separately from the emulator.

### Build the tests

From the project root:

```powershell
g++ -std=c++17 tests/test_chip8.cpp src/Chip8.cpp -o chip8_tests.exe
```

Run them with:

```powershell
.\chip8_tests.exe
```

## Building

### Requirements

* C++17-compatible compiler
* SDL2
* Windows/Linux/macOS

### Compile

With SDL2 configured in your environment:

```powershell
g++ -std=c++17 src/main.cpp src/Chip8.cpp -o chip8.exe
```

### Run

```powershell
.\chip8.exe
```

The emulator loads a CHIP-8 ROM from the `roms` directory.

## Project Structure

```text
CHIP-8-Emulator/
│
├── src/
│   ├── Chip8.h
│   ├── Chip8.cpp
│   └── main.cpp
│
├── tests/
│   └── test_chip8.cpp
│
├── roms/
│   └── *.ch8
│
└── README.md
```

## What I Learned

Building this emulator required implementing a small virtual computer from the ground up rather than relying on an existing emulator framework.

The project gave me practical experience with:

* C++ classes and encapsulation
* `std::array` and fixed-size memory structures
* Bitwise operations and bit masking
* Binary and hexadecimal representations
* Instruction decoding
* CPU fetch/decode/execute architecture
* Program counters and stack-based control flow
* Memory addressing
* Timers and real-time execution
* Keyboard event handling
* Framebuffer rendering
* SDL2 graphics and audio
* Automated testing and debugging
* Git and GitHub version control in Visual Studio

## Future Improvements

Possible future improvements include:

* Support for additional CHIP-8 ROM compatibility quirks
* Configurable keyboard mappings
* More robust ROM validation
* Improved audio configuration
* More configurable display scaling
* Additional integration tests using complete CHIP-8 ROMs

## Author

**Israel Anuku**

Electrical Engineering Co-op Student
McMaster University

This project was created as a personal systems programming project to develop stronger C++ and emulator/embedded-systems fundamentals.
