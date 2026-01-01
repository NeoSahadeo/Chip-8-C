#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef DISPLAY_H
#define DISPLAY_H

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

// typedef struct Display Display;
//
// typedef struct Display {
//   uint8_t* buffer;
//   uint8_t (*get_pixel)(Display* d, size_t index);
//   void (*set_pixel)(Display* d, size_t index, uint8_t value);
//   void (*clear)(Display* d);
// } Display;

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
} DisplayCtx;

// Display* create_display();
DisplayCtx* create_display();
void destroy_display(DisplayCtx* ctx);

bool get_pixel(uint8_t x, uint8_t y);
void set_pixel(uint8_t x, uint8_t y, bool v);
#endif
