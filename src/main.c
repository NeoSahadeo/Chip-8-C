#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/cpu.h"
#include "include/display.h"

const int SCREEN_WIDTH = 64 * 10;
const int SCREEN_HEIGHT = 32 * 10;

void render_display(DisplayCtx* ctx, CPU* cpu);

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Requires a ROM!\n");
    return 0;
  }

  CPU* cpu = init_cpu();
  load_rom(cpu, argv[1]);

  DisplayCtx* ctx = create_display();

  SDL_Event event;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
        break;
      }
    }
    cycle(cpu);
    render_display(ctx, cpu);
    SDL_Delay(32);
  }

  destroy_display(ctx);
  free(cpu);
  return 0;
}
