#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int platform_init(Platform *p, int scale){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
        printf("SDL_Init Error; %s\n", SDL_GetError());
        return 0;
    }

    p->window = SDL_CreateWindow("CHIP-8 Emulator",
                              0, 0,
                              64 * scale, 32 * scale,
                              SDL_WINDOW_SHOWN);
    if (!p->window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    p->renderer = SDL_CreateRenderer(p->window,
                                    -1,
                                    SDL_RENDERER_ACCELERATED
                                );
    if (!p->renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(p->window);
        SDL_Quit();
        return 0;
    }
    
    p->texture = SDL_CreateTexture(p->renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    64, 32);
    if (!p->texture) {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(p->renderer);
        SDL_DestroyWindow(p->window);
        SDL_Quit();
        return 0;
    }
    
    return 1;
}

void platform_update(Platform *p, Chip8 *c, int pitch){
    SDL_UpdateTexture(p->texture, NULL, c->video, pitch);
    SDL_RenderClear(p->renderer);
    SDL_RenderCopy(p->renderer, p->texture, NULL, NULL);
    SDL_RenderPresent(p->renderer);
}   

bool platform_input(uint8_t *keys){
    bool quit = false;

    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                quit = true;
                break;
            
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    
                    case SDLK_x:
                        keys[0] = 1;
                        break;
                    
                    case SDLK_1:
                        keys[1] = 1;
                        break;

                    case SDLK_2:
						keys[2] = 1;
						break;

					case SDLK_3:
						keys[3] = 1;
						break;

					case SDLK_q:
						keys[4] = 1;
						break;

					case SDLK_w:
						keys[5] = 1;
						break;

					case SDLK_e:
						keys[6] = 1;
						break;

					case SDLK_a:
						keys[7] = 1;
						break;

					case SDLK_s:
						keys[8] = 1;
						break;

					case SDLK_d:
						keys[9] = 1;
						break;

					case SDLK_z:
						keys[0xA] = 1;
						break;

					case SDLK_c:
						keys[0xB] = 1;
						break;

					case SDLK_4:
						keys[0xC] = 1;
						break;

					case SDLK_r:
						keys[0xD] = 1;
						break;

					case SDLK_f:
						keys[0xE] = 1;
						break;

					case SDLK_v:
						keys[0xF] = 1;
						break;
                }

            }break;

            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym){
                    
                    case SDLK_x:
                        keys[0] = 0;
                        break;
                    
                    case SDLK_1:
                        keys[1] = 0;
                        break;

                    case SDLK_2:
						keys[2] = 0;
						break;

					case SDLK_3:
						keys[3] = 0;
						break;

					case SDLK_q:
						keys[4] = 0;
						break;

					case SDLK_w:
						keys[5] = 0;
						break;

					case SDLK_e:
						keys[6] = 0;
						break;

					case SDLK_a:
						keys[7] = 0;
						break;

					case SDLK_s:
						keys[8] = 0;
						break;

					case SDLK_d:
						keys[9] = 0;
						break;

					case SDLK_z:
						keys[0xA] = 0;
						break;

					case SDLK_c:
						keys[0xB] = 0;
						break;

					case SDLK_4:
						keys[0xC] = 0;
						break;

					case SDLK_r:
						keys[0xD] = 0;
						break;

					case SDLK_f:
						keys[0xE] = 0;
						break;

					case SDLK_v:
						keys[0xF] = 0;
						break;
                }

            }break;
            }
        }
    

    return quit;
}

void platform_cleanup(Platform *p) {
    if (p->texture) SDL_DestroyTexture(p->texture);
    if (p->renderer) SDL_DestroyRenderer(p->renderer);
    if (p->window) SDL_DestroyWindow(p->window);
    SDL_Quit();
}
