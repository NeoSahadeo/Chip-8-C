#include "include/display.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/cpu.h"

bool get_pixel(uint8_t x, uint8_t y) {
  return false;
}

void set_pixel(uint8_t x, uint8_t y, bool v) {}

DisplayCtx* create_display() {
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

void destroy_display(DisplayCtx* ctx) {
  SDL_DestroyWindow(ctx->window);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_Quit();
  free(ctx);
}

const int chip_w = 64;
const int chip_h = 32;
const int scale_x = 640 / chip_w;
const int scale_y = 320 / chip_h;

void render_display(DisplayCtx* ctx, CPU* cpu) {
  void* pixels;
  int pitch;
  if (!SDL_LockTexture(ctx->texture, NULL, &pixels, &pitch)) {
    printf("SDL error: %s\n", SDL_GetError());
    return;
  }

  uint32_t* p = (uint32_t*)pixels;
  int row_len = pitch / sizeof(uint32_t);

  for (int cy = 0; cy < chip_h; ++cy) {
    for (int cx = 0; cx < chip_w; ++cx) {
      uint32_t color = cpu->vram[cy * chip_w + cx] ? 0xFFFFFFFF : 0xFF000000;

      // Top-left of this CHIP-8 pixel in texture space
      int sx0 = cx * scale_x;
      int sy0 = cy * scale_y;

      // Fill the scaled block
      for (int oy = 0; oy < scale_y; ++oy) {
        uint32_t* row = p + (sy0 + oy) * row_len + sx0;
        for (int ox = 0; ox < scale_x; ++ox) {
          row[ox] = color;
        }
      }
    }
  }

  SDL_UnlockTexture(ctx->texture);

  SDL_RenderClear(ctx->renderer);
  SDL_RenderTexture(ctx->renderer, ctx->texture, NULL, NULL);
  SDL_RenderPresent(ctx->renderer);
}
