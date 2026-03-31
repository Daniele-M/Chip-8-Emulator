#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define FONTSIZE 80
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

typedef struct{
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t keypad[16];
    uint32_t video[64 * 32];
    uint16_t opcode;
} Chip8;

void chip8_init(Chip8 *chip8);
void chip8_load_rom(Chip8 *chip8);
uint8_t random_byte(void);

#endif