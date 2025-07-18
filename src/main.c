#include "cpu.h"
#include "loadrom.h"
#include "screen.h"
#include <SDL3/SDL_oldnames.h>
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

  init_window(&window, 64, 32);
  init_cpu(&cpu);
  load_rom(argv[1], &cpu);

  SDL_FRect rect = {0, 0, 64, 32};
  bool done = false;
  while (!done) {
    cycle(&cpu);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_SetRenderLogicalPresentation(window.sdl_renderer, 64, 32,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_RenderClear(window.sdl_renderer);

    build_texture(&window, &cpu);

    SDL_RenderTexture(window.sdl_renderer, window.sdl_texture, NULL, &rect);
    SDL_RenderPresent(window.sdl_renderer);
    SDL_Delay(16); // ~60fps
  }

  window.cleanup(window.sdl_window, window.sdl_renderer, window.sdl_texture);

  return 0;
}
