#pragma once

#include "cpu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

typedef struct {
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  SDL_Texture *sdl_texture;
  void (*cleanup)(SDL_Window *window, SDL_Renderer *renderer,
                  SDL_Texture *texture);
} Window;

void init_window(Window *window, int width, int height);
void build_texture(Window *window, CPU *cpu);
