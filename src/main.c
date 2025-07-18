#include "cpu.h"
#include "loadrom.h"
#include "screen.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Missing ROM file!\n");
    exit(EXIT_FAILURE);
  }

  CPU cpu;
  Window window;

  init_window(&window, 1024, 512);
  init_cpu(&cpu);
  load_rom(argv[1], &cpu);
  cpu.memory[0x200] = 0x00;
  cpu.memory[0x201] = 0xE0;

  bool done = false;
  while (!done) {
    // cycle(&cpu);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);

    build_texture(&window, &cpu);

    SDL_FRect dest = {
        0,
        0,
        1024,
        512,
    };
    SDL_RenderTexture(window.sdl_renderer, window.sdl_texture, NULL, &dest);
    SDL_RenderPresent(window.sdl_renderer);
    sleep(1);
  }

  window.cleanup(window.sdl_window, window.sdl_renderer, window.sdl_texture);

  return 0;
}
