#include "screen.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void cleanup_window(SDL_Window *window, SDL_Renderer *renderer,
                    SDL_Texture *texture) {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void error(const char *message) {
  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s: %s\n", message, SDL_GetError());
  exit(EXIT_FAILURE);
}

void init_window(Window *window, int width, int height) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    error("SDL could not initialize!");
  }

  window->sdl_window = SDL_CreateWindow("Chip-8", width, height, 0);
  if (window->sdl_window == NULL) {
    error("Could not create window");
  }

  window->sdl_renderer = SDL_CreateRenderer(window->sdl_window, NULL);
  if (window->sdl_renderer == NULL) {
    SDL_DestroyWindow(window->sdl_window);
    error("Could not create renderer");
  }

  window->sdl_texture =
      SDL_CreateTexture(window->sdl_renderer, SDL_PIXELFORMAT_RGBX8888,
                        SDL_TEXTUREACCESS_STREAMING, 64, 32);
  if (window->sdl_texture == NULL) {
    SDL_DestroyRenderer(window->sdl_renderer);
    SDL_DestroyWindow(window->sdl_window);
    error("Could not create texture");
  }

  window->cleanup = cleanup_window;
}

void build_texture(Window *window, CPU *cpu) {
  uint32_t *pixels = NULL;
  uint32_t pitch = 0;
  if (!SDL_LockTexture(window->sdl_texture, NULL, (void **)&pixels,
                       (int *)&pitch)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_LockTexture Error: %s\n",
                 SDL_GetError());
    exit(EXIT_FAILURE);
  }

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      int index = y * 64 + x;
      pixels[index] = cpu->graphics[index] == 1 ? 0xFFFFFFFF : 0x000000FF;
    }
  }
  SDL_UnlockTexture(window->sdl_texture);
}
