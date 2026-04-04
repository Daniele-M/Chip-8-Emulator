#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


static Chip8Func table[0xF + 1];
static Chip8Func table0[0xE + 1];
static Chip8Func table8[0xE + 1];
static Chip8Func tableE[0xE + 1];
static Chip8Func tableF[0x65 +1];

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

//LD Vx, byte
void OP_6xkk(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t byte = c->opcode & 0x00FFu;

    c->registers[Vx] = byte;
}

//ADD Vx, byte
void OP_7xkk(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t byte = c->opcode & 0x00FFu;

    c->registers[Vx] += byte;
}

//LD Vx, Vy
void OP_8xy0(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    c->registers[Vx] = c->registers[Vy];
}

//OR Vx, Vy
void OP_8xy1(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    c->registers[Vx] |= c->registers[Vy];
}

//AND Vx, Vy
void OP_8xy2(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    c->registers[Vx] &= c->registers[Vy];
}

//XOR Vx, Vy
void OP_8xy3(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    c->registers[Vx] ^= c->registers[Vy];
}

//ADD Vx, Vy
void OP_8xy4(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;
    
    int16_t sum = c->registers[Vx] + c->registers[Vy];
    if (sum > 255u){
        c->registers[0xF] = 1; 
    }
    else{
        c->registers[0xF] = 0;
    }
    c->registers[Vx] = sum & 0x00FFu;
}

//SUB Vx, Vy
void OP_8xy5(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;
    
    if (c->registers[Vx] > c->registers[Vy]){
        c->registers[0xF] = 1;
    }
    else{
        c->registers[0xF] = 0;
    }
    c->registers[Vx] -= c->registers[Vy];
}

//SHR Vx
void OP_8xy6(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;

    c->registers[0xF] = c->registers[Vx] & 0x1u;
    c->registers[Vx] >>= 1;
}

//SUBN Vx, Vy
void OP_8xy7(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    if (c->registers[Vy] > c->registers[Vx]){
        c->registers[0xF] = 1;
    }
    else{
        c->registers[0xF] = 0;
    }
    c->registers[Vx] = c->registers[Vy] - c->registers[Vx];
}

//SHL Vx {, Vy}
void OP_8xyE(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;

    c->registers[0xF] = (c->registers[Vx] & 0x80u) >> 7u;
    c->registers[Vx] <<= 1;
}

//SNE Vx, Vy
void OP_9xy0(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;

    if (c->registers[Vx] != c->registers[Vy]){
        c->pc += 2;
    }
}

//LD I, addr
void OP_Annn(Chip8 *c){
    uint16_t address = c->opcode & 0x0FFFu;

    c->index = address;
}

//JP V0, addr
void OP_Bnnn(Chip8 *c){
    uint16_t address = c->opcode & 0x0FFFu;

    c->pc = c->registers[0] + address;
}

//RND Vx, byte
void OP_Cxkk(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t byte = c->opcode & 0x00FFu;

    c->registers[Vx] = random_byte() & byte;
}

//DRW Vx, Vy, nibble
//Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void OP_Dxyn(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (c->opcode & 0x00F0u) >> 4u;
    uint8_t height = c->opcode & 0x000Fu;

    //Wrap around
    uint8_t xPos = c->registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = c->registers[Vy] % VIDEO_HEIGHT;

    c->registers[0xF] = 0;
    for (unsigned int row = 0; row < height; row++){
        uint8_t spriteByte = c->memory[c->index + row];
        for (unsigned int col = 0; col < 8; col++){
            uint8_t spritePixel = spriteByte & (0x80u >> col); //get (row,col) pixel
            uint32_t* screenPixel = &(c->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)]);

            if (spritePixel){

                if (*screenPixel == 0xFFFFFFFF){
                    c->registers[0xF] = 1;
                }
                
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

//SKP Vx
void OP_Ex9E(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t key = c->registers[Vx];

    if (c->keypad[key]){
        c->pc += 2;
    }
}

//SKNP Vx
void OP_ExA1(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t key = c->registers[Vx];

    if (!c->keypad[key]){
        c->pc += 2;
    }
}

//LD Vx, DT
void OP_Fx07(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    c->registers[Vx] = c->delayTimer;
}

//LD Vx, K
void OP_Fx0A(Chip8 *c){
	uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;

	for (unsigned int key = 0; key < 16; ++key){
        if (c->keypad[key]){
            c->registers[Vx] = key;
            return;
        }
    }
    c->pc -= 2;
}

//LD DT, Vx
void OP_Fx15(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    c->delayTimer = c->registers[Vx];
}

//LD ST, Vx
void OP_Fx18(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    c->soundTimer = c->registers[Vx];
}

//ADD I, Vx
void OP_Fx1E(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    c->index += c->registers[Vx];
}

//LD F, Vx
void OP_Fx29(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t digit = c->registers[Vx];
    
    c->index = START_ADDRESS + (digit * 5);
}

//LD B, Vx
void OP_Fx33(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    uint8_t value = c->registers[Vx];
    
    c->memory[c->index + 2] = value % 10;
    value /= 10;

    c->memory[c->index + 1] = value % 10;
    value /= 10;

    c->memory[c->index] = value % 10;
}

//LD [I], Vx
void OP_Fx55(Chip8 *c){
    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    for (uint8_t i = 0; i <= Vx; ++i){
        c->memory[c->index + i] = c->registers[i];
    }
}

//LD Vx, [I]
void OP_Fx65(Chip8 *c){

    uint8_t Vx = (c->opcode & 0x0F00u) >> 8u;
    
    for (uint8_t i = 0; i <= Vx; ++i){
        c->registers[i] = c->memory[c->index + i];
    }
}

void OP_NULL(Chip8 *c){
}

//Dispatchers
void Table0(Chip8 *c){
    table0[c->opcode & 0x000Fu](c);
}

void Table8(Chip8 *c){
    table8[c->opcode & 0x000Fu](c);
}

void TableE(Chip8 *c){
    tableE[c->opcode & 0x000Fu](c);
}

void TableF(Chip8 *c){
    tableF[c->opcode & 0x00FFu](c);
}

//Initialization
void chip8_init_tables(){

    table[0x0] = Table0;
    table[0x1] = OP_1nnn;
    table[0x2] = OP_2nnn;
    table[0x3] = OP_3xkk;
    table[0x4] = OP_4xkk;
    table[0x5] = OP_5xy0;
    table[0x6] = OP_6xkk;
    table[0x7] = OP_7xkk;
    table[0x8] = Table8;
    table[0x9] = OP_9xy0;
    table[0xA] = OP_Annn;
    table[0xB] = OP_Bnnn;
    table[0xC] = OP_Cxkk;
    table[0xD] = OP_Dxyn;
    table[0xE] = TableE;
    table[0xF] = TableF;

    for (size_t i = 0; i <= 0xE; i++){
        table0[i] = OP_NULL;
        table8[i] = OP_NULL;
        tableE[i] = OP_NULL;
    }

    table0[0x0] = OP_00E0;
    table0[0xE] = OP_00EE;

    table8[0x0] = OP_8xy0;
    table8[0x1] = OP_8xy1;
    table8[0x2] = OP_8xy2;
    table8[0x3] = OP_8xy3;
    table8[0x4] = OP_8xy4;
    table8[0x5] = OP_8xy5;
    table8[0x6] = OP_8xy6;
    table8[0x7] = OP_8xy7;
    table8[0xE] = OP_8xyE;

    tableE[0x1] = OP_ExA1;
    tableE[0xE] = OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++){
        tableF[i] = OP_NULL;
    }

    tableF[0x07] = OP_Fx07;
    tableF[0x0A] = OP_Fx0A;
    tableF[0x15] = OP_Fx15;
    tableF[0x18] = OP_Fx18;
    tableF[0x1E] = OP_Fx1E;
    tableF[0x29] = OP_Fx29;
    tableF[0x33] = OP_Fx33;
    tableF[0x55] = OP_Fx55;
    tableF[0x65] = OP_Fx65;

}

void chip8_cycle(Chip8 *c){
    //Fetch the opcode
    c->opcode = (c->memory[c->pc] << 8u) | c->memory[c->pc + 1];

    c->pc += 2;

    //Decode and execute the opcode
    table[(c->opcode & 0xF000u) >> 12u](c);

    if (c->delayTimer > 0){
        --c->delayTimer;
    }

    if (c->soundTimer > 0){
        --c->soundTimer;
    }

}