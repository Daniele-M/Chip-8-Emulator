#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


const uint8_t fontset[FONTSIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *chip8){
    memset(chip8, 0, sizeof(Chip8));
    chip8->pc = START_ADDRESS;

    for (unsigned int i = 0; i < FONTSIZE; i++){
        chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void chip8_load_rom(Chip8 *chip8, const char *filename){

    FILE *file = fopen(filename, "rb");

    if (file != NULL){
        //Get size of the file and create a buffer
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        char *buffer = (char*)malloc(size);
        
        //Fill the buffer with the file contents and close the file
        rewind(file);
        fread(buffer, sizeof(char), size, file);
        fclose(file);

        //Load the Chip8 memory with the ROM contents
        for (long i = 0; i < size; i++){
            chip8->memory[START_ADDRESS + i] = buffer[i];
        }

        free(buffer);
    }
    else{
        printf("Error: file %s not found\n", filename);
    }

}

uint8_t random_byte(void){
    return (uint8_t)(rand() % 256);
}

//INSTRUCTIONS

//CLS clear the display
void OP_00E0(Chip8 *c){
    memset(c->video, 0, sizeof(c->video));
}

//RET returns
void OP_00EE(Chip8 *c){
    c->sp -=1;
    c->pc = c->stack[c->sp]; 
}

//JP addr
void OP_1nnn(Chip8 *c){
    uint16_t address = c->opcode & 0x0FFFu;
    c->pc = address;
}

//CALL addr
void OP_2nnn(Chip8 *c){
    uint16_t address = c->opcode & 0x0FFFu;
    c->stack[c->sp] = c->pc;
    c->sp += 1;
    c->pc = address;
}

//SE Vx, byte
void OP_3xkk(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t byte = c->opcode & 0x00FFu;
    if (c->registers[Vx] == byte){
        c->pc += 2;
    } 
}

//SNE Vx, byte
void OP_4xkk(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t byte = c->opcode & 0x00FFu;
    if (c->registers[Vx] != byte){
        c->pc += 2;
    } 
}

//SE Vx, Vy
void OP_5xy0(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;
    if (c->registers[Vx] == c->registers[Vy]){
        c->pc += 2;
    } 
}