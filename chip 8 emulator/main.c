#include "chip8.h"
#include "platform.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv){
    if (argc != 4)
    {
        printf("Usage: %s <Scale> <Delay> <ROM>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int videoScale = atoi(argv[1]);
    int cycleDelay = atoi(argv[2]);
    const char* romFilename = argv[3];

    Platform platform;
    if (!platform_init(&platform, videoScale))
    {
        return EXIT_FAILURE;
    }

    Chip8 chip8;
    chip8_init(&chip8);
    chip8.video[0] = 0xFFFFFFFF;
    chip8_init_tables();
    chip8_load_rom(&chip8, romFilename);

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    uint32_t lastCycleTime = SDL_GetTicks();
    int quit = 0;

    while (!quit)
    {
        quit = platform_input(chip8.keypad);

        uint32_t currentTime = SDL_GetTicks();
        uint32_t dt = currentTime - lastCycleTime;

        if (dt > (uint32_t)cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8_cycle(&chip8);

            platform_update(&platform, &chip8, videoPitch);
        }
    }

    platform_cleanup(&platform);

    return 0;

}