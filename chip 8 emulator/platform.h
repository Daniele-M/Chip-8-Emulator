#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "chip8.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Platform;

int platform_init(Platform *p, int scale);
void platform_update(Platform *p, Chip8 *c, int pitch);
bool platform_input(uint8_t *keys);
void platform_cleanup(Platform *p);

#endif