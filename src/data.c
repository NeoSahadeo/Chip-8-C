#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/cpu.h"

void load_rom(CPU* cpu, char* dir) {
  FILE* fp = fopen(dir, "rb");

  if (!fp) {
    printf("File opening failed\n");
    exit(EXIT_FAILURE);
  }

  int byte;
  size_t c = 0;
  while ((byte = fgetc(fp)) != EOF && c < 0xFFE - 0x200) {
    cpu->memory[0x200 + c++] = (uint8_t)byte;
  }
  fclose(fp);
}

uint8_t sdl_to_chip8(SDL_Keycode key) {
  switch (key) {
    case SDLK_1:
      return 0x1;
    case SDLK_2:
      return 0x2;
    case SDLK_3:
      return 0x3;
    case SDLK_C:
      return 0xC;
    case SDLK_4:
      return 0x4;
    case SDLK_5:
      return 0x5;
    case SDLK_6:
      return 0x6;
    case SDLK_D:
      return 0xD;
    case SDLK_7:
      return 0x7;
    case SDLK_8:
      return 0x8;
    case SDLK_9:
      return 0x9;
    case SDLK_E:
      return 0xE;
    case SDLK_A:
      return 0xA;
    case SDLK_0:
      return 0x0;
    case SDLK_B:
      return 0xB;
    case SDLK_F:
      return 0xF;
    default:
      return 0xFF;
  }
}

int font[80] = {
    // 0
    0xf0,
    0x90,
    0x90,
    0x90,
    0xf0,
    // 1
    0x20,
    0x60,
    0x20,
    0x20,
    0x70,
    // 2
    0xf0,
    0x10,
    0xf0,
    0x80,
    0xf0,
    // 3
    0xf0,
    0x10,
    0xf0,
    0x10,
    0xf0,
    // 4
    0x90,
    0x90,
    0xf0,
    0x10,
    0x10,
    // 5
    0xf0,
    0x80,
    0xf0,
    0x10,
    0xf0,
    // 6
    0xf0,
    0x80,
    0xf0,
    0x90,
    0xf0,
    // 7
    0xf0,
    0x10,
    0x20,
    0x40,
    0x40,
    // 8
    0xf0,
    0x90,
    0xf0,
    0x90,
    0xf0,
    // 9
    0xf0,
    0x90,
    0xf0,
    0x10,
    0xf0,
    // A
    0xf0,
    0x90,
    0xf0,
    0x90,
    0x90,
    // B
    0xe0,
    0x90,
    0xe0,
    0x90,
    0xe0,
    // C
    0xf0,
    0x80,
    0x80,
    0x80,
    0xf0,
    // D
    0xe0,
    0x90,
    0x90,
    0x90,
    0xe0,
    // E
    0xf0,
    0x80,
    0xf0,
    0x80,
    0xf0,
    // F
    0xf0,
    0x80,
    0xf0,
    0x80,
    0x80,
};
