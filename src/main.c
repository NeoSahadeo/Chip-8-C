#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/cpu.h"
#include "include/data.h"
#include "include/display.h"

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Requires a ROM!\n");
    return 0;
  }

  DisplayCtx* ctx = init_display();
  CPU cpu;
  cpu.reg_PC = 0x200;
  cpu.display = create_display();
  for (uint8_t i = 0; i < 80; i++)
    cpu.memory[i] = font[i];

  FILE* fp = fopen(argv[1], "rb");
  if (!fp) {
    printf("File opening failed\n");
    return 1;
  }

  int byte;
  size_t c = 0;
  while ((byte = fgetc(fp)) != EOF && c < 0xFFE - 0x200) {
    cpu.memory[0x200 + c++] = (uint8_t)byte;
  }

  SDL_Event event;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
        break;
      }
    }

    cycle(&cpu);

    render_display(ctx, &cpu);

    SDL_Delay(16);
  }

  SDL_DestroyRenderer(ctx->renderer);
  SDL_Quit();
  free(cpu.display->buffer);
  free(cpu.display);
  free(ctx);
  return 0;
}
