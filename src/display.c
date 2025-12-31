#include "include/display.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/cpu.h"

void clear(Display* d) {
  memset(d->buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
}

uint8_t get_pixel(Display* d, size_t index) {
  return d->buffer[index];
};

void set_pixel(Display* d, size_t index, uint8_t value) {
  d->buffer[index] = value;
};

Display* create_display() {
  Display* display = malloc(sizeof(Display));
  display->set_pixel = set_pixel;
  display->get_pixel = get_pixel;
  display->clear = clear;
  display->buffer = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint8_t));
  return display;
}

DisplayCtx* init_display() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s",
                 SDL_GetError());
    exit(1);
  };

  SDL_Window* window =
      SDL_CreateWindow("SDL", SCREEN_WIDTH, SCREEN_HEIGHT,
                       SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    exit(2);
  };

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    exit(3);
  }

  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    exit(4);
  }

  DisplayCtx* ctx = malloc(sizeof(DisplayCtx));
  ctx->window = window;
  ctx->renderer = renderer;
  ctx->texture = texture;
  return ctx;
}

void render_display(DisplayCtx* ctx, CPU* cpu) {
  void* pixels;
  int pitch;
  if (!SDL_LockTexture(ctx->texture, NULL, &pixels, &pitch)) {
    printf("SDL error: %s\n", SDL_GetError());
    return;
  }

  uint32_t* p = (uint32_t*)pixels;
  int row_len = pitch / sizeof(uint32_t);

  for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
      size_t index = y * SCREEN_WIDTH + x;
      uint8_t pixel = cpu->display->buffer[index];
      uint32_t color = pixel ? 0xFFFFFFFF : 0x000000FF;
      p[y * row_len + x] = color;
    }
  }

  SDL_UnlockTexture(ctx->texture);

  SDL_RenderClear(ctx->renderer);
  SDL_RenderTexture(ctx->renderer, ctx->texture, NULL, NULL);
  SDL_RenderPresent(ctx->renderer);
}
